/**
* @file kvm_client.c
*
* @brief The module contains client library implementation.
*
*/

#include <stdlib.h>
#include <string.h>
#include "kvm_requests.h"
#include "kvm_replies.h"
#include "kvm_utils.h"
#include "kvm_client.h"
#include "kvm_client_internal.h"

static kvm_request_generic_t * prepare_request(kvm_request_id_t id, uint32_t size)
{
    kvm_request_generic_t * request = (kvm_request_generic_t *) malloc(size);
    if (NULL != request)
    {
        request->id = id;
    }

    return request;
}

kvm_result_t
kvm_client_open(
    kvm_client_handle_t *   h_client,
    const char *            server_ip,
    uint16_t                server_port)
{
    if (NULL == h_client || NULL == server_ip)
    {
        return KVM_RESULT_INVALID_PARAM;
    }

    const kvm_client_handle_t client = (kvm_client_handle_t) malloc(sizeof(struct kvm_client_s));
    if (NULL == client)
    {
        return KVM_RESULT_SYS_CALL_FAIL;
    }

    const kvm_result_t result = kvm_transport_open(&client->h_transport, server_ip, server_port);
    if (KVM_RESULT_OK == result)
    {
        *h_client = client;
    }

    return result;
}

kvm_result_t
kvm_client_close(
    kvm_client_handle_t h_client)
{
    kvm_result_t result = KVM_RESULT_OK;
    if (NULL != h_client)
    {
        result = kvm_transport_close(h_client->h_transport);
        free(h_client);
    }
    return result;
}

kvm_result_t
kvm_client_put(
    kvm_client_handle_t     h_client,
    kvm_const_dlob_data_t * key,
    kvm_const_dlob_data_t * value)
{
    if (NULL == h_client || NULL == key || NULL == value)
    {
        return KVM_RESULT_INVALID_PARAM;
    }

    const uint32_t size = sizeof(kvm_request_generic_t) + sizeof(kvm_request_put_t) + key->size + value->size;
    kvm_request_generic_t * request = prepare_request(KVM_REQUST_PUT, size);
    if (NULL == request)
    {
        return KVM_RESULT_SYS_CALL_FAIL;
    }
    uint8_t * ptr = (uint8_t *) (request + 1);

    /* Setup PUT request specific data. */
    kvm_request_put_t put_req;
    put_req.key_size = kvm_util_host_to_transport32(key->size);
    put_req.value_size = kvm_util_host_to_transport32(value->size);
    memcpy(ptr, &put_req, sizeof(put_req));

    ptr += sizeof(kvm_request_put_t);
    memcpy(ptr, key->data, key->size);
    memcpy(ptr + key->size, value->data, value->size);

    uint32_t reply_size;
    uint8_t * reply;
    kvm_result_t result = kvm_transport_send(h_client->h_transport, size, (uint8_t *) request, &reply_size, &reply);
    if (KVM_RESULT_OK == result)
    {
        if (KVM_REPLY_STATUS_OK != ((kvm_reply_generic_t *) (reply))->status)
        {
            result = KVM_RESULT_CONNECTION_FAIL;
        }
        free(reply);
    }

    return result;
}

kvm_result_t
kvm_client_get(
    kvm_client_handle_t     h_client,
    kvm_const_dlob_data_t * key,
    kvm_data_callback_t     callback,
    void *                  user_context)
{
    if (NULL == h_client || NULL == key || NULL == callback)
    {
        return KVM_RESULT_INVALID_PARAM;
    }

    const uint32_t size = sizeof(kvm_request_generic_t) + sizeof(kvm_request_get_t) + key->size;
    kvm_request_generic_t * request = prepare_request(KVM_REQUST_GET, size);
    if (NULL == request)
    {
        return KVM_RESULT_SYS_CALL_FAIL;
    }
    uint8_t * ptr = (uint8_t *) (request + 1);

    /* Setup GET request specific data. */
    kvm_request_get_t get_req;
    get_req.key_size = kvm_util_host_to_transport32(key->size);
    memcpy(ptr, &get_req, sizeof(get_req));

    ptr += sizeof(kvm_request_get_t);
    memcpy(ptr, key->data, key->size);

    uint32_t reply_size;
    uint8_t * reply;
    kvm_result_t result = kvm_transport_send(h_client->h_transport, size, (uint8_t *) request, &reply_size, &reply);
    if (KVM_RESULT_OK == result)
    {
        uint8_t * ptr = reply;
        if (KVM_REPLY_STATUS_OK != ((kvm_reply_generic_t *) (ptr))->status)
        {
            result = KVM_RESULT_CONNECTION_FAIL;
        }
        else
        {
            kvm_const_dlob_data_t value;

            ptr += sizeof(kvm_reply_generic_t);
            kvm_reply_get_t get_reply;
            memcpy(&get_reply, ptr, sizeof(get_reply));

            value.size = kvm_util_transport_to_host32(get_reply.value_size);
            value.data = ptr + sizeof(kvm_reply_get_t);
            callback(user_context, &value);
        }

        free(reply);
    }

    return result;
}

