#include <gtest/gtest.h>
#include "kvm_results.h"
#include "kvm_requests.h"
#include "kvm_replies.h"
#include "kvm_server_internal.h"

/* PUT key1=value1 */
const uint8_t put_key1_value1_request[] = {KVM_REQUST_PUT, 4, 0, 0, 0, 6, 0, 0, 0, 'k', 'e', 'y', '1', 'v', 'a', 'l', 'u', 'e', '1'};

/* PUT key2=value2 */
const uint8_t put_key2_value2_request[] = {KVM_REQUST_PUT, 4, 0, 0, 0, 6, 0, 0, 0, 'k', 'e', 'y', '2', 'v', 'a', 'l', 'u', 'e', '2'};

/* GET key1 */
const uint8_t get_key1_request[] = {KVM_REQUST_GET, 4, 0, 0, 0, 'k', 'e', 'y', '1'};

/* GET key2 */
const uint8_t get_key2_request[] = {KVM_REQUST_GET, 4, 0, 0, 0, 'k', 'e', 'y', '2'};

/* DELETE key1 */
const uint8_t delete_key1_request[] = {KVM_REQUST_DELETE, 4, 0, 0, 0, 'k', 'e', 'y', '1'};

/* DELETE key2 */
const uint8_t delete_key2_request[] = {KVM_REQUST_DELETE, 4, 0, 0, 0, 'k', 'e', 'y', '2'};

/* LIST */
const uint8_t list_request[] = {KVM_REQUST_LIST};

/* COUNT */
const uint8_t count_request[] = {KVM_REQUST_COUNT};


const uint8_t generic_reply_ok[] = {KVM_REPLY_STATUS_OK};
const uint8_t generic_reply_bad_request[] = {KVM_REPLY_BAD_REQUEST};
const uint8_t get_key1_reply_ok[] = {KVM_REPLY_STATUS_OK, 6, 0, 0, 0, 'v', 'a', 'l', 'u', 'e', '1'};
const uint8_t list_reply_ok[] = {KVM_REPLY_STATUS_OK, 2, 0, 0, 0, 4, 0, 0, 0, 'k', 'e', 'y', '1', 4, 0, 0, 0, 'k', 'e', 'y', '2'};
const uint8_t list_count_empty_reply_ok[] = {KVM_REPLY_STATUS_OK, 0, 0, 0, 0};
const uint8_t count_reply_ok[] = {KVM_REPLY_STATUS_OK, 2, 0, 0, 0};

class server_handle_request : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        ASSERT_EQ(KVM_RESULT_OK, init_apr_hashtable());
        reply = nullptr;
        reply_size = 0;
    }

    virtual void TearDown()
    {
        reset_reply();
        uninit_apr_hashtable();
    }

    void reset_reply()
    {
        if (nullptr != reply)
        {
            memset(reply, 0, reply_size);
            free(reply);
            reply = nullptr;
        }
        reply_size = 0;
    }

    uint8_t *   reply;
    uint32_t    reply_size;
};

/********** PUT **********/
TEST_F(server_handle_request, handle_request_put_return_ok)
{
    EXPECT_EQ(KVM_RESULT_OK, handle_request(sizeof(put_key1_value1_request), put_key1_value1_request, &reply_size, &reply));
    EXPECT_EQ(sizeof(generic_reply_ok), reply_size);
    EXPECT_EQ(0, memcmp(generic_reply_ok, reply, reply_size));
}

TEST_F(server_handle_request, handle_request_put_invalid_request_size_return_bad_request)
{
    EXPECT_EQ(KVM_RESULT_OK, handle_request(sizeof(put_key1_value1_request) - 1, put_key1_value1_request, &reply_size, &reply));
    EXPECT_EQ(sizeof(generic_reply_bad_request), reply_size);
    EXPECT_EQ(0, memcmp(generic_reply_bad_request, reply, reply_size));
}

/********** GET **********/
TEST_F(server_handle_request, handle_request_get_return_ok)
{
    EXPECT_EQ(KVM_RESULT_OK, handle_request(sizeof(put_key1_value1_request), put_key1_value1_request, &reply_size, &reply));
    EXPECT_EQ(sizeof(generic_reply_ok), reply_size);
    EXPECT_EQ(0, memcmp(generic_reply_ok, reply, reply_size));

    reset_reply();

    EXPECT_EQ(KVM_RESULT_OK, handle_request(sizeof(get_key1_request), get_key1_request, &reply_size, &reply));
    EXPECT_EQ(sizeof(get_key1_reply_ok), reply_size);
    EXPECT_EQ(0, memcmp(get_key1_reply_ok, reply, reply_size));
}

