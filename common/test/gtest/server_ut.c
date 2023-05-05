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
