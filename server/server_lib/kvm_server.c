/**
* @file kvm_server.c
*
* @brief Key/Value Management System server implementation.
*
*/
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <arpa/inet.h>

#include "kvm_requests.h"
#include "kvm_replies.h"
#include "kvm_server.h"
#include "kvm_server_internal.h"

kvm_server_t g_server;

static kvm_result_t process_client(int client_socket);
static void close_client(int client_socket);

kvm_result_t
kvm_server_init(
    uint16_t port)
{
    g_server.server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (g_server.server_socket == -1)
    {
        return KVM_RESULT_SYS_CALL_FAIL;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (-1 == bind(g_server.server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)))
    {
        close(g_server.server_socket);
        return KVM_RESULT_SYS_CALL_FAIL;
    }

    if (-1 == listen(g_server.server_socket, 5))
    {
        close(g_server.server_socket);
        return KVM_RESULT_SYS_CALL_FAIL;
    }

    kvm_result_t result = init_apr_hashtable();
    if (KVM_RESULT_OK != result)
    {
        return result;
    }

    FD_ZERO(&g_server.readfds);
    FD_SET(g_server.server_socket, &g_server.readfds);
    g_server.max_fd = g_server.server_socket;

    return result;
}

kvm_result_t
kvm_server_uninit(
    void)
{
    uninit_apr_hashtable();

    for(int i = 0; i < MAX_CLIENT_COUNT; ++i)
    {
        if (0 != g_server.client_sockets[i])
        {
            close(g_server.client_sockets[i]);
        }
    }

    close(g_server.server_socket);

    memset(&g_server, 0, sizeof(g_server));
    return KVM_RESULT_OK;
}

kvm_result_t
kvm_server_wait_client_request(
    void)
{
    while(1)
    {
        fd_set current_set = g_server.readfds;

        if (-1 == select(FD_SETSIZE, &current_set, NULL, NULL, NULL))
        {
            return KVM_RESULT_SYS_CALL_FAIL;
        }

        if (FD_ISSET(g_server.server_socket, &current_set))
        {
            int client_sock = accept(g_server.server_socket, NULL, NULL);
            if (-1 == client_sock)
            {
                return KVM_RESULT_SYS_CALL_FAIL;
            }

            for (int i = 0; i < MAX_CLIENT_COUNT; i++)
            {
                if (0 == g_server.client_sockets[i])
                {
                    g_server.client_sockets[i] = client_sock;
                    break;
                }
            }

            FD_SET(client_sock, &g_server.readfds);

            if (client_sock > g_server.max_fd)
            {
                g_server.max_fd = client_sock;
            }

        }

        memset(&g_server.active_client_sockets, 0, sizeof(g_server.active_client_sockets));
        int found_count = 0;
        for (int i = 0; i < MAX_CLIENT_COUNT; i++)
        {
            if (0 != g_server.client_sockets[i] && FD_ISSET(g_server.client_sockets[i], &current_set))
            {
                g_server.active_client_sockets[i] = g_server.client_sockets[i];
                found_count++;
            }
        }

        if (0 != found_count)
        {
            break;
        }
    }

    return KVM_RESULT_OK;
}

void test(void);

kvm_result_t
kvm_server_handle_request(
    void)
{
    for (int i = 0; i < MAX_CLIENT_COUNT; i++)
    {
        if (0 != g_server.active_client_sockets[i])
        {
           process_client(g_server.active_client_sockets[i]);
        }
    }
    return KVM_RESULT_OK;
}

kvm_result_t process_client(int client_socket)
{
    uint32_t request_size;

    uint32_t read_len = read(client_socket, &request_size, sizeof(request_size));
    if (sizeof(request_size) != read_len)
    {
        close_client(client_socket);
        return KVM_RESULT_CONNECTION_FAIL;
    }

    uint8_t * request = malloc(request_size);
    if (NULL == request)
    {
        return KVM_RESULT_SYS_CALL_FAIL;
    }

    read_len = read(client_socket, request, request_size);
    if (request_size != read_len)
    {
        close_client(client_socket);
        return KVM_RESULT_CONNECTION_FAIL;
    }

    uint32_t reply_size;
    uint8_t * reply;
    kvm_result_t result = handle_request(request_size, request, &reply_size, &reply);
    if (KVM_RESULT_OK != result)
    {
        return result;
    }

    uint32_t wr_len = write(client_socket, &reply_size, sizeof(reply_size));
    if (sizeof(request_size) != wr_len)
    {
        free(reply);
        close_client(client_socket);
        return KVM_RESULT_CONNECTION_FAIL;
    }

    wr_len = write(client_socket, reply, reply_size);
    if (reply_size != wr_len)
    {
        free(reply);
        close_client(client_socket);
        return KVM_RESULT_CONNECTION_FAIL;
    }

    return KVM_RESULT_OK;
}

static void close_client(int client_socket)
{
    close(client_socket);

    FD_CLR(client_socket, &g_server.readfds);

    for (int i = 0; i < MAX_CLIENT_COUNT; i++)
    {
        if (g_server.active_client_sockets[i] == client_socket)
        {
            g_server.active_client_sockets[i] = 0;
        }

        if (g_server.client_sockets[i] == client_socket)
        {
            g_server.client_sockets[i] = 0;
        }
    }
}