TEST_F(server_handle_request, handle_request_get_invalid_request_size_return_bad_request)
{
    EXPECT_EQ(KVM_RESULT_OK, handle_request(sizeof(get_key1_request) - 1, get_key1_request, &reply_size, &reply));
    EXPECT_EQ(sizeof(generic_reply_bad_request), reply_size);
    EXPECT_EQ(0, memcmp(generic_reply_bad_request, reply, reply_size));
}

TEST_F(server_handle_request, handle_request_get_empty_hash_table_return_bad_request)
{
    EXPECT_EQ(KVM_RESULT_OK, handle_request(sizeof(get_key1_request), get_key1_request, &reply_size, &reply));
    EXPECT_EQ(sizeof(generic_reply_bad_request), reply_size);
    EXPECT_EQ(0, memcmp(generic_reply_bad_request, reply, reply_size));
}

TEST_F(server_handle_request, handle_request_get_missing_key_return_bad_request)
{
    EXPECT_EQ(KVM_RESULT_OK, handle_request(sizeof(put_key1_value1_request), put_key1_value1_request, &reply_size, &reply));
    EXPECT_EQ(sizeof(generic_reply_ok), reply_size);
    EXPECT_EQ(0, memcmp(generic_reply_ok, reply, reply_size));

    reset_reply();

    EXPECT_EQ(KVM_RESULT_OK, handle_request(sizeof(get_key2_request), get_key2_request, &reply_size, &reply));
    EXPECT_EQ(sizeof(generic_reply_bad_request), reply_size);
    EXPECT_EQ(0, memcmp(generic_reply_bad_request, reply, reply_size));
}

/********** DELETE **********/
TEST_F(server_handle_request, handle_request_delete_return_ok)
{
    EXPECT_EQ(KVM_RESULT_OK, handle_request(sizeof(put_key1_value1_request), put_key1_value1_request, &reply_size, &reply));
    EXPECT_EQ(sizeof(generic_reply_ok), reply_size);
    EXPECT_EQ(0, memcmp(generic_reply_ok, reply, reply_size));

    reset_reply();

    EXPECT_EQ(KVM_RESULT_OK, handle_request(sizeof(delete_key1_request), delete_key1_request, &reply_size, &reply));
    EXPECT_EQ(sizeof(generic_reply_ok), reply_size);
    EXPECT_EQ(0, memcmp(generic_reply_ok, reply, reply_size));
    
    reset_reply();

    EXPECT_EQ(KVM_RESULT_OK, handle_request(sizeof(get_key1_request), get_key1_request, &reply_size, &reply));
    EXPECT_EQ(sizeof(generic_reply_bad_request), reply_size);
    EXPECT_EQ(0, memcmp(generic_reply_bad_request, reply, reply_size));
}

TEST_F(server_handle_request, handle_request_delete_invalid_request_size_return_bad_request)
{
    EXPECT_EQ(KVM_RESULT_OK, handle_request(sizeof(delete_key1_request) - 1, delete_key1_request, &reply_size, &reply));
    EXPECT_EQ(sizeof(generic_reply_bad_request), reply_size);
    EXPECT_EQ(0, memcmp(generic_reply_bad_request, reply, reply_size));
}

TEST_F(server_handle_request, handle_request_delete_empty_hash_table_return_ok)
{
    EXPECT_EQ(KVM_RESULT_OK, handle_request(sizeof(delete_key1_request), delete_key1_request, &reply_size, &reply));
    EXPECT_EQ(sizeof(generic_reply_ok), reply_size);
    EXPECT_EQ(0, memcmp(generic_reply_ok, reply, reply_size));
}

