#include "kvm_requests.h"
#include "kvm_replies.h"

const char * ip = "127.0.0.1";
const uint16_t port = 55555;

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