/**
* @file kvm_client_transport.c
*
* @brief The module contains client transport library implementation.
*
*/

//#include <arpa/inet.h>
//#include <unistd.h>


#include "kvm_client_transport.h"



#include "kvm_client_transport_internal.h"

kvm_result_t
kvm_transport_open(
    kvm_transport_handle_t *    h_transport,
    const char *                server_ip,
    uint16_t                    server_port)
{
    WSADATA wsa;
    WSAStartup(MAKEWORD(2,2),&wsa);

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

    transport->client = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == transport->client)
    {
        kvm_transport_close(transport);
        return KVM_RESULT_SYS_CALL_FAIL;
    }

    if (-1 == connect(transport->client, (struct sockaddr *)&server_addr, sizeof(server_addr)))
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
    WSACleanup();

    if (NULL != h_transport)
    {
        if (-1 != h_transport->client)
        {
            //close(h_transport->client);
            closesocket(h_transport->client);
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
    uint32_t *              response_size,
    uint8_t **              response)
{
    if (NULL == h_transport || NULL == request || NULL == response_size || NULL == response || 0 == request_size)
    {
        return KVM_RESULT_INVALID_PARAM;
    }

    if (-1 == send(h_transport->client, (char *) &request_size, sizeof(request_size), 0))
    {
        return KVM_RESULT_SYS_CALL_FAIL;
    }

    if (-1 == send(h_transport->client, request, request_size, 0))
    {
        return KVM_RESULT_SYS_CALL_FAIL;
    }

    uint32_t size;
    uint32_t read_len = recv(h_transport->client, (char *) &size, sizeof(size), 0);
    //read_len = read(h_transport->client, &size, sizeof(size));
    if (read_len == -1)
    {
        return KVM_RESULT_SYS_CALL_FAIL;
    }

    uint8_t * buff = malloc(size);
    if (NULL == buff)
    {
        return KVM_RESULT_SYS_CALL_FAIL;
    }

    read_len = recv(h_transport->client, buff, size, 0);
    //read_len = read(h_transport->client, buff, size, 0);
    if (read_len == -1)
    {
        free(buff);
        return KVM_RESULT_SYS_CALL_FAIL;
    }

    *response_size = size;
    *response = buff;

    return KVM_RESULT_OK;
}