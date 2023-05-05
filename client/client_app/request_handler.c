#include <stdio.h>
#include <stdlib.h>
#include "request_handler.h"

#include "apr_general.h"
#include "apr_pools.h"
#include "apr_hash.h"

apr_hash_t * ht = NULL;

static void init_apr_hashtable(void);
static void uninit_apr_hashtable(void);

const char* requests[] =
{
    "put",
    "get",
    "del",
    "list-keys",
    "count",
    "quit",
};

static int parse_request(char * input_line, char ** request, char ** key, char ** value);
static void callback(void * context, kvm_const_dlob_data_t * data);

static int handle_put_request(kvm_client_handle_t h_client, const char * key, const char * value);
static int handle_get_request(kvm_client_handle_t h_client, const char * key, const char * value);
static int handle_del_request(kvm_client_handle_t h_client, const char * key, const char * value);
static int handle_list_keys_request(kvm_client_handle_t h_client, const char * key, const char * value);
static int handle_count_request(kvm_client_handle_t h_client, const char * key, const char * value);
static int handle_quit_request(kvm_client_handle_t h_client, const char * key, const char * value);

int init_request_handler()
{
    if (NULL == ht)
    {
        return 1;
    }

    init_apr_hashtable();
    if (NULL == ht)
    {
        printf("Hash Table initialization failed\n");
        return 0;
    }

    apr_hash_set(ht, requests[0], APR_HASH_KEY_STRING, handle_put_request);
    apr_hash_set(ht, requests[1], APR_HASH_KEY_STRING, handle_get_request);
    apr_hash_set(ht, requests[2], APR_HASH_KEY_STRING, handle_del_request);
    apr_hash_set(ht, requests[3], APR_HASH_KEY_STRING, handle_list_keys_request);
    apr_hash_set(ht, requests[4], APR_HASH_KEY_STRING, handle_count_request);
}

int uninit_request_handler()
{
    uninit_apr_hashtable();
}

int handle_request(kvm_client_handle_t h_client, char * input_line)
{
    char * request = NULL;
    char * key = NULL;
    char * value = NULL;

    if (!parse_request(input_line, &request, &key, &value))
    {
        printf("invalid input. Please try again\n");
        return 1;
    }

     int (* handler) (kvm_client_handle_t, const char *, const char *) = apr_hash_get(ht, key, APR_HASH_KEY_STRING);
     if (NULL == handler)
     {
        printf("invalid input. Please try again\n");
        return 1;
     }

     return handler(h_client, key, value);
}

static void init_apr_hashtable(void)
{
    apr_status_t status = APR_SUCCESS;
    apr_pool_t * pool = NULL;

    status = apr_initialize();
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
        apr_pool_t * pool = NULL;
        apr_hash_index_t * hi = NULL;
        void * key = NULL;
        void * val = NULL;

        for (hi = apr_hash_first(NULL, ht); hi; hi = apr_hash_next(hi))
        {
            apr_hash_this(hi, (const void **) &key, NULL, &val);
            free(key);
            free(val);
        }

        pool = apr_hash_pool_get(ht);
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
    char * ptr = strchr(input_line, ' ');
    if (NULL == ptr)
    {
        return 0;
    }

    *request = input_line;
    *ptr = '\0';
    input_line = ptr + 1;

    ptr = strchr(input_line, '=');
    if (NULL == ptr)
    {
        // Possible request with no extra arguments. Will be further verified.
        *key = NULL;
        *value = NULL;
        return 1;
    }

    *key = input_line;
    *ptr = '\0';
    *value = ptr + 1;

    return 1;
}

static void callback(void * context, kvm_const_dlob_data_t * data)
{
    char * str = malloc(data->size + 1);
    memcpy(str, data->data, data->size);
    str[data->size] = '\0';
    printf("%s\n", str);
    free(str);
}

static int handle_put_request(kvm_client_handle_t h_client, const char * key, const char * value)
{
    if (NULL == key || NULL == value)
    {
        printf("invalid input. Please try again\n");
        return 1;
    }

    kvm_result_t result = KVM_RESULT_INVALID;
    kvm_const_dlob_data_t key_blob;
    kvm_const_dlob_data_t value_blob;

    key_blob.size = strlen(key);
    key_blob.data = key;

    value_blob.size = strlen(key);
    value_blob.data = key;

    result = kvm_client_put(h_client, &key_blob, &value_blob);
    if (KVM_RESULT_OK != result)
    {
        printf("kvm_client_get failed: error %d", result);
    }
    else
    {
        printf("Key/Value pair successfully stored\n");
    }

    return 1;
}

static int handle_get_request(kvm_client_handle_t h_client, const char * key, const char * value)
{
    if (NULL == key || NULL != value)
    {
        printf("invalid input. Please try again\n");
        return 1;
    }

    kvm_result_t result = KVM_RESULT_INVALID;
    kvm_const_dlob_data_t key_blob;

    key_blob.size = strlen(key);
    key_blob.data = key;
    result = kvm_client_get(h_client, &key_blob,  callback, NULL);
    if (KVM_RESULT_OK != result)
    {
        printf("handle_get_request failed: error %d", result);
    }

    return 1;
}

static int handle_del_request(kvm_client_handle_t h_client, const char * key, const char * value)
{
    if (NULL == key || NULL != value)
    {
        printf("invalid input. Please try again\n");
        return 1;
    }

    kvm_result_t result = KVM_RESULT_INVALID;
    kvm_const_dlob_data_t key_blob;

    key_blob.size = strlen(key);
    key_blob.data = key;
    result = kvm_client_delete(h_client, &key_blob);
    if (KVM_RESULT_OK != result)
    {
        printf("handle_del_request failed: error %d", result);
    }
    else
    {
        printf("%s key deleted", key);
    }

    return 1;
}

static int handle_list_keys_request(kvm_client_handle_t h_client, const char * key, const char * value)
{
    if (NULL != key || NULL != value)
    {
        printf("invalid input. Please try again\n");
        return 1;
    }

    kvm_result_t result = KVM_RESULT_INVALID;
    uint32_t coutn = 0;
    result = kvm_client_list(h_client, callback, NULL);
    if (KVM_RESULT_OK != result)
    {
        printf("kvm_client_count failed: error %d", result);
    }

    return 1;
}

static int handle_count_request(kvm_client_handle_t h_client, const char * key, const char * value)
{
    if (NULL != key || NULL != value)
    {
        printf("invalid input. Please try again\n");
        return 1;
    }

    kvm_result_t result = KVM_RESULT_INVALID;
    uint32_t coutn = 0;
    result = kvm_client_count(h_client, &coutn);
    if (KVM_RESULT_OK != result)
    {
        printf("kvm_client_count failed: error %d", result);
    }
    else
    {
        printf("Count = %ul", coutn);
    }

    return 1;
}

static int handle_quit_request(kvm_client_handle_t h_client, const char * key, const char * value)
{
    if (NULL != key || NULL != value)
    {
        printf("invalid input. Please try again\n");
        return 1;
    }
    return 0;
}