/**
 * @file kvm_server.h
 *
 * @brief Defines Key/Value Management System server interfaces.
 *
 */

#ifndef __kvm_server_h__
#define __kvm_server_h__

#include "kvm_results.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/*!
*******************************************************************************
** Initializes Key/Value Management System server.
**
** @param[in]   port    Server port.
**
** @return
**      - KVM_RESULT_OK or corresponding KVM_RESULT_XXX in case of failure.
*/
kvm_result_t
kvm_server_init(
    uint16_t port);

/*!
*******************************************************************************
** Un-initializes Key/Value Management System server.
**
** @return
**      - KVM_RESULT_OK or corresponding KVM_RESULT_XXX in case of failure.
*/
kvm_result_t
kvm_server_uninit(
    void);

/*!
*******************************************************************************
** Waits for request from the client.
**
** @return
**      - KVM_RESULT_OK or corresponding KVM_RESULT_XXX in case of failure.
*/
kvm_result_t
kvm_server_wait_client_request(
    void);

/*!
*******************************************************************************
** Handles request from the client.
**
** @return
**      - KVM_RESULT_OK or corresponding KVM_RESULT_XXX in case of failure.
*/
kvm_result_t
kvm_server_handle_request(
    void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __kvm_server_h__ */