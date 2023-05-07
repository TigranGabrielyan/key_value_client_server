ADD_EXECUTABLE(kvm_daemon daemon.c)

TARGET_LINK_LIBRARIES(kvm_daemon kvm_server kvm_utils apr-1)