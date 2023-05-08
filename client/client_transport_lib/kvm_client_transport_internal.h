/**
 * @file kvm_client_transport_internal.h
 *
 * @brief Defines the internal structures and helper functions.
 *
 */

#ifndef __kvm_client_transport_internal_h__
#define __kvm_client_transport_internal_h__

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* Client context */
struct kvm_transprot_s
{
    int client_socket;
};

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __kvm_client_transport_internal_h__ */