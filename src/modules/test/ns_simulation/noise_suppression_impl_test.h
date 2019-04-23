/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_AUDIO_PROCESSING_NOISE_SUPPRESSION_IMPL_TEST_H_
#define MODULES_AUDIO_PROCESSING_NOISE_SUPPRESSION_IMPL_TEST_H_

#include <memory>
#include <vector>

//#include "../audio_processing/include/audio_processing.h"
#include "../../../rtc_base/constructormagic.h"
//#include "rtc_base/criticalsection.h"
#include "../../audio_processing/audio_buffer.h"
namespace webrtc {

class NoiseSuppression {
 public:
  virtual int Enable(bool enable) = 0;
  virtual bool is_enabled() const = 0;

  // Determines the aggressiveness of the suppression. Increasing the level
  // will reduce the noise level at the expense of a higher speech distortion.
  enum Level { kLow, kModerate, kHigh, kVeryHigh };

  virtual int set_level(Level level) = 0;
  virtual Level level() const = 0;

  // Returns the internally computed prior speech probability of current frame
  // averaged over output channels. This is not supported in fixed point, for
  // which |kUnsupportedFunctionError| is returned.
  virtual float speech_probability() const = 0;

  // Returns the noise estimate per frequency bin averaged over all channels.
  virtual std::vector<float> NoiseEstimate() = 0;

 protected:
  virtual ~NoiseSuppression() {}
};

class AudioBuffer;

class NoiseSuppressionImplTest : public NoiseSuppression {
 public:
  NoiseSuppressionImplTest() = default;
  ~NoiseSuppressionImplTest() = default;

  // TODO(peah): Fold into ctor, once public API is removed.
  void Initialize(size_t channels, int sample_rate_hz);
  void AnalyzeCaptureAudio(AudioBuffer* audio);
  void ProcessCaptureAudio(AudioBuffer* audio);

  // NoiseSuppression implementation.
  int Enable(bool enable) override;
  bool is_enabled() const override;
  int set_level(Level level) override;
  Level level() const override;
  float speech_probability() const override;
  std::vector<float> NoiseEstimate() override;
  static size_t num_noise_bins();

 private:
  class Suppressor;
  bool enabled_ = false;
  Level level_ = kModerate;
  size_t channels_ = 0;
  int sample_rate_hz_ = 0;
  std::vector<Suppressor*> suppressors_;
};
}  // namespace webrtc

#endif  // MODULES_AUDIO_PROCESSING_NOISE_SUPPRESSION_IMPL_H_