kvm_result_t
kvm_client_delete(
    kvm_client_handle_t     h_client,
    kvm_const_dlob_data_t * key)
{
    if (NULL == h_client || NULL == key)
    {
        return KVM_RESULT_INVALID_PARAM;
    }

    const uint32_t size = sizeof(kvm_request_generic_t) + sizeof(kvm_request_delete_t) + key->size;
    kvm_request_generic_t * request = prepare_request(KVM_REQUST_DELETE, size);
    if (NULL == request)
    {
        return KVM_RESULT_SYS_CALL_FAIL;
    }

    uint8_t * ptr = (uint8_t *) (request + 1);

    /* Setup DELETE request specific data. */
    kvm_request_delete_t del_req;
    del_req.key_size = kvm_util_host_to_transport32(key->size);
    memcpy(ptr, &del_req, sizeof(del_req));

    ptr += sizeof(kvm_request_delete_t);
    memcpy(ptr, key->data, key->size);

    uint32_t reply_size;
    uint8_t * reply;
    kvm_result_t result = kvm_transport_send(h_client->h_transport, size, (uint8_t *) request, &reply_size, &reply);
    if (KVM_RESULT_OK == result)
    {
        if (KVM_REPLY_STATUS_OK != ((kvm_reply_generic_t *) (reply))->status)
        {
            result = KVM_RESULT_CONNECTION_FAIL;
        }
        free(reply);
    }

    return result;
}

kvm_result_t
kvm_client_list_keys(
    kvm_client_handle_t h_client,
    kvm_data_callback_t callback,
    void *              user_context)
{
    if (NULL == h_client || NULL == callback)
    {
        return KVM_RESULT_INVALID_PARAM;
    }

    const uint32_t size = sizeof(kvm_request_generic_t);
    kvm_request_generic_t * request = prepare_request(KVM_REQUST_LIST, size);
    if (NULL == request)
    {
        return KVM_RESULT_SYS_CALL_FAIL;
    }

    uint32_t reply_size;
    uint8_t * reply;
    kvm_result_t result = kvm_transport_send(h_client->h_transport, size, (uint8_t *) request, &reply_size, &reply);
    if (KVM_RESULT_OK == result)
    {
        uint8_t * ptr = reply;
        if (KVM_REPLY_STATUS_OK != ((kvm_reply_generic_t *) (ptr))->status)
        {
            result = KVM_RESULT_CONNECTION_FAIL;
        }
        else
        {
            kvm_const_dlob_data_t key;

            ptr += sizeof(kvm_reply_generic_t);
            kvm_reply_list_t list_reply;
            memcpy(&list_reply, ptr, sizeof(list_reply));
 
            ptr += sizeof(kvm_reply_list_t);
            for (uint32_t i = 0; i < kvm_util_transport_to_host32(list_reply.count); ++i)
            {
                memcpy(&key.size, ptr, sizeof(key.size));
                key.size = kvm_util_transport_to_host32(key.size);
                ptr += sizeof(uint32_t);

                key.data = ptr;
                ptr += key.size;

                callback(user_context, &key);
            }

            callback(user_context, NULL);
        }
        free(reply);
    }

    return result;
}

kvm_result_t
kvm_client_count(
    kvm_client_handle_t h_client,
    uint32_t *          count)
{
    if (NULL == h_client || NULL == count)
    {
        return KVM_RESULT_INVALID_PARAM;
    }

    const uint32_t size = sizeof(kvm_request_generic_t);
    kvm_request_generic_t * request = prepare_request(KVM_REQUST_COUNT, size);
    if (NULL == request)
    {
        return KVM_RESULT_SYS_CALL_FAIL;
    }

    uint32_t reply_size;
    uint8_t * reply;
    kvm_result_t result = kvm_transport_send(h_client->h_transport, size, (uint8_t *) request, &reply_size, &reply);
    if (KVM_RESULT_OK == result)
    {
        uint8_t * ptr = reply;
        if (KVM_REPLY_STATUS_OK != ((kvm_reply_generic_t *) (ptr))->status)
        {
            result = KVM_RESULT_CONNECTION_FAIL;
        }
        else
        {
            ptr += sizeof(kvm_reply_generic_t);
            kvm_reply_count_t count_reply;
            memcpy(&count_reply, ptr, sizeof(count_reply));
            *count = kvm_util_transport_to_host32(count_reply.count);
        }
        free(reply);
    }

    return result;
}