/**
 * @file kvm_client_internal.h
 *
 * @brief Defines the internal structures and helper functions.
 *
 */

#ifndef __kvm_client_internal_h__
#define __kvm_client_internal_h__

#include "kvm_results.h"
#include "kvm_client_transport.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* Client context */
struct kvm_client_s
{
    kvm_transport_handle_t h_transport;
};

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __kvm_client_internal_h__ */