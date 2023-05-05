/**
 * @file kvm_requests.h
 *
 * @brief Defines the request ID's and corresponding structures supported by Key/Value Management system.
 *
 */

#ifndef __kvm_requests_h__
#define __kvm_requests_h__

#include "kvm_results.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

typedef uint8_t kvm_request_id_t;
/* Definitions of the supported request Ids. */
#define KVM_REQUST_NOOP     ((kvm_request_id_t) 0)
#define KVM_REQUST_PUT      ((kvm_request_id_t) 1)
#define KVM_REQUST_GET      ((kvm_request_id_t) 2)
#define KVM_REQUST_DELETE   ((kvm_request_id_t) 3)
#define KVM_REQUST_LIST     ((kvm_request_id_t) 4)
#define KVM_REQUST_COUNT    ((kvm_request_id_t) 5)

#pragma pack(push, 1)
typedef struct kvm_request_generic_s
{
    kvm_request_id_t id;
} kvm_request_generic_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct kvm_request_by_key_s
{
    uint32_t key_size;
    /* Followed by key data */
} kvm_request_by_key_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct kvm_request_by_key_value_s
{
    uint32_t key_size;
    uint32_t value_size;
    /* Followed by key data + value data */
} kvm_request_by_key_value_t;
#pragma pack(pop)

typedef kvm_request_by_key_value_t kvm_request_put_t;
typedef kvm_request_by_key_t kvm_request_get_t;
typedef kvm_request_by_key_t kvm_request_delete_t;
typedef kvm_request_generic_t kvm_request_list_t;
typedef kvm_request_generic_t kvm_request_count_t;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __kvm_requests_h__ */
