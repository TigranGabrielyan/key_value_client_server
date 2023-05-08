/**
* @file kvm_request_handler.c
*
* @brief The module contains server request handler library implementation.
*
*/

#include<stdlib.h>

#include <sys/socket.h>

#include "kvm_requests.h"
#include "kvm_replies.h"
#include "kvm_utils.h"

#include "kvm_server_internal.h"

#include "apr_general.h"
#include "apr_pools.h"
#include "apr_hash.h"

apr_hash_t * ht = NULL;

typedef kvm_result_t (*request_handler_t) (uint32_t request_size, const uint8_t * request, uint32_t * reply_size, uint8_t ** reply);

static kvm_result_t handle_put_request(uint32_t request_size, const uint8_t * request, uint32_t * reply_size, uint8_t ** reply);
static kvm_result_t handle_get_request(uint32_t request_size, const uint8_t * request, uint32_t * reply_size, uint8_t ** reply);
static kvm_result_t handle_delete_request(uint32_t request_size, const uint8_t * request, uint32_t * reply_size, uint8_t ** reply);
static kvm_result_t handle_list_request(uint32_t request_size, const uint8_t * request, uint32_t * reply_size, uint8_t ** reply);
static kvm_result_t handle_count_request(uint32_t request_size, const uint8_t * request, uint32_t * reply_size, uint8_t ** reply);

static uint8_t * prepare_reply(uint32_t size, uint32_t * reply_size, uint8_t ** reply);
static kvm_result_t prepare_generic_reply(kvm_reply_status_t status, uint32_t * reply_size, uint8_t ** reply);


request_handler_t handlers[] =
{
    NULL,                   //KVM_REQUST_NOOP
    handle_put_request,     //KVM_REQUST_PUT
    handle_get_request,     //KVM_REQUST_GET
    handle_delete_request,  //KVM_REQUST_DELETE
    handle_list_request,    //KVM_REQUST_LIST
    handle_count_request,   //KVM_REQUST_COUNT
};

kvm_result_t init_apr_hashtable(void)
{
    if (NULL != ht)
    {
        return KVM_RESULT_OK;
    }

    apr_pool_t * pool = NULL;

    apr_status_t status = apr_initialize();
    if (APR_SUCCESS != status)
    {
        return KVM_RESULT_SYS_CALL_FAIL;
    }

    status = apr_pool_create(&pool, NULL);
    if (APR_SUCCESS != status)
    {
        apr_terminate();
        return KVM_RESULT_SYS_CALL_FAIL;
    }

    ht = apr_hash_make(pool);
    if (NULL == ht)
    {
        apr_pool_destroy(pool);
        apr_terminate();
        return KVM_RESULT_SYS_CALL_FAIL;
    }

    return KVM_RESULT_OK;
}

void uninit_apr_hashtable(void)
{
    if (NULL != ht)
    {
        void * key = NULL;
        void * val = NULL;
        for (apr_hash_index_t * hi = apr_hash_first(NULL, ht); hi; hi = apr_hash_next(hi))
        {
            apr_hash_this(hi, (const void **) &key, NULL, &val);
            free(key);
            free(val);
        }

        apr_pool_t * pool = apr_hash_pool_get(ht);
        if (NULL != pool)
        {
            apr_pool_destroy(pool);
        }

        apr_terminate();

        ht = NULL;
    }
}

static uint8_t * prepare_reply(uint32_t size, uint32_t * reply_size, uint8_t ** reply)
{
    size += sizeof(kvm_reply_generic_t);
    uint8_t * r = malloc(size);
    if (NULL == r)
    {
        return NULL;
    }

    ((kvm_reply_generic_t *) r)->status = KVM_REPLY_STATUS_OK;
    *reply_size = size;
    *reply = r;

    return r + sizeof(kvm_reply_generic_t);
}

static kvm_result_t prepare_generic_reply(kvm_reply_status_t status, uint32_t * reply_size, uint8_t ** reply)
{
    uint8_t * r = (uint8_t *) malloc(sizeof(kvm_reply_generic_t));
    if (NULL == r)
    {
        return KVM_RESULT_SYS_CALL_FAIL;
    }

    ((kvm_reply_generic_t *) r)->status = status;
    *reply_size = sizeof(kvm_reply_generic_t);
    *reply = r;

    return KVM_RESULT_OK;
}

kvm_result_t handle_request(uint32_t request_size, const uint8_t * request, uint32_t * reply_size, uint8_t ** reply)
{
    const kvm_request_id_t id = ((const kvm_request_generic_t *) request)->id;
    request_handler_t handler = NULL;
    if (id < sizeof(handlers)/sizeof(handlers[0]))
    {
        handler = handlers[id];
    }

    if (NULL != handler)
    {
        return handler(request_size - sizeof(kvm_request_generic_t), request + sizeof(kvm_request_generic_t), reply_size, reply);
    }

    return prepare_generic_reply(KVM_REPLY_BAD_REQUEST, reply_size, reply);
}

