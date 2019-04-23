/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_AUDIO_PROCESSING_GAIN_CONTROLLER2_Test_H_
#define MODULES_AUDIO_PROCESSING_GAIN_CONTROLLER2_Test_H_

#include <memory>
#include <string>

#include "modules/audio_processing/agc2/adaptive_agc.h"
#include "modules/audio_processing/agc2/fixed_gain_controller.h"
//#include "modules/audio_processing/include/audio_processing.h"
#include "rtc_base/constructormagic.h"

namespace webrtc {

class ApmDataDumper;
class AudioBuffer;

struct GainController2 {
  bool enabled = false;
  bool adaptive_digital_mode = true;
  float fixed_gain_db = 0.f;
};

// Gain Controller 2 aims to automatically adjust levels by acting on the
// microphone gain and/or applying digital gain.
class GainController2Test {
 public:
  GainController2Test();
  ~GainController2Test();

  void Initialize(int sample_rate_hz);
  void Process(AudioBuffer* audio);
  void NotifyAnalogLevel(int level);

  void ApplyConfig(const GainController2& config);
  static bool Validate(const GainController2& config);
  static std::string ToString(
      const GainController2& config);

 private:
  static int instance_count_;
  std::unique_ptr<ApmDataDumper> data_dumper_;
  FixedGainController fixed_gain_controller_;
  GainController2 config_;
  AdaptiveAgc adaptive_agc_;
  int analog_level_ = -1;
  bool adaptive_digital_mode_ = true;

  //RTC_DISALLOW_COPY_AND_ASSIGN(GainController2Test);
};

}  // namespace webrtc

#endif  // MODULES_AUDIO_PROCESSING_GAIN_CONTROLLER2_H_