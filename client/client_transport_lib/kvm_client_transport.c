/**
* @file kvm_client_transport.c
*
* @brief The module contains client transport library implementation.
*
*/

#include "kvm_client_transport.h"
#include "kvm_client_transport_internal.h"

kvm_result_t
kvm_transport_open(
    kvm_transport_handle_t *    h_transport,
    const char * const          server_ip,
    uint32_t                    server_port)
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
    const uint8_t * const   request,
    uint32_t *              responce_size,
    const uint8_t **        responce)
{
    return KVM_RESULT_OK;
}