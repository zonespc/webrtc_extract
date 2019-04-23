/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "noise_suppression_impl_test.h"

#include "../../audio_processing/audio_buffer.h"
#include "../../../rtc_base/constructormagic.h"

#include "modules/audio_processing/ns/noise_suppression.h"
#define NS_CREATE WebRtcNs_Create
#define NS_FREE WebRtcNs_Free
#define NS_INIT WebRtcNs_Init
#define NS_SET_POLICY WebRtcNs_set_policy
typedef NsHandle NsState;


namespace webrtc {

  enum Error {
    // Fatal errors.
    kNoError = 0,
    kUnspecifiedError = -1,
    kCreationFailedError = -2,
    kUnsupportedComponentError = -3,
    kUnsupportedFunctionError = -4,
    kNullPointerError = -5,
    kBadParameterError = -6,
    kBadSampleRateError = -7,
    kBadDataLengthError = -8,
    kBadNumberChannelsError = -9,
    kFileError = -10,
    kStreamParameterNotSetError = -11,
    kNotEnabledError = -12,

    // Warnings are non-fatal.
    // This results when a set_stream_ parameter is out of range. Processing
    // will continue, but the parameter may have been truncated.
    kBadStreamParameterWarning = -13
  };




class NoiseSuppressionImplTest::Suppressor {
 public:
  explicit Suppressor(int sample_rate_hz) {
    state_ = NS_CREATE();
    NS_INIT(state_, sample_rate_hz);

  }
  ~Suppressor() { NS_FREE(state_); }
  NsState* state() { return state_; }

 private:
  NsState* state_ = nullptr;
};


void NoiseSuppressionImplTest::Initialize(size_t channels, int sample_rate_hz) {
  channels_ = channels;
  sample_rate_hz_ = sample_rate_hz;
  std::vector<Suppressor*> new_suppressors;
  if (enabled_) {
    new_suppressors.resize(channels);
    for (size_t i = 0; i < channels; i++) {
      new_suppressors[i] = new Suppressor(sample_rate_hz);
    }
  }
  suppressors_.swap(new_suppressors);
  set_level(level_);
}

void NoiseSuppressionImplTest::AnalyzeCaptureAudio(AudioBuffer* audio) {
  if (!enabled_) {
    return;
  }
  for (size_t i = 0; i < suppressors_.size(); i++) {
    WebRtcNs_Analyze(suppressors_[i]->state(),
                     audio->split_bands_const_f(i)[kBand0To8kHz]);
  }
}

void NoiseSuppressionImplTest::ProcessCaptureAudio(AudioBuffer* audio) {
  if (!enabled_) {
    return;
  }
  for (size_t i = 0; i < suppressors_.size(); i++) {
    WebRtcNs_Process(suppressors_[i]->state(), audio->split_bands_const_f(i),
                     audio->num_bands(), audio->split_bands_f(i));
  }
}

int NoiseSuppressionImplTest::Enable(bool enable) {
  if (enabled_ != enable) {
    enabled_ = enable;
    Initialize(channels_, sample_rate_hz_);
  }
  return kNoError;
}

bool NoiseSuppressionImplTest::is_enabled() const {
  return enabled_;
}

int NoiseSuppressionImplTest::set_level(Level level) {
  int policy = 1;
  switch (level) {
    case NoiseSuppression::kLow:
      policy = 0;
      break;
    case NoiseSuppression::kModerate:
      policy = 1;
      break;
    case NoiseSuppression::kHigh:
      policy = 2;
      break;
    case NoiseSuppression::kVeryHigh:
      policy = 3;
      break;
  }
  level_ = level;
  for (auto& suppressor : suppressors_) {
    NS_SET_POLICY(suppressor->state(), policy);
  }
  return kNoError;
}

NoiseSuppression::Level NoiseSuppressionImplTest::level() const {
  return level_;
}

float NoiseSuppressionImplTest::speech_probability() const {

  float probability_average = 0.0f;
  for (auto& suppressor : suppressors_) {
    probability_average +=
        WebRtcNs_prior_speech_probability(suppressor->state());
  }
  if (!suppressors_.empty()) {
    probability_average /= suppressors_.size();
  }
  return probability_average;
}

std::vector<float> NoiseSuppressionImplTest::NoiseEstimate() {
  std::vector<float> noise_estimate;

  const float kNumChannelsFraction = 1.f / suppressors_.size();
  noise_estimate.assign(WebRtcNs_num_freq(), 0.f);
  for (auto& suppressor : suppressors_) {
    const float* noise = WebRtcNs_noise_estimate(suppressor->state());
    for (size_t i = 0; i < noise_estimate.size(); ++i) {
      noise_estimate[i] += kNumChannelsFraction * noise[i];
    }
  }
  return noise_estimate;
}

size_t NoiseSuppressionImplTest::num_noise_bins() {
  return WebRtcNs_num_freq();
}

}  // namespace webrtc
