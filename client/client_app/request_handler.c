#include <stdio.h>
#include <stdlib.h>
#include "request_handler.h"

#include "apr_general.h"
#include "apr_pools.h"
#include "apr_hash.h"

typedef int (*request_handler_t) (kvm_client_handle_t, const char *, const char *);

static void init_apr_hashtable(void);
static void uninit_apr_hashtable(void);

static int parse_request(char * input_line, char ** request, char ** key, char ** value);
static void callback(void * context, const kvm_const_dlob_data_t * data);

static int handle_put_request(const kvm_client_handle_t h_client, const char * key, const char * value);
static int handle_get_request(const  kvm_client_handle_t h_client, const char * key, const char * value);
static int handle_del_request(const kvm_client_handle_t h_client, const char * key, const char * value);
static int handle_list_keys_request(const kvm_client_handle_t h_client, const char * key, const char * value);
static int handle_count_request(const kvm_client_handle_t h_client, const char * key, const char * value);
static int handle_quit_request(const kvm_client_handle_t h_client, const char * key, const char * value);

apr_hash_t * ht = NULL;

int init_request_handler(void)
{
    if (NULL != ht)
    {
        return 1;
    }

    init_apr_hashtable();
    if (NULL == ht)
    {
        printf("Hash Table initialization failed\n");
        return 0;
    }

    apr_hash_set(ht, "put", APR_HASH_KEY_STRING, (void *) handle_put_request);
    apr_hash_set(ht, "get", APR_HASH_KEY_STRING, (void *) handle_get_request);
    apr_hash_set(ht, "del", APR_HASH_KEY_STRING, (void *) handle_del_request);
    apr_hash_set(ht, "list-keys", APR_HASH_KEY_STRING, (void *) handle_list_keys_request);
    apr_hash_set(ht, "count", APR_HASH_KEY_STRING, (void *) handle_count_request);
    apr_hash_set(ht, "quit", APR_HASH_KEY_STRING, (void *) handle_quit_request);

    return 1;
}

void uninit_request_handler(void)
{
    uninit_apr_hashtable();
}

int handle_request(const kvm_client_handle_t h_client, char * input_line)
{
    char * request = NULL;
    char * key = NULL;
    char * value = NULL;

    if (!parse_request(input_line, &request, &key, &value))
    {
        printf("invalid input. Please try again\n");
        return 1;
    }

     const request_handler_t handler = (request_handler_t) apr_hash_get(ht, request, APR_HASH_KEY_STRING);
     if (NULL == handler)
     {
        printf("invalid input. Please try again\n");
        return 1;
     }

     return handler(h_client, key, value);
}

static void init_apr_hashtable(void)
{
    apr_pool_t * pool = NULL;

    apr_status_t status = apr_initialize();
    if (APR_SUCCESS != status)
    {
        return;
    }

    status = apr_pool_create(&pool, NULL);
    if (APR_SUCCESS != status)
    {
        apr_terminate();
        return;
    }

    ht = apr_hash_make(pool);
    if (NULL == ht)
    {
        apr_pool_destroy(pool);
        apr_terminate();
    }
}

static void uninit_apr_hashtable(void)
{
    if (NULL != ht)
    {
        apr_pool_t * pool = apr_hash_pool_get(ht);
        if (NULL != pool)
        {
            apr_pool_destroy(pool);
        }

        apr_terminate();

        ht = NULL;
    }
}

static int parse_request(char * input_line, char ** request, char ** key, char ** value)
{
    *request = input_line;

    char * ptr = strchr(input_line, '\n');
    if (NULL != ptr)
    {
        *ptr = '\0';
    }

    ptr = strchr(input_line, ' ');
    if (NULL == ptr)
    {
        // Possible request with no extra arguments. Will be further verified.
        return 1;
    }

    *ptr = '\0';
    input_line = ptr + 1;
    *key = input_line;

    ptr = strchr(input_line, '=');
    if (NULL == ptr)
    {
        // Possible request with key only. Will be further verified.
        return 1;
    }

    *ptr = '\0';
    *value = ptr + 1;

    return 1;
}

static void callback(void * context, const kvm_const_dlob_data_t * data)
{
    if (NULL == data)
    {
        return;
    }
    char * str = malloc(data->size + 1);
    memcpy(str, data->data, data->size);
    str[data->size] = '\0';
    printf("%s\n", str);
    free(str);
}

static int handle_put_request(const kvm_client_handle_t h_client, const char * key, const char * value)
{
    if (NULL == key || NULL == value)
    {
        printf("invalid input. Please try again\n");
        return 1;
    }

    kvm_const_dlob_data_t key_blob;
    kvm_const_dlob_data_t value_blob;

    key_blob.size = (uint32_t) strlen(key);
    key_blob.data = (const uint8_t *) key;

    value_blob.size = (uint32_t) strlen(value);
    value_blob.data = (const uint8_t *) value;

    const kvm_result_t result = kvm_client_put(h_client, &key_blob, &value_blob);
    if (KVM_RESULT_OK != result)
    {
        printf("kvm_client_get failed: error %d\n", result);
    }
    else
    {
        printf("Key/Value pair successfully stored\n");
    }

    return 1;
}

static int handle_get_request(const kvm_client_handle_t h_client, const char * key, const char * value)
{
    if (NULL == key || NULL != value)
    {
        printf("invalid input. Please try again\n");
        return 1;
    }

    kvm_const_dlob_data_t key_blob;

    key_blob.size = (uint32_t) strlen(key);
    key_blob.data = (const uint8_t *) key;
    const kvm_result_t result = kvm_client_get(h_client, &key_blob,  callback, NULL);
    if (KVM_RESULT_OK != result)
    {
        printf("handle_get_request failed: error %d\n", result);
    }

    return 1;
}

static int handle_del_request(const kvm_client_handle_t h_client, const char * key, const char * value)
{
    if (NULL == key || NULL != value)
    {
        printf("invalid input. Please try again\n");
        return 1;
    }

    kvm_const_dlob_data_t key_blob;

    key_blob.size = (uint32_t) strlen(key);
    key_blob.data = (const uint8_t *) key;
    const kvm_result_t result = kvm_client_delete(h_client, &key_blob);
    if (KVM_RESULT_OK != result)
    {
        printf("handle_del_request failed: error %d\n", result);
    }
    else
    {
        printf("%s key deleted\n", key);
    }

    return 1;
}

static int handle_list_keys_request(const kvm_client_handle_t h_client, const char * key, const char * value)
{
    if (NULL != key || NULL != value)
    {
        printf("invalid input. Please try again\n");
        return 1;
    }

    const kvm_result_t result = kvm_client_list_keys(h_client, callback, NULL);
    if (KVM_RESULT_OK != result)
    {
        printf("kvm_client_count failed: error %d\n", result);
    }

    return 1;
}

static int handle_count_request(const kvm_client_handle_t h_client, const char * key, const char * value)
{
    if (NULL != key || NULL != value)
    {
        printf("invalid input. Please try again\n");
        return 1;
    }

    uint32_t count = 0;
    const kvm_result_t result = kvm_client_count(h_client, &count);
    if (KVM_RESULT_OK != result)
    {
        printf("kvm_client_count failed: error %d\n", result);
    }
    else
    {
        printf("Count = %u\n", count);
    }

    return 1;
}

static int handle_quit_request(const kvm_client_handle_t h_client, const char * key, const char * value)
{
    if (NULL != key || NULL != value)
    {
        printf("invalid input. Please try again\n");
        return 1;
    }
    return 0;
}