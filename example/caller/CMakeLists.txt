set(SRC_LIST calluserservice.cc ../user.pb.cc)

add_executable(consumer ${SRC_LIST})
# 声明编译时需要链接mprpc、protobuf动态库
target_link_libraries(consumer mprpc protobuf)