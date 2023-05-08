INCLUDE_DIRECTORIES(../../../external/googletest/include)

LINK_DIRECTORIES(../../../external/googletestexternal/apr/lib)


ADD_EXECUTABLE(kvm_test
    client.cc
    server.cc
)

TARGET_LINK_LIBRARIES(kvm_test
    gtest
    gtest_main
    pthread
    apr-1
)
