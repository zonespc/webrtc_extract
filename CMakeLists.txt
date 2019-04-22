#CMake 最低版本号要求
cmake_minimum_required (VERSION 2.8)
# 项目信息
project (webrtc)

include_directories(./)
SET(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_LIST_DIR}/../lib)

# 指定生成目标
add_executable(main main.cpp)

#   agc_simulation库
#aux_source_directory(./modules/test/agc_simulation AGC_SIMULATION_SRCS)
#ADD_LIBRARY(agc_simulation ${AGC_SIMULATION_SRCS})
#link_directories(${CMAKE_CURRENT_LIST_DIR}/../lib)
#target_link_libraries(agc_simulation ns fft4g signal_processing rtc_base test_utils api_audio common_audio audio_butter splitting_filter three_band_filter_bank resampler memory agc_legacy agc2 system_wrappers audio_logging rnn_vad audio_utility rnnoise)

SET(CMAKE_CXX_COMPILER      "gcc" )
SET(CMAKE_CXX_FLAGS   "-std=c++11")

