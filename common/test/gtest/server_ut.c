#include <CUnit/Basic.h>
#include <stdio.h>
#include <string.h>

#include "server.h"

void test_handle_client_request_set(void)
{
    int sd = 1;
    char buffer[1024] = "SET key value";
    char response[1024];

    handle_client_request(sd, buffer, response);

    CU_ASSERT_STRING_EQUAL(response, "OK");

    // Check if the value was set correctly
    char* value = get_value("key");
    CU_ASSERT_STRING_EQUAL(value, "value");
}

void test_handle_client_request_get(void)
{
    int sd = 1;
    char buffer[1024] = "GET key";
    char response[1024];

    handle_client_request(sd, buffer, response);

    CU_ASSERT_STRING_EQUAL(response, "value");
}

void test_handle_client_request_get_nonexistent_key(void)
{
    int sd = 1;
    char buffer[1024] = "GET nonexistent_key";
    char response[1024];

    handle_client_request(sd, buffer, response);

    CU_ASSERT_STRING_EQUAL(response, "ERROR");
}

int main()
{
    // Initialize CUnit test registry
    if (CUE_SUCCESS != CU_initialize_registry())
    {
        return CU_get_error();
    }

    // Add the tests to the suite
    CU_pSuite suite = CU_add_suite("handle_client_request_suite", NULL, NULL);
    CU_add_test(suite, "test_handle_client_request_set", test_handle_client_request_set);
    CU_add_test(suite, "test_handle_client_request_get", test_handle_client_request_get);
    CU_add_test(suite, "test_handle_client_request_get_nonexistent_key", test_handle_client_request_get_nonexistent_key);

    // Run the tests
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();

    // Cleanup
    CU_cleanup_registry();

    return CU_get_error();
}

#if 0
void test(void)
{
    uint8_t request[100];
    uint32_t request_size = 0;

    uint8_t * reply;
    uint32_t reply_size = 0;

    uint8_t * ptr;
    uint32_t tmp;
    kvm_result_t result;
    kvm_reply_status_t status;

    const char * key1 = "key1";
    uint32_t key1_size = (uint32_t) strlen(key1);
    const char * key2 = "key2";
    uint32_t key2_size = (uint32_t) strlen(key2);

    const char * value1 = "value1";
    uint32_t value1_size = (uint32_t) strlen(value1);
    const char * value2 = "value2";
    uint32_t value2_size = (uint32_t) strlen(value2);

    /* Add key 1 */
    ptr = request;
    ((kvm_request_generic_t *) ptr)->id = KVM_REQUST_PUT;
    ptr += sizeof (kvm_request_generic_t);
    ((kvm_request_put_t *) ptr)->key_size = key1_size;
    ((kvm_request_put_t *) ptr)->value_size = value1_size;
    ptr += sizeof(kvm_request_put_t);
    memcpy(ptr, key1, key1_size);
    memcpy(ptr + key1_size, value1, value1_size);

    request_size = sizeof(kvm_request_generic_t) + sizeof(kvm_request_put_t) + key1_size + value1_size;
    result = handle_request(request_size, request, &reply_size, &reply);

    status = ((kvm_reply_generic_t *) reply)->status;

    /* Add key 2 */
    ptr = request;
    ((kvm_request_generic_t *) ptr)->id = KVM_REQUST_PUT;
    ptr += sizeof (kvm_request_generic_t);
    ((kvm_request_put_t *) ptr)->key_size = key2_size;
    ((kvm_request_put_t *) ptr)->value_size = value2_size;
    ptr += sizeof(kvm_request_put_t);
    memcpy(ptr, key2, key2_size);
    memcpy(ptr + key2_size, value2, value2_size);

    request_size = sizeof(kvm_request_generic_t) + sizeof(kvm_request_put_t) + key2_size + value2_size;
    result = handle_request(request_size, request, &reply_size, &reply);

    status = ((kvm_reply_generic_t *) reply)->status;
    free(reply);

    /* Get key 1 */
    ptr = request;
    ((kvm_request_generic_t *) ptr)->id = KVM_REQUST_GET;
    ptr += sizeof (kvm_request_generic_t);
    ((kvm_request_get_t *) ptr)->key_size = key1_size;
    ptr += sizeof(kvm_request_get_t);
    memcpy(ptr, key1, key1_size);

    request_size = sizeof(kvm_request_generic_t) + sizeof(kvm_request_get_t) + key1_size;
    result = handle_request(request_size, request, &reply_size, &reply);

    status = ((kvm_reply_generic_t *) reply)->status;
    ptr = reply + sizeof(kvm_reply_generic_t);
    tmp = ((kvm_reply_get_t *) ptr)->value_size;
    free(reply);

    /* Get count */
    ptr = request;
    ((kvm_request_generic_t *) ptr)->id = KVM_REQUST_COUNT;

    request_size = sizeof(kvm_request_generic_t);
    result = handle_request(request_size, request, &reply_size, &reply);

    status = ((kvm_reply_generic_t *) reply)->status;
    ptr = reply + sizeof(kvm_reply_generic_t);
    tmp = ((kvm_reply_count_t *) ptr)->count;
    free(reply);

    /* Get List */
    ptr = request;
    ((kvm_request_generic_t *) ptr)->id = KVM_REQUST_LIST;

    request_size = sizeof(kvm_request_generic_t);
    result = handle_request(request_size, request, &reply_size, &reply);

    status = ((kvm_reply_generic_t *) reply)->status;
    ptr = reply + sizeof(kvm_reply_generic_t);
    tmp = ((kvm_reply_list_t *) ptr)->count;
    free(reply);

    /* Delete key1 */
    ptr = request;
    ((kvm_request_generic_t *) ptr)->id = KVM_REQUST_DELETE;
    ptr += sizeof (kvm_request_generic_t);
    ((kvm_request_delete_t *) ptr)->key_size = key1_size;
    ptr += sizeof(kvm_request_delete_t);
    memcpy(ptr, key1, key1_size);

    request_size = sizeof(kvm_request_generic_t) + sizeof(kvm_request_delete_t) + key1_size;
    result = handle_request(request_size, request, &reply_size, &reply);

    status = ((kvm_reply_generic_t *) reply)->status;
    free(reply);

    /* Get key 1 */
    ptr = request;
    ((kvm_request_generic_t *) ptr)->id = KVM_REQUST_GET;
    ptr += sizeof (kvm_request_generic_t);
    ((kvm_request_get_t *) ptr)->key_size = key1_size;
    ptr += sizeof(kvm_request_get_t);
    memcpy(ptr, key1, key1_size);

    request_size = sizeof(kvm_request_generic_t) + sizeof(kvm_request_get_t) + key1_size + value1_size;
    result = handle_request(request_size, request, &reply_size, &reply);

    status = ((kvm_reply_generic_t *) reply)->status;
    ptr = reply + sizeof(kvm_reply_generic_t);
    tmp = ((kvm_reply_get_t *) ptr)->value_size;
    free(reply);

    /* Get count */
    ptr = request;
    ((kvm_request_generic_t *) ptr)->id = KVM_REQUST_COUNT;

    request_size = sizeof(kvm_request_generic_t);
    result = handle_request(request_size, request, &reply_size, &reply);

    status = ((kvm_reply_generic_t *) reply)->status;
    ptr = reply + sizeof(kvm_reply_generic_t);
    tmp = ((kvm_reply_count_t *) ptr)->count;
    free(reply);
}
#endif