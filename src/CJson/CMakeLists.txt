cmake_minimum_required(VERSION 3.10.2)
find_package(Threads)

# include_directories(../Inc)
include_directories(Inc)

aux_source_directory(Src DIR_LIB_SRCS)
add_library(Cjson STATIC ${DIR_LIB_SRCS})


#生成文件到指定位置
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build)



