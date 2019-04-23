#CMake 最低版本号要求
cmake_minimum_required (VERSION 2.8)
# 项目信息
project (webrtc_extract)

include_directories(${CMAKE_CURRENT_LIST_DIR}/../include)
link_directories(${CMAKE_CURRENT_LIST_DIR}/../lib)
add_executable(${PROJECT_NAME} ./main.cpp)
target_link_libraries(${PROJECT_NAME} noise_suppression)

SET(CMAKE_CXX_COMPILER      "g++" )
SET(CMAKE_CXX_FLAGS   "-std=c++11")
#SET(CMAKE_BUILD_TYPE "Debug")
#SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
#SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")
