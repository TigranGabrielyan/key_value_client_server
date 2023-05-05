/**
 * @file kvm_results.h
 *
 * @brief Defines result codes used by Key/Value Management system.
 *
 */

#ifndef __kvm_results_h__
#define __kvm_results_h__

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

typedef uint32_t kvm_result_t;

/* Result codes */
#define KVM_RESULT_OK               ((kvm_result_t) 0)
#define KVM_RESULT_INVALID_PARAM    ((kvm_result_t) 1)
#define KVM_RESULT_SYS_CALL_FAIL    ((kvm_result_t) 2)
#define KVM_RESULT_CONNECTION_FAIL  ((kvm_result_t) 3)
#define KVM_RESULT_INVALID          ((kvm_result_t) 4)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __kvm_results__ */
