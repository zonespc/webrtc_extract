#include "ns_simulation.h"
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include "../../audio_processing/ns/noise_suppression.h"
#include "../../audio_processing/test/test_utils.h"
#include "../../../common_audio/channel_buffer.h"
#include "../../../common_audio/wav_file.h"
#include "../../../common_audio/wav_header.h"
#include "../../../common_audio/include/audio_util.h"
#include "../../audio_processing/audio_buffer.h"
#include "noise_suppression_impl_test.h"
#include "../define.h"

using namespace webrtc;
using namespace std;

const int kChunkSizeMs = 10;
const int kSampleRate8kHz = 8000;
const int kSampleRate16kHz = 16000;
const int kSampleRate32kHz = 32000;
const int kSampleRate48kHz = 48000;

void CopyToAudioFrame(const ChannelBuffer<float>& src, AudioFrame* dest) {
  RTC_CHECK_EQ(src.num_channels(), dest->num_channels_);
  RTC_CHECK_EQ(src.num_frames(), dest->samples_per_channel_);
  int16_t* dest_data = dest->mutable_data();
  for (size_t ch = 0; ch < dest->num_channels_; ++ch) {
    for (size_t sample = 0; sample < dest->samples_per_channel_; ++sample) {
      dest_data[sample * dest->num_channels_ + ch] =
          src.channels()[ch][sample] * 32767;
    }
  }
}

struct ApmCaptureState {
    ApmCaptureState(bool transient_suppressor_enabled);
    ~ApmCaptureState();
    int aec_system_delay_jumps;
    int delay_offset_ms;
    bool was_stream_delay_set;
    int last_stream_delay_ms;
    int last_aec_system_delay_ms;
    int stream_delay_jumps;
    bool output_will_be_muted;
    bool key_pressed;
    bool transient_suppressor_enabled;
    std::unique_ptr<AudioBuffer> capture_audio;
    // Only the rate and samples fields of capture_processing_format_ are used
    // because the capture processing number of channels is mutable and is
    // tracked by the capture_audio_.
    StreamConfig capture_processing_format;
    int split_rate;
    bool echo_path_gain_change;
    int prev_analog_mic_level;
    float prev_pre_amp_gain;
};

ApmCaptureState::ApmCaptureState(
    bool transient_suppressor_enabled)
    : aec_system_delay_jumps(-1),
      delay_offset_ms(0),
      was_stream_delay_set(false),
      last_stream_delay_ms(0),
      last_aec_system_delay_ms(0),
      stream_delay_jumps(-1),
      output_will_be_muted(false),
      key_pressed(false),
      transient_suppressor_enabled(transient_suppressor_enabled),
      capture_processing_format(kSampleRate16kHz),
      split_rate(kSampleRate16kHz),
      echo_path_gain_change(false),
      prev_analog_mic_level(-1),
      prev_pre_amp_gain(-1.f) {}

ApmCaptureState::~ApmCaptureState() = default;

void implement_ns(struct NsInput *ns_input)
{
    //    输入wav音频文件
    std::unique_ptr<WavReader> in_file(new WavReader(ns_input->input_file));
    int input_sample_rate_hz = in_file->sample_rate();
    int input_num_channels = in_file->num_channels();
    //int input_samples_num = in_file->num_samples();
    //cout << "采样率:" << input_sample_rate_hz << ", 通道数:" << input_num_channels << ", 采样点数:" << input_samples_num << endl;

    //    输出wav音频文件
    std::unique_ptr<WavWriter> out_file(new WavWriter(ns_input->output_file,input_sample_rate_hz,input_num_channels));
    std::unique_ptr<ChannelBufferWavReader> buffer_reader_;
    buffer_reader_.reset(new ChannelBufferWavReader(std::move(in_file)));


    std::unique_ptr<ChannelBuffer<float>> in_buf_;
    int kChunksPerSecond = 1000 / 10;    //  每秒帧数
    in_buf_.reset(new ChannelBuffer<float>(input_sample_rate_hz / kChunksPerSecond,input_num_channels));

    std::unique_ptr<ChannelBufferWavWriter> buffer_writer_;
    buffer_writer_.reset(new ChannelBufferWavWriter(std::move(out_file)));
    std::unique_ptr<ChannelBuffer<float>> out_buf_;
    out_buf_.reset(new ChannelBuffer<float>(input_sample_rate_hz / kChunksPerSecond,input_num_channels));

    NoiseSuppressionImplTest noise_suppressor;
    noise_suppressor.Initialize(input_num_channels,input_sample_rate_hz);
    noise_suppressor.Enable(true);
    NoiseSuppression::Level level;
    level = NoiseSuppression::kLow;
    level = NoiseSuppression::Level(ns_input->mode);
    noise_suppressor.set_level(level);


    StreamConfig sc(input_sample_rate_hz,input_num_channels);
    AudioBuffer ab(input_sample_rate_hz / kChunksPerSecond,input_num_channels,input_sample_rate_hz / kChunksPerSecond,input_num_channels,input_sample_rate_hz / kChunksPerSecond);

    bool samples_left_to_process = true;
    int count = 0;
    while(samples_left_to_process){
        samples_left_to_process = buffer_reader_->Read(in_buf_.get());
        ab.CopyFrom(in_buf_->channels(),sc);

        if(input_sample_rate_hz > kSampleRate16kHz)
            ab.SplitIntoFrequencyBands();

        noise_suppressor.AnalyzeCaptureAudio(&ab);
        noise_suppressor.ProcessCaptureAudio(&ab);

        if(input_sample_rate_hz > kSampleRate16kHz)
            ab.MergeFrequencyBands();

        ab.CopyTo(sc,out_buf_->channels());
        buffer_writer_->Write(*out_buf_);
        count ++;
    }
    //cout << "总帧数:"<< count << "." << endl;
    //cout << "降噪处理结束!" << endl;
}
