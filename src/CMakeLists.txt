# 当前文件夹中所有文件 别名 SRC_LIST
aux_source_directory(. SRC_LIST)
# 生成静态库
add_library(mprpc ${SRC_LIST})
# 依赖muduo库
target_link_libraries(mprpc muduo_net muduo_base pthread)