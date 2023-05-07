#ifndef __request_handler_h__
#define __request_handler_h__

#include <stdint.h>
#include "kvm_client.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

int init_request_handler(void);
void uninit_request_handler(void);

int handle_request(const kvm_client_handle_t h_client, char * input_line);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __request_handler_h__ */