TEST_F(server_handle_request, handle_request_delete_missing_key_return_ok)
{
    EXPECT_EQ(KVM_RESULT_OK, handle_request(sizeof(put_key1_value1_request), put_key1_value1_request, &reply_size, &reply));
    EXPECT_EQ(sizeof(generic_reply_ok), reply_size);
    EXPECT_EQ(0, memcmp(generic_reply_ok, reply, reply_size));

    reset_reply();

    EXPECT_EQ(KVM_RESULT_OK, handle_request(sizeof(delete_key2_request), delete_key2_request, &reply_size, &reply));
    EXPECT_EQ(sizeof(generic_reply_ok), reply_size);
    EXPECT_EQ(0, memcmp(generic_reply_ok, reply, reply_size));
}

/********** LIST **********/
TEST_F(server_handle_request, handle_request_list_return_ok)
{
    EXPECT_EQ(KVM_RESULT_OK, handle_request(sizeof(put_key1_value1_request), put_key1_value1_request, &reply_size, &reply));
    EXPECT_EQ(sizeof(generic_reply_ok), reply_size);
    EXPECT_EQ(0, memcmp(generic_reply_ok, reply, reply_size));

    reset_reply();

    EXPECT_EQ(KVM_RESULT_OK, handle_request(sizeof(put_key2_value2_request), put_key2_value2_request, &reply_size, &reply));
    EXPECT_EQ(sizeof(generic_reply_ok), reply_size);
    EXPECT_EQ(0, memcmp(generic_reply_ok, reply, reply_size));

    reset_reply();

    EXPECT_EQ(KVM_RESULT_OK, handle_request(sizeof(list_request), list_request, &reply_size, &reply));
    EXPECT_EQ(sizeof(list_reply_ok), reply_size);
    EXPECT_EQ(0, memcmp(list_reply_ok, reply, reply_size));
}

TEST_F(server_handle_request, handle_request_list_invalid_request_size_return_bad_request)
{
    EXPECT_EQ(KVM_RESULT_OK, handle_request(sizeof(list_request) - 1, list_request, &reply_size, &reply));
    EXPECT_EQ(sizeof(generic_reply_bad_request), reply_size);
    EXPECT_EQ(0, memcmp(generic_reply_bad_request, reply, reply_size));
}

TEST_F(server_handle_request, handle_request_list_empty_hash_table_return_ok)
{
    EXPECT_EQ(KVM_RESULT_OK, handle_request(sizeof(list_request), list_request, &reply_size, &reply));
    EXPECT_EQ(sizeof(list_count_empty_reply_ok), reply_size);
    EXPECT_EQ(0, memcmp(list_count_empty_reply_ok, reply, reply_size));
}

/********** COUNT **********/
TEST_F(server_handle_request, handle_request_count_return_ok)
{
    EXPECT_EQ(KVM_RESULT_OK, handle_request(sizeof(put_key1_value1_request), put_key1_value1_request, &reply_size, &reply));
    EXPECT_EQ(sizeof(generic_reply_ok), reply_size);
    EXPECT_EQ(0, memcmp(generic_reply_ok, reply, reply_size));

    reset_reply();

    EXPECT_EQ(KVM_RESULT_OK, handle_request(sizeof(put_key2_value2_request), put_key2_value2_request, &reply_size, &reply));
    EXPECT_EQ(sizeof(generic_reply_ok), reply_size);
    EXPECT_EQ(0, memcmp(generic_reply_ok, reply, reply_size));

    reset_reply();

    EXPECT_EQ(KVM_RESULT_OK, handle_request(sizeof(count_request), count_request, &reply_size, &reply));
    EXPECT_EQ(sizeof(count_reply_ok), reply_size);
    EXPECT_EQ(0, memcmp(count_reply_ok, reply, reply_size));
}

TEST_F(server_handle_request, handle_request_count_invalid_request_size_return_bad_request)
{
    EXPECT_EQ(KVM_RESULT_OK, handle_request(sizeof(count_request) - 1, count_request, &reply_size, &reply));
    EXPECT_EQ(sizeof(generic_reply_bad_request), reply_size);
    EXPECT_EQ(0, memcmp(generic_reply_bad_request, reply, reply_size));
}

TEST_F(server_handle_request, handle_request_count_empty_hash_table_return_ok)
{
    EXPECT_EQ(KVM_RESULT_OK, handle_request(sizeof(count_request), count_request, &reply_size, &reply));
    EXPECT_EQ(sizeof(list_count_empty_reply_ok), reply_size);
    EXPECT_EQ(0, memcmp(list_count_empty_reply_ok, reply, reply_size));
}