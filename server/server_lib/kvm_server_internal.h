
/**
 * @file kvm_server_internal.h
 *
 * @brief Defines the internal structures and helper functions.
 *
 */

#ifndef __kvm_server_internal_h__
#define __kvm_server_internal_h__

#include <sys/select.h>
#include "kvm_results.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#define MAX_CLIENT_COUNT 64

typedef struct kvm_server_s
{
    int server_socket;

    fd_set readfds;
    int max_fd;

    int client_sockets[MAX_CLIENT_COUNT];
    int active_client_sockets[MAX_CLIENT_COUNT];
} kvm_server_t;

kvm_result_t init_apr_hashtable(void);
void uninit_apr_hashtable(void);

kvm_result_t handle_request(uint32_t request_size, const uint8_t * request, uint32_t * reply_size, uint8_t ** reply);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __kvm_server_internal_h__ */