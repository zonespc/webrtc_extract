#CMake 最低版本号要求
cmake_minimum_required (VERSION 2.8)
# 项目信息
project (webrtc_extract)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_LIST_DIR}/../bin)

include_directories(${CMAKE_CURRENT_LIST_DIR}/../include)
link_directories(${CMAKE_CURRENT_LIST_DIR}/../lib)
add_executable(${PROJECT_NAME} ./main.cpp)
target_link_libraries(${PROJECT_NAME} noise_suppression ns fft4g signal_processing rtc_base test_utils api_audio common_audio audio_butter splitting_filter three_band_filter_bank resampler memory agc_legacy agc2 system_wrappers audio_logging rnn_vad audio_utility rnnoise)

SET(CMAKE_CXX_COMPILER      "g++" )
SET(CMAKE_CXX_FLAGS   "-std=c++11")
#SET(CMAKE_BUILD_TYPE "Debug")
#SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
#SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")
