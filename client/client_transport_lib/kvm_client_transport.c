/**
* @file kvm_client_transport.c
*
* @brief The module contains client transport library implementation.
*
*/

#include<string.h>
#include<unistd.h>

#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "kvm_client_transport.h"
#include "kvm_client_transport_internal.h"

kvm_result_t
kvm_transport_open(
    kvm_transport_handle_t *    h_transport,
    const char *                server_ip,
    uint16_t                    server_port)
{
    if (NULL == h_transport || NULL == server_ip)
    {
        return KVM_RESULT_INVALID_PARAM;
    }

    const kvm_transport_handle_t transport = (kvm_transport_handle_t) malloc(sizeof(struct kvm_transprot_s));
    if (NULL == transport)
    {
        return KVM_RESULT_SYS_CALL_FAIL;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(server_ip);
    server_addr.sin_port = htons(server_port);

    transport->client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == transport->client_socket)
    {
        kvm_transport_close(transport);
        return KVM_RESULT_SYS_CALL_FAIL;
    }

    if (-1 == connect(transport->client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)))
    {
        kvm_transport_close(transport);
        return KVM_RESULT_CONNECTION_FAIL;
    }

    *h_transport = transport;

    return KVM_RESULT_OK;
}

kvm_result_t
kvm_transport_close(
    kvm_transport_handle_t h_transport)
{
    if (NULL != h_transport)
    {
        if (-1 != h_transport->client_socket)
        {
            close(h_transport->client_socket);
        }

        free(h_transport);
    }
    return KVM_RESULT_OK;
}

kvm_result_t
kvm_transport_send(
    kvm_transport_handle_t  h_transport,
    uint32_t                request_size,
    const uint8_t *         request,
    uint32_t *              reply_size,
    uint8_t **              reply)
{
    if (NULL == h_transport || NULL == request || NULL == reply_size || NULL == reply || 0 == request_size)
    {
        return KVM_RESULT_INVALID_PARAM;
    }

    if (-1 == send(h_transport->client_socket, (char *) &request_size, sizeof(request_size), 0))
    {
        return KVM_RESULT_SYS_CALL_FAIL;
    }

    if (-1 == send(h_transport->client_socket, request, request_size, 0))
    {
        return KVM_RESULT_SYS_CALL_FAIL;
    }

    uint32_t size;
    uint32_t read_len = read(h_transport->client_socket, &size, sizeof(size));
    if (read_len == -1)
    {
        return KVM_RESULT_SYS_CALL_FAIL;
    }

    uint8_t * buff = malloc(size);
    if (NULL == buff)
    {
        return KVM_RESULT_SYS_CALL_FAIL;
    }

    read_len = read(h_transport->client_socket, buff, size);
    if (read_len == -1)
    {
        free(buff);
        return KVM_RESULT_SYS_CALL_FAIL;
    }

    *reply_size = size;
    *reply = buff;

    return KVM_RESULT_OK;
}