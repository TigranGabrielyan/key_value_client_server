#include <string.h>
#include <stdlib.h>

#include "kvm_requests.h"
#include "kvm_replies.h"
#include "kvm_client.h"
#include "kvm_client_transport.h"

const uint32_t * dummy_ctx = ((uint32_t *) 0xABABABAB);
const char * ip = "127.0.0.1";
const uint16_t port = 55555;

const uint8_t get_reply_ok[] = {KVM_REPLY_STATUS_OK, 6, 0, 0, 0, 'v', 'a', 'l', 'u', 'e', '1'};
const uint8_t list_reply_ok[] = {KVM_REPLY_STATUS_OK, 1, 0, 0, 0, 4, 0, 0, 0, 'k', 'e', 'y', '1'};
const uint8_t count_reply_ok[] = {KVM_REPLY_STATUS_OK, 1, 0, 0, 0};

uint8_t delete_called;

kvm_result_t
kvm_transport_open(
    kvm_transport_handle_t *    h_transport,
    const char *                server_ip,
    uint16_t                    server_port)
{
    if (NULL == h_transport || NULL == server_ip)
    {
        return KVM_RESULT_INVALID_PARAM;
    }

    if (port != server_port)
    {
        return KVM_RESULT_INVALID_PARAM;
    }

    if (0 != strcmp(ip, server_ip))
    {
        return KVM_RESULT_INVALID_PARAM;
    }

    *h_transport = (kvm_transport_handle_t) dummy_ctx;
    delete_called = 0;

    return KVM_RESULT_OK;
}

kvm_result_t
kvm_transport_close(
    kvm_transport_handle_t h_transport)
{
    return KVM_RESULT_OK;
}

kvm_result_t
kvm_transport_send(
    kvm_transport_handle_t  h_transport,
    uint32_t                request_size,
    const uint8_t *         request,
    uint32_t *              reply_size,
    uint8_t **              reply)
{
    kvm_request_id_t id = (kvm_request_id_t) *request;
    uint8_t * r_buf = (uint8_t *) malloc(100);
    if (NULL == r_buf)
    {
        return KVM_RESULT_SYS_CALL_FAIL;
    }

    switch(id)
    {
        case KVM_REQUST_DELETE:
            delete_called = 1;
        case KVM_REQUST_PUT:
        {
            *reply_size = sizeof(kvm_reply_generic_t);
            ((kvm_reply_generic_t *) r_buf)->status = KVM_REPLY_STATUS_OK;
            break;
        }      
        case KVM_REQUST_GET:
        {
            if (0 == delete_called)
            {
                *reply_size = sizeof(get_reply_ok);
                mempcpy(r_buf, get_reply_ok, sizeof(get_reply_ok));
            }
            else
            {
                *reply_size = sizeof(kvm_reply_generic_t);
                ((kvm_reply_generic_t *) r_buf)->status = KVM_REPLY_STATUS_OK;
            }
            break;
        }        
        case KVM_REQUST_LIST:
        {
            *reply_size = sizeof(list_reply_ok);
            mempcpy(r_buf, list_reply_ok, sizeof(list_reply_ok));
            break;
        }
        case KVM_REQUST_COUNT:
        {
            *reply_size = sizeof(count_reply_ok);
            mempcpy(r_buf, count_reply_ok, sizeof(count_reply_ok));
            break;
        }
        default:
        {
            *reply_size = sizeof(kvm_reply_generic_t);
            ((kvm_reply_generic_t *) r_buf)->status = KVM_REPLY_BAD_REQUEST;
            break;
        }
    }

    *reply = r_buf;

    return KVM_RESULT_OK;
}