#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include"request_handler.h"

static int extract_ip_and_port(const char * input, char ** ip, uint32_t * port)
{
    const char * delimiter = strchr(input, ':');
    if (NULL == delimiter)
    {
        return 0;
    }

    const size_t ip_size = delimiter - input;
    char * ptr = (char *) malloc(ip_size + sizeof('\0'));
    if (NULL == ptr)
    {
        printf("Memory allocation failed\n");
        return 0;
    }
    memcpy(ip, input, ip_size);
    ptr[ip_size] = '\0';
    *ip = ptr;

    ptr = NULL;
    *port = strtol(delimiter + 1, &ptr, 10);
    if ('\0' != *ptr)
    {
        free(*ip);
        return 0;
    }
    return 1;
}

static kvm_client_handle_t init_client(char * command_line)
{
    char * ip;
    uint32_t port;

    if (!extract_ip_and_port(command_line, &ip, &port))
    {
        printf("Please specify server IP and port in <IP>:<PORT> format\n");
        return NULL;
    }

    kvm_client_handle_t h_client = NULL;
    kvm_result_t result = KVM_RESULT_INVALID;

    result = kvm_client_open(&h_client, ip, port);
    if (KVM_RESULT_OK != result)
    {
        printf("kvm_client_open failed: error %d\n", result);
    }

    free(ip);

    return h_client;
}

static void print_welcome_message()
{
    printf("Welcome to KVM Client. Below are the supported requests:\n");
    printf("put <key>=<value>   - store key/value pair in server\n");
    printf("get <key>           - retrieve value with specified key from server\n");
    printf("del <key>           - delete value with specified key from server\n");
    printf("list-keys           - get all keys from the server\n");
    printf("count               - get count of key/value pairs stored on the server\n");
    printf("quit                - exit from application\n");
}

int main(int argc, char * argv[])
{
    if (argc != 2)
    {
        printf("Please specify server IP and port in <IP>:<PORT> format\n");
        return 1;
    }

    if (!init_request_handler())
    {
        return 1;
    }

    kvm_client_handle_t h_client = NULL;
    h_client = init_client(argv[1]);
    if (NULL == h_client)
    {
        uninit_request_handler();
        return 1;
    }

    print_welcome_message();

    while(1)
    {
        char input_line[1024];

        printf("Enter request:\n");

        const int read_symbols = scanf("%1023s", &input_line);
        if (sizeof(input_line) == read_symbols)
        {
            printf("Request too long. Maximum allowed length is 1024");
            continue;
        }

        if (!handle_request(h_client, input_line))
        {
            break;
        }
    }

    uninit_request_handler();
    kvm_client_close(h_client);

    return 0;
}