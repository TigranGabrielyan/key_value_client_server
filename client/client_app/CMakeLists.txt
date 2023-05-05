ADD_EXECUTABLE(kvm_client_app main.c request_handler.c)

TARGET_LINK_LIBRARIES(kvm_client_app kvm_client kvm_client_transport kvm_utils apr-1)