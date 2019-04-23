#CMake 最低版本号要求
cmake_minimum_required (VERSION 2.8)
# 项目信息
project (webrtc)

include_directories(./)
SET(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_LIST_DIR}/../lib)

#   ns库
aux_source_directory(./modules/audio_processing/ns/ NS_SRCS)
add_library(ns ${NS_SRCS})

#   fft4g库
aux_source_directory(./common_audio/third_party/fft4g FFT4G_SRCS)
add_library(fft4g ${FFT4G_SRCS})

#   signal_processing库
aux_source_directory(./common_audio/signal_processing SIGNAL_PROCESSING_SRCS)
add_library(signal_processing ${SIGNAL_PROCESSING_SRCS})

#   rtc_base库
aux_source_directory(./rtc_base RTC_BASE_SRCS)
add_library(rtc_base ${RTC_BASE_SRCS})

#   test_utils库
aux_source_directory(./modules/audio_processing/test TEST_UTILS_SRCS)
add_library(test_utils ${TEST_UTILS_SRCS})

#    api_audio库
aux_source_directory(./api/audio API_AUDIO_SRCS)
add_library(api_audio ${API_AUDIO_SRCS})

#    common_audio库
aux_source_directory(./common_audio/ COMMON_AUDIO_SRCS)
add_library(common_audio ${COMMON_AUDIO_SRCS})

#    audio_processing相关库,audio_butter,splitting_filter,three_band_filter_bank
add_library(audio_butter ./modules/audio_processing/audio_buffer.cc)
add_library(splitting_filter ./modules/audio_processing/splitting_filter.cc)
add_library(three_band_filter_bank ./modules/audio_processing/three_band_filter_bank.cc)

#   resampler库
aux_source_directory(./common_audio/resampler RESAMPLER_SRCS)
add_library(resampler ${RESAMPLER_SRCS})

#    memory库
aux_source_directory(./rtc_base/memory MEMORY_SRCS)
add_library(memory ${MEMORY_SRCS})

#   agc_legacy库
aux_source_directory(./modules/audio_processing/agc/legacy AGC_LEGACY_SRCS)
add_library(agc_legacy ${AGC_LEGACY_SRCS})

#   system_wrappers库
aux_source_directory(./system_wrappers/source SYSTEM_WRAPPERS_SRCS)
add_library(system_wrappers ${SYSTEM_WRAPPERS_SRCS})

#   audio_logging库
aux_source_directory(./modules/audio_processing/logging AUDIO_LOGGING_SRCS)
add_library(audio_logging ${AUDIO_LOGGING_SRCS})

#   rnn_vad库
aux_source_directory(./modules/audio_processing/agc2/rnn_vad RNN_VAD_SRCS)
add_library(rnn_vad ${RNN_VAD_SRCS})

#   audio_utility库
aux_source_directory(./modules/audio_processing/utility AUDIO_UTILITY_SRCS)
add_library(audio_utility ${AUDIO_UTILITY_SRCS})

#   rnnoise库
aux_source_directory(./third_party/rnnoise/src RNNOISE_SRCS)
add_library(rnnoise ${RNNOISE_SRCS})

#   agc2库
aux_source_directory(./modules/audio_processing/agc2 AGC2_SRCS)
add_library(agc2 ${AGC2_SRCS})

#   noise_suppression库
aux_source_directory(./modules/test/ns_simulation NOISE_SUPPRESSION_SRCS)
ADD_LIBRARY(noise_suppression ${NOISE_SUPPRESSION_SRCS})
link_directories(${CMAKE_CURRENT_LIST_DIR}/../lib)
target_link_libraries(noise_suppression ns fft4g signal_processing rtc_base test_utils api_audio common_audio audio_butter splitting_filter three_band_filter_bank resampler memory agc_legacy agc2 system_wrappers audio_logging rnn_vad audio_utility rnnoise)

#   agc_simulation库
aux_source_directory(./modules/test/agc_simulation AGC_SIMULATION_SRCS)
ADD_LIBRARY(agc_simulation ${AGC_SIMULATION_SRCS})
link_directories(${CMAKE_CURRENT_LIST_DIR}/../lib)
target_link_libraries(agc_simulation ns fft4g signal_processing rtc_base test_utils api_audio common_audio audio_butter splitting_filter three_band_filter_bank resampler memory agc_legacy agc2 system_wrappers audio_logging rnn_vad audio_utility rnnoise)


SET(CMAKE_CXX_COMPILER      "gcc" )
SET(CMAKE_CXX_FLAGS   "-std=c++11")
#SET(CMAKE_BUILD_TYPE "Debug")
#SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
#SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")
