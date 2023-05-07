SET(LIB_NAME kvm_server)

SET(SRC_FILES kvm_server.c kvm_request_handler.c)

ADD_LIBRARY(${LIB_NAME} ${SRC_FILES})