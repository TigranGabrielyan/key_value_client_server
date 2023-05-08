#if 0
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <syslog.h>
#include <sys/stat.h>

void daemonize() {
    pid_t pid, sid;

    // Fork parent process
    pid = fork();
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    // Detach from terminal
    if (setsid() < 0) {
        exit(EXIT_FAILURE);
    }

    // Change working directory to root
    if (chdir("/") < 0) {
        exit(EXIT_FAILURE);
    }

    // Close standard file descriptors
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

void signal_handler(int signal) {
    switch (signal) {
        case SIGTERM:
            syslog(LOG_INFO, "Received SIGTERM signal.");
            exit(EXIT_SUCCESS);
            break;
        default:
            break;
    }
}

int main(int argc, char *argv[]) {
    // Daemonize the process
    daemonize();

    // Open system log
    openlog("mydaemon", LOG_PID, LOG_DAEMON);

    // Set signal handlers
    signal(SIGTERM, signal_handler);

    // Main loop
    while (1) {
        // Do some work here...
        syslog(LOG_INFO, "Daemon is running.");
        sleep(10);
    }

    // Close system log
    closelog();

    return 0;
}
#endif

#include <stdio.h>

#include "kvm_server.h"

void main()
{
    kvm_server_init(55555);

    int count = 10;
    while (0 != count)
    {
        kvm_result_t result = kvm_server_wait_client_request();
        if (KVM_RESULT_OK != result)
        {
            printf("kvm_server_wait_client_request failed: error %d", result);
            break;
        }

        result = kvm_server_handle_request();
        if (KVM_RESULT_OK != result)
        {
            printf("kvm_server_handle_request failed: error %d", result);
            break;
        }

        --count;
    }

    kvm_server_uninit();
}

