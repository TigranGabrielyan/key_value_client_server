/**
 * @file kvm_replies.h
 *
 * @brief Defines the reply structures supported by Key/Value Management system.
 *
 */

#ifndef __kvm_replies_h__
#define __kvm_replies_h__

#include "kvm_results.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

typedef uint8_t kvm_reply_status_t;

/* Reply status codes */
#define KVM_REPLY_STATUS_OK     ((kvm_reply_status_t) 0)


#pragma pack(push, 1)
typedef struct kvm_reply_generic_s
{
    kvm_reply_status_t status;
} kvm_reply_generic_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct kvm_reyply_value_s
{
    uint32_t value_size;
    /* Followed by value data */
} kvm_reyply_value_t;
#pragma pack(pop)

typedef kvm_reply_generic_t kvm_reply_put_t;
typedef kvm_reyply_value_t  kvm_reply_get_t;
typedef kvm_reply_generic_t kvm_reply_delete_t;

#pragma pack(push, 1)
typedef struct kvm_reply_list_s
{
    uint32_t count;
    /* Followed by key size (uint32_t), key data (<key size>) */
} kvm_reply_list_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct kvm_reply_count_s
{
    uint32_t count;
} kvm_reply_count_t;
#pragma pack(pop)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __kvm_replies_h__ */
