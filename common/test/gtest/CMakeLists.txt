INCLUDE_DIRECTORIES(../../../server/server_lib)
INCLUDE_DIRECTORIES(../../../client/include)

ADD_EXECUTABLE(kvm_test
    client.cc
    server.cc
    client_transport_mock.cc
)

TARGET_LINK_LIBRARIES(kvm_test
    kvm_server
    kvm_client
    kvm_utils    
    gtest
    gtest_main
    pthread
    apr-1
)
