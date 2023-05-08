#include <string.h>
#include <stdlib.h>
#include "kvm_client_transport.h"

kvm_result_t
kvm_transport_open(
    kvm_transport_handle_t *    h_transport,
    const char *                server_ip,
    uint16_t                    server_port)
{
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
    return KVM_RESULT_OK;
}