cmake_minimum_required(VERSION 2.8.3)
project(sbusHanle-go2)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -O3 -march=native -Wall -std=c++11")

include_directories(include src/CJson/Inc)
include_directories(/usr/local/include/ddscxx 
                    /usr/local/include/iceoryx/v2.0.2
                    /usr/local/include/unitree/
                    # /usr/local/include/ros2_idl
                    /usr/local/include/common/dds
                    )
aux_source_directory(src SRC_LIST)
add_subdirectory(src/CJson)

link_libraries(unitree_go2_idl_cpp common_dds_utils ddsc ddscxx rt pthread)

# if (CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
#   message(STATUS "[${PROJECT_NAME}] current cpu type is: x86_64")
# elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
#   message(STATUS "[${PROJECT_NAME}] current cpu type is: aarch64")
# else()
#   message(STATUS "[${PROJECT_NAME}] current cpu type is: unknown!!!")
# endif()
# LINK_DIRECTORIES(./thirdparty/lib/${CMAKE_SYSTEM_PROCESSOR})

add_executable(sbus_handle ${SRC_LIST})
target_link_libraries(sbus_handle Cjson)

# set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

