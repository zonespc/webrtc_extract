/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_AUDIO_PROCESSING_GAIN_CONTROL_IMPL_TEST_H_
#define MODULES_AUDIO_PROCESSING_GAIN_CONTROL_IMPL_TEST_H_

#include <memory>
#include <vector>

//#include "modules/audio_processing/include/audio_processing.h"
#include "modules/audio_processing/render_queue_item_verifier.h"
#include "modules/audio_processing/include/gain_control.h"
#include "rtc_base/constructormagic.h"
//#include "rtc_base/criticalsection.h"
#include "rtc_base/swap_queue.h"
#include "rtc_base/thread_annotations.h"
#include "api/array_view.h"

namespace webrtc {

class ApmDataDumper;
class AudioBuffer;

class GainControlImplTest : public GainControl {
 public:
  GainControlImplTest();
  ~GainControlImplTest() override;

  void ProcessRenderAudio(rtc::ArrayView<const int16_t> packed_render_audio);
  int AnalyzeCaptureAudio(AudioBuffer* audio);
  int ProcessCaptureAudio(AudioBuffer* audio, bool stream_has_echo);

  void Initialize(size_t num_proc_channels, int sample_rate_hz);

  static void PackRenderAudioBuffer(AudioBuffer* audio,
                                    std::vector<int16_t>* packed_buffer);

  // GainControl implementation.
  bool is_enabled() const override;
  int stream_analog_level() override;
  bool is_limiter_enabled() const override;
  Mode mode() const override;

  int compression_gain_db() const override;

 private:
  class GainController;

  // GainControl implementation.
  int Enable(bool enable) override;
  int set_stream_analog_level(int level) override;
  int set_mode(Mode mode) override;
  int set_target_level_dbfs(int level) override;
  int target_level_dbfs() const override;
  int set_compression_gain_db(int gain) override;
  int enable_limiter(bool enable) override;
  int set_analog_level_limits(int minimum, int maximum) override;
  int analog_level_minimum() const override;
  int analog_level_maximum() const override;
  bool stream_is_saturated() const override;

  int Configure();

  //rtc::CriticalSection* const crit_render_ RTC_ACQUIRED_BEFORE(crit_capture_);
  //rtc::CriticalSection* const crit_capture_;

  //std::unique_ptr<ApmDataDumper> data_dumper_;

  bool enabled_ = false;

  Mode mode_;
  int minimum_capture_level_;
  int maximum_capture_level_;
  bool limiter_enabled_;
  int target_level_dbfs_;
  int compression_gain_db_;
  int analog_capture_level_;
  bool was_analog_level_set_;
  bool stream_is_saturated_;

  std::vector<std::unique_ptr<GainController>> gain_controllers_;

  size_t num_proc_channels_;
  int sample_rate_hz_;

  static int instance_counter_;
  //RTC_DISALLOW_IMPLICIT_CONSTRUCTORS(GainControlImplTest);
};
}  // namespace webrtc

#endif  // MODULES_AUDIO_PROCESSING_GAIN_CONTROL_IMPL_H_
