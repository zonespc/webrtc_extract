/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "gain_controller2_test.h"

#include "modules/audio_processing/audio_buffer.h"
#include "modules/audio_processing/include/audio_frame_view.h"
#include "modules/audio_processing/logging/apm_data_dumper.h"
#include "rtc_base/atomicops.h"
#include "rtc_base/checks.h"
#include "rtc_base/strings/string_builder.h"
#include <iostream>

namespace webrtc {

enum NativeRate {
  kSampleRate8kHz = 8000,
  kSampleRate16kHz = 16000,
  kSampleRate32kHz = 32000,
  kSampleRate48kHz = 48000
};


int GainController2Test::instance_count_ = 0;

GainController2Test::GainController2Test()
    : data_dumper_(
          new ApmDataDumper(rtc::AtomicOps::Increment(&instance_count_))),
      fixed_gain_controller_(data_dumper_.get()),
      adaptive_agc_(data_dumper_.get()) {}

GainController2Test::~GainController2Test() = default;

void GainController2Test::Initialize(int sample_rate_hz) {
  RTC_DCHECK(sample_rate_hz == kSampleRate8kHz ||
             sample_rate_hz == kSampleRate16kHz ||
             sample_rate_hz == kSampleRate32kHz ||
             sample_rate_hz == kSampleRate48kHz);
  fixed_gain_controller_.SetSampleRate(sample_rate_hz);
  data_dumper_->InitiateNewSetOfRecordings();
  data_dumper_->DumpRaw("sample_rate_hz", sample_rate_hz);
}

void GainController2Test::Process(AudioBuffer* audio) {
  AudioFrameView<float> float_frame(audio->channels_f(), audio->num_channels(),
                                    audio->num_frames());
  if (adaptive_digital_mode_) {
    //std::cout << fixed_gain_controller_.LastAudioLevel() << std::endl; 

    adaptive_agc_.Process(float_frame, fixed_gain_controller_.LastAudioLevel());
  }
  //std::cout << "固定增益" << std::endl; 
  fixed_gain_controller_.Process(float_frame);
}

void GainController2Test::NotifyAnalogLevel(int level) {
  if (analog_level_ != level && adaptive_digital_mode_) {
    adaptive_agc_.Reset();
  }
  analog_level_ = level;
}

void GainController2Test::ApplyConfig(
    const GainController2& config) {
  RTC_DCHECK(Validate(config));
  config_ = config;
  fixed_gain_controller_.SetGain(config_.fixed_gain_db);
  adaptive_digital_mode_ = config_.adaptive_digital_mode;
}

bool GainController2Test::Validate(
    const GainController2& config) {
  return config.fixed_gain_db >= 0.f;
}

std::string GainController2Test::ToString(
    const GainController2& config) {
  rtc::StringBuilder ss;
  ss << "{enabled: " << (config.enabled ? "true" : "false") << ", "
     << "fixed_gain_dB: " << config.fixed_gain_db << "}";
  return ss.Release();
}

}  // namespace webrtc
