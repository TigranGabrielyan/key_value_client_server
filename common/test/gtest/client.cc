#include <gtest/gtest.h>
#include "kvm_client.h"

const uint8_t key1[] = {'k', 'e', 'y', '1'};
const uint8_t value1[] = {'v', 'a', 'l', 'u', 'e', '1'};

kvm_const_dlob_data_t key1_blob = {sizeof(key1), key1};
kvm_const_dlob_data_t value1_blob = {sizeof(value1), value1};

class client_request : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        h_client = nullptr;
    }

    virtual void TearDown()
    {
    }

    kvm_client_handle_t h_client;
    const char * ip = "127.0.0.1";
    const uint16_t port = 55555;
};

/********** kvm_client_open **********/
TEST_F(client_request, client_open_return_ok)
{
    ASSERT_EQ(KVM_RESULT_OK, kvm_client_open(&h_client, ip, port));
    ASSERT_NE(nullptr, h_client);
    EXPECT_EQ(KVM_RESULT_OK, kvm_client_close(h_client));
}

TEST_F(client_request, client_open_null_handle_pointer_return_bad_param)
{
    ASSERT_EQ(KVM_RESULT_INVALID_PARAM, kvm_client_open(NULL, ip, port));
}

TEST_F(client_request, client_open_null_ip_return_bad_param)
{
    ASSERT_EQ(KVM_RESULT_INVALID_PARAM, kvm_client_open(&h_client, NULL, port));
}

TEST_F(client_request, client_open_invalid_ip_return_bad_param)
{  
    ASSERT_EQ(KVM_RESULT_INVALID_PARAM, kvm_client_open(&h_client, "arbitrary string", port));
}

/********** kvm_client_put **********/
TEST_F(client_request, client_put_return_ok)
{
    ASSERT_EQ(KVM_RESULT_OK, kvm_client_open(&h_client, ip, port));
    ASSERT_NE(nullptr, h_client);

    EXPECT_EQ(KVM_RESULT_OK, kvm_client_put(h_client, &key1_blob, &value1_blob));

    EXPECT_EQ(KVM_RESULT_OK, kvm_client_close(h_client));
}

TEST_F(client_request, client_put_null_client_handle_return_bad_param)
{
    ASSERT_EQ(KVM_RESULT_INVALID_PARAM, kvm_client_put(NULL, &key1_blob, &value1_blob));
}

TEST_F(client_request, client_put_null_value_return_bad_param)
{
    ASSERT_EQ(KVM_RESULT_INVALID_PARAM, kvm_client_put(h_client, &key1_blob, NULL));
}

/********** kvm_client_get **********/
void get_callback(void * context, const kvm_const_dlob_data_t * data)
{
    if (data->size == value1_blob.size)
    {
        if (0 == memcmp(data->data, value1_blob.data, data->size))
        {
            *((uint8_t *) context) = 1;
        }
    }
}
TEST_F(client_request, client_get_return_ok)
{
    ASSERT_EQ(KVM_RESULT_OK, kvm_client_open(&h_client, ip, port));
    ASSERT_NE(nullptr, h_client);

    EXPECT_EQ(KVM_RESULT_OK, kvm_client_put(h_client, &key1_blob, &value1_blob));

    uint8_t cb_result = 0;
    EXPECT_EQ(KVM_RESULT_OK, kvm_client_get(h_client, &key1_blob, get_callback, &cb_result));
    EXPECT_EQ(1, cb_result);

    EXPECT_EQ(KVM_RESULT_OK, kvm_client_close(h_client));
}


TEST_F(client_request, client_get_null_client_handle_return_bad_param)
{
    ASSERT_EQ(KVM_RESULT_INVALID_PARAM, kvm_client_get(NULL, &key1_blob, get_callback, NULL));
}

TEST_F(client_request, client_get_null_key_return_bad_param)
{
    ASSERT_EQ(KVM_RESULT_INVALID_PARAM, kvm_client_get(h_client, NULL, get_callback, NULL));
}

