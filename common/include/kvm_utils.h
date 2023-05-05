/**
 * @file kvm_utils.h
 *
 * @brief Defines utility functions used by Key/Value Management system.
 *
 */

#ifndef __kvm_utils_h__
#define __kvm_utils_h__

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

uint16_t
kvm_util_host_to_transport16(
    uint16_t u16);

uint32_t
kvm_util_host_to_transport32(
    uint32_t u32);

uint16_t
kvm_util_transport_to_host16(
    uint16_t u16);

uint32_t
kvm_util_transport_to_host32(
    uint32_t u32);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __kvm_utils_h__ */