static kvm_result_t
handle_put_request(
    uint32_t        request_size,
    const uint8_t * request,
    uint32_t *      reply_size,
    uint8_t **      reply)
{
    uint32_t key_size;
    uint32_t value_size;

    if (request_size < sizeof(key_size) + sizeof(value_size))
    {
        return prepare_generic_reply(KVM_REPLY_BAD_REQUEST, reply_size, reply);
    }
    request_size -= sizeof(key_size) + sizeof(value_size);

    memcpy(&key_size, request, sizeof(key_size));
    key_size = kvm_util_transport_to_host32(key_size);
    request += sizeof(key_size);
    memcpy(&value_size, request, sizeof(value_size));
    value_size = kvm_util_transport_to_host32(value_size);
    request += sizeof(value_size);

    if (request_size < key_size + value_size)
    {
        return prepare_generic_reply(KVM_REPLY_BAD_REQUEST, reply_size, reply);
    }

    uint8_t * key = (uint8_t *) malloc(key_size);
    if (NULL == key)
    {
        return KVM_RESULT_SYS_CALL_FAIL;
    }
    memcpy(key, request, key_size);

    uint8_t * value = (uint8_t *) malloc(value_size + sizeof(value_size));
    if (NULL == value)
    {
        return KVM_RESULT_SYS_CALL_FAIL;
    }
    memcpy(value, &value_size, sizeof(value_size));
    memcpy(value + sizeof(value_size), request + key_size, value_size);

    apr_hash_set(ht, key, key_size, value);

    return prepare_generic_reply(KVM_REPLY_STATUS_OK, reply_size, reply);
}

static kvm_result_t
handle_get_request(
    uint32_t        request_size,
    const uint8_t * request,
    uint32_t *      reply_size,
    uint8_t **      reply)
{
    uint32_t key_size;

    if (request_size < sizeof(key_size))
    {
        return prepare_generic_reply(KVM_REPLY_BAD_REQUEST, reply_size, reply);
    }
    request_size -= sizeof(key_size);

    memcpy(&key_size, request, sizeof(key_size));
    key_size = kvm_util_transport_to_host32(key_size);

    if (request_size < key_size)
    {
        return prepare_generic_reply(KVM_REPLY_BAD_REQUEST, reply_size, reply);
    }

    const uint8_t * key = request + sizeof(key_size);

    uint8_t * value = apr_hash_get(ht, key, key_size);
    if (NULL == value)
    {
        return prepare_generic_reply(KVM_REPLY_BAD_REQUEST, reply_size, reply);
    }

    const uint32_t r_size = *((uint32_t *) value);
    kvm_reply_get_t * r = (kvm_reply_get_t *) prepare_reply(r_size + sizeof(kvm_reply_get_t), reply_size, reply);
    if (NULL == r)
    {
        return KVM_RESULT_SYS_CALL_FAIL;
    }

    r->value_size = r_size;
    memcpy(r + 1, value + sizeof(uint32_t), r_size);

    return KVM_RESULT_OK;
}

static kvm_result_t
handle_delete_request(
    uint32_t        request_size,
    const uint8_t * request,
    uint32_t *      reply_size,
    uint8_t **      reply)
{
    uint32_t key_size;

    if (request_size < sizeof(key_size))
    {
        return prepare_generic_reply(KVM_REPLY_BAD_REQUEST, reply_size, reply);
    }
    request_size -= sizeof(key_size);

    memcpy(&key_size, request, sizeof(key_size));
    key_size = kvm_util_transport_to_host32(key_size);

    if (request_size < key_size)
    {
        return prepare_generic_reply(KVM_REPLY_BAD_REQUEST, reply_size, reply);
    }

    const uint8_t * key = request + sizeof(key_size);

    uint8_t * value = apr_hash_get(ht, key, key_size);
    if (NULL != value)
    {
        free(value);
    }

    apr_hash_set(ht, key, key_size, NULL);

    return prepare_generic_reply(KVM_REPLY_STATUS_OK, reply_size, reply);
}

static kvm_result_t
handle_list_request(
    uint32_t        request_size,
    const uint8_t * request,
    uint32_t *      reply_size,
    uint8_t **      reply)
{
    if (request_size != 0)
    {
        return prepare_generic_reply(KVM_REPLY_BAD_REQUEST, reply_size, reply);
    }

    uint32_t size = 0;
    for (apr_hash_index_t * hi = apr_hash_first(NULL, ht); hi; hi = apr_hash_next(hi))
    {
        size += apr_hash_this_key_len(hi);
    }

    const uint32_t count = apr_hash_count(ht);

    size += count * sizeof(uint32_t);

    uint8_t * r = prepare_reply(size + sizeof(kvm_reply_list_t), reply_size, reply);
    if (NULL == r)
    {
        return KVM_RESULT_SYS_CALL_FAIL;
    }

    ((kvm_reply_list_t *)r)->count = kvm_util_host_to_transport32(count);
    r += sizeof(kvm_reply_list_t);

    for (apr_hash_index_t * hi = apr_hash_first(NULL, ht); hi; hi = apr_hash_next(hi))
    {
        const uint8_t * key = apr_hash_this_key(hi); 
        const uint32_t key_len = (uint32_t) apr_hash_this_key_len(hi);

        uint32_t tmp = kvm_util_host_to_transport32(key_len);
        memcpy(r, &tmp, sizeof(key_len));
        r += sizeof(key_len);
        memcpy(r, key, key_len);
        r += key_len;
    }

    return KVM_RESULT_OK;
}

static kvm_result_t
handle_count_request(
    uint32_t        request_size,
    const uint8_t * request,
    uint32_t *      reply_size,
    uint8_t **      reply)
{
    if (request_size != 0)
    {
        return prepare_generic_reply(KVM_REPLY_BAD_REQUEST, reply_size, reply);
    }

    kvm_reply_count_t * r = (kvm_reply_count_t *) prepare_reply(sizeof(kvm_reply_count_t), reply_size, reply);
    if (NULL == r)
    {
        return KVM_RESULT_SYS_CALL_FAIL;
    }

    r->count = kvm_util_host_to_transport32(apr_hash_count(ht));
    return KVM_RESULT_OK;
}