TEST_F(client_request, client_get_null_callback_return_bad_param)
{
    ASSERT_EQ(KVM_RESULT_INVALID_PARAM, kvm_client_get(h_client, &key1_blob, NULL, NULL));
}

/********** kvm_client_delete **********/
TEST_F(client_request, client_delete_return_ok)
{
    ASSERT_EQ(KVM_RESULT_OK, kvm_client_open(&h_client, ip, port));
    ASSERT_NE(nullptr, h_client);

    EXPECT_EQ(KVM_RESULT_OK, kvm_client_put(h_client, &key1_blob, &value1_blob));

    EXPECT_EQ(KVM_RESULT_OK, kvm_client_delete(h_client, &key1_blob));

    uint8_t cb_result = 0;
    EXPECT_EQ(KVM_RESULT_OK, kvm_client_get(h_client, &key1_blob, get_callback, &cb_result));
    EXPECT_EQ(0, cb_result);

    EXPECT_EQ(KVM_RESULT_OK, kvm_client_close(h_client));
}

TEST_F(client_request, client_delete_null_client_handle_return_bad_param)
{
    ASSERT_EQ(KVM_RESULT_INVALID_PARAM, kvm_client_delete(NULL, &key1_blob));
}

TEST_F(client_request, client_delete_null_key_return_bad_param)
{
    ASSERT_EQ(KVM_RESULT_INVALID_PARAM, kvm_client_delete(h_client, NULL));
}

/********** kvm_client_list_keys **********/
void list_callback(void * context, const kvm_const_dlob_data_t * data)
{
    if (NULL == data && *((uint8_t *) context) != 1)
    {
        *((uint8_t *) context) = 0;
    }

    if (data->size == value1_blob.size)
    {
        if (0 == memcmp(data->data, value1_blob.data, data->size))
        {
            *((uint8_t *) context) = 1;
        }
    }
}

TEST_F(client_request, client_list_keys_return_ok)
{
    ASSERT_EQ(KVM_RESULT_OK, kvm_client_open(&h_client, ip, port));
    ASSERT_NE(nullptr, h_client);

    EXPECT_EQ(KVM_RESULT_OK, kvm_client_put(h_client, &key1_blob, &value1_blob));

    uint8_t cb_result = 0;
    EXPECT_EQ(KVM_RESULT_OK, kvm_client_list_keys(h_client, list_callback, &cb_result));
    EXPECT_EQ(1, cb_result);

    EXPECT_EQ(KVM_RESULT_OK, kvm_client_close(h_client));
}

TEST_F(client_request, client_list_keys_null_client_handle_return_bad_param)
{
    ASSERT_EQ(KVM_RESULT_INVALID_PARAM, kvm_client_list_keys(NULL, list_callback, NULL));
}

TEST_F(client_request, client_list_keys_null_callback_return_bad_param)
{
    ASSERT_EQ(KVM_RESULT_INVALID_PARAM, kvm_client_list_keys(h_client, NULL, NULL));
}

/********** kvm_client_count **********/
TEST_F(client_request, client_count_return_ok)
{
    ASSERT_EQ(KVM_RESULT_OK, kvm_client_open(&h_client, ip, port));
    ASSERT_NE(nullptr, h_client);

    EXPECT_EQ(KVM_RESULT_OK, kvm_client_put(h_client, &key1_blob, &value1_blob));

    uint32_t count = 0;
    EXPECT_EQ(KVM_RESULT_OK, kvm_client_count(h_client, &count));
    EXPECT_EQ(1, count);

    EXPECT_EQ(KVM_RESULT_OK, kvm_client_close(h_client));
}

TEST_F(client_request, client_count_null_client_handle_return_bad_param)
{
    uint32_t count = 0;
    ASSERT_EQ(KVM_RESULT_INVALID_PARAM, kvm_client_count(NULL, &count));
}

TEST_F(client_request, client_count_null_count_return_bad_param)
{
    ASSERT_EQ(KVM_RESULT_INVALID_PARAM, kvm_client_count(h_client, NULL));
}