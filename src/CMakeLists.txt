# 当前文件夹中所有文件 别名 SRC_LIST
aux_source_directory(. SRC_LIST)
# 生成动态库
add_library(mprpc SHARED ${SRC_LIST})