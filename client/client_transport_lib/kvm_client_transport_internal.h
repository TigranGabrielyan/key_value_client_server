/**
 * @file kvm_client_transport_internal.h
 *
 * @brief Defines the internal structures and helper functions.
 *
 */

#ifndef __kvm_client_transport_internal_h__
#define __kvm_client_transport_internal_h__

//#include <sys/socket.h>
#include <winsock2.h>

#include "kvm_results.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* Client context */
struct kvm_transprot_s
{
    SOCKET client;
};

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __kvm_client_transport_internal_h__ */