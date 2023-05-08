/**
 * @file kvm_client.h
 *
 * @brief Defines the interfaces used by Key/Value Management System clients.
 *
 */

#ifndef __kvm_client_h__
#define __kvm_client_h__

#include "kvm_results.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

typedef struct kvm_client_s * kvm_client_handle_t;

typedef struct kvm_const_dlob_data_s
{
    uint32_t size;
    const uint8_t * data;
} kvm_const_dlob_data_t;

/**< Key/Value provider callback type */
typedef void (* kvm_data_callback_t)(
    void *                          context,
    const kvm_const_dlob_data_t *   data);


/*!
*******************************************************************************
** Opens the client to work with Key/Value Management System.
**
** @param[out]  h_client        Pinter where opened client handle will be stored.
** @param[in]   server_ip       Zero terminated IP address of the server.
** @param[in]   server_port     Server port.
**
** @return
**      - KVM_RESULT_OK or corresponding KVM_RESULT_XXX in case of failure.
*/
kvm_result_t
kvm_client_open(
    kvm_client_handle_t *   h_client,
    const char *            server_ip,
    uint16_t                server_port);

/*!
*******************************************************************************
** Closes the client opened by kvm_client_open().
**
** @param[in]  h_client    Client handle.
**
** @return
**      - KVM_RESULT_OK or corresponding KVM_RESULT_XXX in case of failure.
*/
kvm_result_t
kvm_client_close(
    kvm_client_handle_t h_client);

/*!
*******************************************************************************
** Sends key/value pair to Key/Value Management System to store.
** If key already exists, it will be overriden.
**
** @param[in]   h_client    Client handle.
** @param[in]   key         Blob containig key.
** @param[in]   value       Blob containig value.
**
** @return
**      - KVM_RESULT_OK or corresponding KVM_RESULT_XXX in case of failure.
*/
kvm_result_t
kvm_client_put(
    kvm_client_handle_t     h_client,
    kvm_const_dlob_data_t * key,
    kvm_const_dlob_data_t * value);

/*!
*******************************************************************************
** Gets value by specified key from Key/Value Management System.
**
** @param[in]   h_client        Client handle.
** @param[in]   key             Blob containig key.
** @param[in]   callback        Callback function to provide value.
** @param[in]   user_context    User context which will be provided during callback call.
**
** @return
**      - KVM_RESULT_OK or corresponding KVM_RESULT_XXX in case of failure.
*/
kvm_result_t
kvm_client_get(
    kvm_client_handle_t     h_client,
    kvm_const_dlob_data_t * key,
    kvm_data_callback_t     callback,
    void *                  user_context);

/*!
*******************************************************************************
** Deletes key/value pair by specified key from Key/Value Management System.
**
** @param[in]   h_client    Client handle.
** @param[in]   key         Blob containig key.
**
** @return
**      - KVM_RESULT_OK or corresponding KVM_RESULT_XXX in case of failure.
*/
kvm_result_t
kvm_client_delete(
    kvm_client_handle_t     h_client,
    kvm_const_dlob_data_t * key);

/*!
*******************************************************************************
** Gets the list of all keys from Key/Value Management System.
**
** @param[in]   h_client        Client handle.
** @param[in]   callback        Callback function to provide keys.
**                              Call with data equal to NULL indicates
**                              the end of the list.
** @param[in]   user_context    User context which will be provided during callback call.
**
** @return
**      - KVM_RESULT_OK or corresponding KVM_RESULT_XXX in case of failure.
*/
kvm_result_t
kvm_client_list_keys(
    kvm_client_handle_t h_client,
    kvm_data_callback_t callback,
    void *              user_context);

/*!
*******************************************************************************
** Gets the count of all key/value pairs from Key/Value Management System.
**
** @param[in]   h_client    Client handle.
** @param[out]  count       Pointer where the count will be stored.
**
** @return
**      - KVM_RESULT_OK or corresponding KVM_RESULT_XXX in case of failure.
*/
kvm_result_t
kvm_client_count(
    kvm_client_handle_t h_client,
    uint32_t *          count);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __kvm_client_h__ */