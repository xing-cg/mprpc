set(SRC_LIST userservice.cc ../user.pb.cc)

add_executable(provider ${SRC_LIST})
# 声明编译时需要链接mprpc、protobuf动态库
target_link_libraries(provider mprpc protobuf)