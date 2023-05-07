/**
 * @file kvm_client_transport.h
 *
 * @brief Defines client trnasport interfaces.
 *
 */

#ifndef __kvm_client_transport_h__
#define __kvm_client_transport_h__

#include "kvm_results.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

typedef struct kvm_transprot_s* kvm_transport_handle_t;


/**< Response callback type */
typedef void (* kvm_response_callback_t)(
    void *          context,
    uint32_t        response_size,
    const uint8_t * response);

/*!
*******************************************************************************
** Opens the client to work with Key/Value management system.
**
** @param[out]  h_transport Pinter where opened transport handle will be stored.
** @param[in]   server_ip   Zero terminated IP address of the server.
** @param[in]   server_port Server port.
**
** @return
**      - KVM_RESULT_OK or corresponding KVM_RESULT_XXX in case of failure.
*/
kvm_result_t
kvm_transport_open(
    kvm_transport_handle_t *    h_transport,
    const char *                server_ip,
    uint16_t                    server_port);

/*!
*******************************************************************************
** Closes the transport opened by kvm_transport_open().
**
** @param[in]  h_transport    Client handle.
**
** @return
**      - KVM_RESULT_OK or corresponding KVM_RESULT_XXX in case of failure.
*/
kvm_result_t
kvm_transport_close(
    kvm_transport_handle_t h_transport);

/*!
*******************************************************************************
** Closes the transport opened by kvm_transport_open().
**
**
** @param[in]   h_transport     Client handle.
** @param[in]   request_size    Size of the request buffer.
** @param[in]   request         Request buffer.
** @param[out]  response_size   Pointer where response size will be stored.
** @param[out]  response        Pointer where response will be stored. It is
**                              up to caller to free the memory.
**
** @return
**      - KVM_RESULT_OK or corresponding KVM_RESULT_XXX in case of failure.
*/
kvm_result_t
kvm_transport_send(
    kvm_transport_handle_t  h_transport,
    uint32_t                request_size,
    const uint8_t *         request,
    uint32_t *              response_size,
    uint8_t **              response);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __kvm_client_transport_h__ */