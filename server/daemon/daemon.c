#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <syslog.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "kvm_server.h"

volatile sig_atomic_t stop_running = 0;

static void daemonize(void)
{
    // Fork parent process
    pid_t pid = fork();
    if (pid < 0)
    {
        exit(EXIT_FAILURE);
    }

    // Exit from parent
    if (pid > 0)
    {
        exit(EXIT_SUCCESS);
    }

    // Detach from terminal
    if (setsid() < 0)
    {
        exit(EXIT_FAILURE);
    }

    // Set file permissions
    umask(0);

    // Change working directory to root
    if (chdir("/") < 0)
    {
        exit(EXIT_FAILURE);
    }

    // Close standard file descriptors
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

static void signal_handler(int signal)
{
    switch (signal)
    {
        case SIGHUP:
        {
            break;
        }
        case SIGTERM:
        {
            syslog(LOG_INFO, "Key/Value Management System server interrupted by SIGTERM");
            stop_running = 1;
            break;
        }
        default:
        {
            break;
        }
    }
}

static uint16_t get_port_from_config(void)
{
    const uint16_t default_port = 55555;
    FILE * f = fopen("server.config", "r");
    if (NULL == f)
    {
        return default_port;
    }

    uint16_t port = 0;
    if (1 != fscanf(f, "%u", &port))
    {
        fclose(f);
        return default_port;
    }

    fclose(f);
    return port;
}

void main()
{
    uint16_t port = get_port_from_config();

    daemonize();

    openlog(NULL, LOG_PID, LOG_DAEMON);
    syslog(LOG_INFO, "Key/Value Management System server started on %u port", port);

    signal(SIGHUP, signal_handler);
    signal(SIGTERM, signal_handler);

    kvm_result_t result = kvm_server_init(port);
    if (KVM_RESULT_OK != result)
    {
        syslog(LOG_ERR, "kvm_server_init() failed: %s", strerror(errno));
    }

    while (!stop_running)
    {
        kvm_result_t result = kvm_server_wait_client_request();
        if (KVM_RESULT_OK != result)
        {
            if (EINTR == errno)
            {
                /* Interrupted by signal. Simply restart waiting. */
                continue;
            }
            syslog(LOG_ERR, "kvm_server_wait_client_request() failed: %s", strerror(errno));
            exit(EXIT_FAILURE);
        }

        result = kvm_server_handle_request();
        if (KVM_RESULT_OK != result)
        {
            syslog(LOG_ERR, "kvm_server_handle_request() failed: %s", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    result = kvm_server_uninit();
    if (KVM_RESULT_OK != result)
    {
        syslog(LOG_ERR, "kvm_server_uninit() failed: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    syslog(LOG_INFO, "Key/Value Management System server exited");
    closelog();
    exit(EXIT_SUCCESS);
}

