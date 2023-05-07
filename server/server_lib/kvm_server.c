/**
* @file kvm_server.c
*
* @brief Key/Value Management System server implementation.
*
*/
#include <stdlib.h>
#include <string.h>

#include "kvm_requests.h"
#include "kvm_replies.h"
#include "kvm_server.h"
#include "kvm_server_internal.h"

kvm_server_t g_server;

static kvm_result_t process_client(SOCKET client);
static void close_client(SOCKET client);

kvm_result_t
kvm_server_init(
    uint16_t port)
{
    WSADATA wsa;
    WSAStartup(MAKEWORD(2,2),&wsa);

    // Create server socket
    g_server.server = socket(AF_INET, SOCK_STREAM, 0);
    if (g_server.server == -1)
    {
        return KVM_RESULT_SYS_CALL_FAIL;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (-1 == bind(g_server.server, (struct sockaddr *)&server_addr, sizeof(server_addr)))
    {
        closesocket(g_server.server);
        //close(g_server.server);
        return KVM_RESULT_SYS_CALL_FAIL;
    }

    if (-1 == listen(g_server.server, 5))
    {
        closesocket(g_server.server);
        //close(server_sock);
        return KVM_RESULT_SYS_CALL_FAIL;
    }

    kvm_result_t result = init_apr_hashtable();
    if (KVM_RESULT_OK != result)
    {
        return result;
    }

    FD_ZERO(&g_server.readfds);
    FD_SET(g_server.server, &g_server.readfds);
    g_server.max_fd = (int) g_server.server;

    return result;
}

kvm_result_t
kvm_server_uninit(
    void)
{
    uninit_apr_hashtable();

    for(int i = 0; i < MAX_CLIENT_COUNT; ++i)
    {
        if (0 != g_server.clients[i])
        {
            closesocket(g_server.clients[i]);
            //close(g_server.clients[i]);
        }
    }

     closesocket(g_server.server);
    //close(server_sock);

    memset(&g_server, 0, sizeof(g_server));
    WSACleanup();
    return KVM_RESULT_OK;
}

kvm_result_t
kvm_server_wait_client_request(
    void)
{
    while(1)
    {
        fd_set current_set = g_server.readfds;

        if (-1 == select(g_server.max_fd, &current_set, NULL, NULL, NULL))
        {
            // handle signal termination
            return KVM_RESULT_SYS_CALL_FAIL;
        }

        if (FD_ISSET(g_server.server, &current_set))
        {
            SOCKET client_sock = accept(g_server.server, NULL, NULL);
            if (-1 == client_sock)
            {
                return KVM_RESULT_SYS_CALL_FAIL;
            }

            for (int i = 0; i < MAX_CLIENT_COUNT; i++)
            {
                if (0 == g_server.clients[i])
                {
                    g_server.clients[i] = client_sock;
                    break;
                }
            }

            FD_SET(client_sock, &g_server.readfds);

            if (client_sock > g_server.max_fd)
            {
                g_server.max_fd = client_sock;
            }

        }

        memset(&g_server.active_clients, 0, sizeof(g_server.active_clients));
        int found_count = 0;
        for (int i = 0; i < MAX_CLIENT_COUNT; i++)
        {
            if (0 != g_server.clients[i] && FD_ISSET(g_server.clients[i], &current_set))
            {
                g_server.active_clients[i] = g_server.clients[i];
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
        if (0 != g_server.active_clients[i])
        {
           process_client(g_server.active_clients[i]);
        }
    }
    return KVM_RESULT_OK;
}

kvm_result_t process_client(SOCKET client)
{
    uint32_t request_size;

    //uint32_t read_len = read(client, request_size, sizeof(request_size));
    uint32_t read_len = recv(client, &request_size, sizeof(request_size), 0);
    if (sizeof(request_size) != read_len)
    {
        close_client(client);
        return KVM_RESULT_CONNECTION_FAIL;
    }

    uint8_t * request = malloc(request_size);
    if (NULL == request)
    {
        return KVM_RESULT_SYS_CALL_FAIL;
    }

    //read_len = read(client, request, request_size);
    read_len = recv(client, request, request_size, 0);
    if (request_size != read_len)
    {
        close_client(client);
        return KVM_RESULT_CONNECTION_FAIL;
    }

    uint32_t reply_size;
    uint8_t * reply;
    kvm_result_t result = handle_request(request_size, request, &reply_size, &reply);
    if (KVM_RESULT_OK != result)
    {
        return result;
    }

    //uint32_t wr_len = write(client, &reply_size, sizeof(reply_size));
    uint32_t wr_len = send(client, &reply_size, sizeof(reply_size), 0);
    if (sizeof(request_size) != wr_len)
    {
        free(reply);
        close_client(client);
        return KVM_RESULT_CONNECTION_FAIL;
    }

    //wr_len = write(client, reply, reply_size);
    wr_len = send(client, reply, reply_size, 0);
    if (reply_size != wr_len)
    {
        free(reply);
        close_client(client);
        return KVM_RESULT_CONNECTION_FAIL;
    }

    return KVM_RESULT_OK;
}

static void close_client(SOCKET client)
{
    //close(sd);
    closesocket(client);

    FD_CLR(client, &g_server.readfds);

    for (int i = 0; i < MAX_CLIENT_COUNT; i++)
    {
        if (g_server.active_clients[i] == client)
        {
            g_server.active_clients[i] = 0;
        }

        if (g_server.clients[i] == client)
        {
            g_server.clients[i] = 0;
        }
    }
}

#if 0
void test(void)
{
    uint8_t request[100];
    uint32_t request_size = 0;

    uint8_t * reply;
    uint32_t reply_size = 0;

    uint8_t * ptr;
    uint32_t tmp;
    kvm_result_t result;
    kvm_reply_status_t status;

    const char * key1 = "key1";
    uint32_t key1_size = (uint32_t) strlen(key1);
    const char * key2 = "key2";
    uint32_t key2_size = (uint32_t) strlen(key2);

    const char * value1 = "value1";
    uint32_t value1_size = (uint32_t) strlen(value1);
    const char * value2 = "value2";
    uint32_t value2_size = (uint32_t) strlen(value2);

    /* Add key 1 */
    ptr = request;
    ((kvm_request_generic_t *) ptr)->id = KVM_REQUST_PUT;
    ptr += sizeof (kvm_request_generic_t);
    ((kvm_request_put_t *) ptr)->key_size = key1_size;
    ((kvm_request_put_t *) ptr)->value_size = value1_size;
    ptr += sizeof(kvm_request_put_t);
    memcpy(ptr, key1, key1_size);
    memcpy(ptr + key1_size, value1, value1_size);

    request_size = sizeof(kvm_request_generic_t) + sizeof(kvm_request_put_t) + key1_size + value1_size;
    result = handle_request(request_size, request, &reply_size, &reply);

    status = ((kvm_reply_generic_t *) reply)->status;

    /* Add key 2 */
    ptr = request;
    ((kvm_request_generic_t *) ptr)->id = KVM_REQUST_PUT;
    ptr += sizeof (kvm_request_generic_t);
    ((kvm_request_put_t *) ptr)->key_size = key2_size;
    ((kvm_request_put_t *) ptr)->value_size = value2_size;
    ptr += sizeof(kvm_request_put_t);
    memcpy(ptr, key2, key2_size);
    memcpy(ptr + key2_size, value2, value2_size);

    request_size = sizeof(kvm_request_generic_t) + sizeof(kvm_request_put_t) + key2_size + value2_size;
    result = handle_request(request_size, request, &reply_size, &reply);

    status = ((kvm_reply_generic_t *) reply)->status;
    free(reply);

    /* Get key 1 */
    ptr = request;
    ((kvm_request_generic_t *) ptr)->id = KVM_REQUST_GET;
    ptr += sizeof (kvm_request_generic_t);
    ((kvm_request_get_t *) ptr)->key_size = key1_size;
    ptr += sizeof(kvm_request_get_t);
    memcpy(ptr, key1, key1_size);

    request_size = sizeof(kvm_request_generic_t) + sizeof(kvm_request_get_t) + key1_size;
    result = handle_request(request_size, request, &reply_size, &reply);

    status = ((kvm_reply_generic_t *) reply)->status;
    ptr = reply + sizeof(kvm_reply_generic_t);
    tmp = ((kvm_reply_get_t *) ptr)->value_size;
    free(reply);

    /* Get count */
    ptr = request;
    ((kvm_request_generic_t *) ptr)->id = KVM_REQUST_COUNT;

    request_size = sizeof(kvm_request_generic_t);
    result = handle_request(request_size, request, &reply_size, &reply);

    status = ((kvm_reply_generic_t *) reply)->status;
    ptr = reply + sizeof(kvm_reply_generic_t);
    tmp = ((kvm_reply_count_t *) ptr)->count;
    free(reply);

    /* Get List */
    ptr = request;
    ((kvm_request_generic_t *) ptr)->id = KVM_REQUST_LIST;

    request_size = sizeof(kvm_request_generic_t);
    result = handle_request(request_size, request, &reply_size, &reply);

    status = ((kvm_reply_generic_t *) reply)->status;
    ptr = reply + sizeof(kvm_reply_generic_t);
    tmp = ((kvm_reply_list_t *) ptr)->count;
    free(reply);

    /* Delete key1 */
    ptr = request;
    ((kvm_request_generic_t *) ptr)->id = KVM_REQUST_DELETE;
    ptr += sizeof (kvm_request_generic_t);
    ((kvm_request_delete_t *) ptr)->key_size = key1_size;
    ptr += sizeof(kvm_request_delete_t);
    memcpy(ptr, key1, key1_size);

    request_size = sizeof(kvm_request_generic_t) + sizeof(kvm_request_delete_t) + key1_size;
    result = handle_request(request_size, request, &reply_size, &reply);

    status = ((kvm_reply_generic_t *) reply)->status;
    free(reply);

    /* Get key 1 */
    ptr = request;
    ((kvm_request_generic_t *) ptr)->id = KVM_REQUST_GET;
    ptr += sizeof (kvm_request_generic_t);
    ((kvm_request_get_t *) ptr)->key_size = key1_size;
    ptr += sizeof(kvm_request_get_t);
    memcpy(ptr, key1, key1_size);

    request_size = sizeof(kvm_request_generic_t) + sizeof(kvm_request_get_t) + key1_size + value1_size;
    result = handle_request(request_size, request, &reply_size, &reply);

    status = ((kvm_reply_generic_t *) reply)->status;
    ptr = reply + sizeof(kvm_reply_generic_t);
    tmp = ((kvm_reply_get_t *) ptr)->value_size;
    free(reply);

    /* Get count */
    ptr = request;
    ((kvm_request_generic_t *) ptr)->id = KVM_REQUST_COUNT;

    request_size = sizeof(kvm_request_generic_t);
    result = handle_request(request_size, request, &reply_size, &reply);

    status = ((kvm_reply_generic_t *) reply)->status;
    ptr = reply + sizeof(kvm_reply_generic_t);
    tmp = ((kvm_reply_count_t *) ptr)->count;
    free(reply);
}
#endif