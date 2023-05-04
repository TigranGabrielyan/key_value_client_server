ADD_EXECUTABLE(jel_test
    client.cc
    server.cc
)

TARGET_LINK_LIBRARIES(jel_test
    gtest
    gtest_main
    pthread
    apr-1
)
