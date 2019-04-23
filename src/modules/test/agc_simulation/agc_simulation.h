#ifndef AGC_SIMULATION_H_
#define AGC_SIMULATION_H_

#include "gain_control_impl_test.h"
#include "gain_controller2_test.h"
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include "../../audio_processing/test/test_utils.h"
#include "../../../common_audio/channel_buffer.h"
#include "../../../common_audio/wav_file.h"
#include "../../../common_audio/wav_header.h"
#include "../../../common_audio/include/audio_util.h"
#include "../../audio_processing/audio_buffer.h"
#include "../define.h"

using namespace webrtc;
using namespace std;

const int kChunkSizeMs = 10;
const int kSampleRate8kHz = 8000;
const int kSampleRate16kHz = 16000;
const int kSampleRate32kHz = 32000;
const int kSampleRate48kHz = 48000;

void implement_agc(struct AGCInput *agc_input)
{
    //    输入wav音频文件
    std::unique_ptr<WavReader> in_file(new WavReader(agc_input->input_file));
    int input_sample_rate_hz = in_file->sample_rate();
    int input_num_channels = in_file->num_channels();
    int input_samples_num = in_file->num_samples();

    //    输出wav音频文件
    std::unique_ptr<WavWriter> out_file(new WavWriter(agc_input->output_file,input_sample_rate_hz,input_num_channels));
    std::unique_ptr<ChannelBufferWavReader> buffer_reader_;
    buffer_reader_.reset(new ChannelBufferWavReader(std::move(in_file)));
    

    std::unique_ptr<ChannelBuffer<float>> in_buf_;
    int kChunksPerSecond = 1000 / 10;    //  每秒帧数
    in_buf_.reset(new ChannelBuffer<float>(input_sample_rate_hz / kChunksPerSecond,input_num_channels));

    std::unique_ptr<ChannelBufferWavWriter> buffer_writer_;
    buffer_writer_.reset(new ChannelBufferWavWriter(std::move(out_file)));
    std::unique_ptr<ChannelBuffer<float>> out_buf_;
    out_buf_.reset(new ChannelBuffer<float>(input_sample_rate_hz / kChunksPerSecond,input_num_channels));

    std::unique_ptr<GainControlImplTest> gain_control;
    gain_control.reset(new GainControlImplTest);

    GainControl* gc = gain_control.get();
    gain_control->Initialize(input_num_channels,input_sample_rate_hz);

    gc->Enable(true);

    gc->set_mode(GainControl::kAdaptiveDigital);
    //gc-set_mode(GainControl::kAdaptiveAnalog)

    gc->enable_limiter(1);

    //gc->Configure();
    /*
    gc->set_mode(GainControl::kFixedDigital);
    gc->set_compression_gain_db(40);
    gc->enable_limiter(1);
    gc->set_target_level_dbfs(1);
    */
    //gc->Configure();

    //cout << "开启：" << gain_control->is_enabled() << endl;
    //cout << "模式：" << gain_control->mode() << endl;
    //cout << "流模拟水平：" << gain_control->stream_analog_level() << endl;

    /*
    cout << "压缩增益：" << gain_control->compression_gain_db() << endl;
    cout << "限制器是否开启：" << gain_control->is_limiter_enabled() << endl;
    cout << "音频流模拟水平：" << gain_control->stream_analog_level() << endl;
    cout << "目标水平dbf:" << gc->target_level_dbfs() << endl;
    */

    GainController2 gc2_config;
    gc2_config.enabled=true;


    gc2_config.adaptive_digital_mode = true;

    std::unique_ptr<GainController2Test> gain_control2;
    gain_control2.reset(new GainController2Test);
    
    if(gc2_config.enabled){
        gain_control2->ApplyConfig(gc2_config);
        gain_control2->Initialize(input_sample_rate_hz);
    }

    StreamConfig sc(input_sample_rate_hz,input_num_channels);
    AudioBuffer ab(input_sample_rate_hz / kChunksPerSecond,input_num_channels,input_sample_rate_hz / kChunksPerSecond,input_num_channels,input_sample_rate_hz / kChunksPerSecond);

    bool samples_left_to_process = true;
    int count = 0;
    while(samples_left_to_process){ 
        samples_left_to_process = buffer_reader_->Read(in_buf_.get());
        ab.CopyFrom(in_buf_->channels(),sc);

        if(input_sample_rate_hz > kSampleRate16kHz)
            ab.SplitIntoFrequencyBands();

        gain_control->AnalyzeCaptureAudio(&ab);
        gain_control->ProcessCaptureAudio(&ab, false);

        if(gc2_config.enabled){
            gain_control2->NotifyAnalogLevel(gain_control->stream_analog_level());
            gain_control2->Process(&ab);
        }

        if(input_sample_rate_hz > kSampleRate16kHz)
            ab.MergeFrequencyBands();

        ab.CopyTo(sc,out_buf_->channels());
        buffer_writer_->Write(*out_buf_);
        count ++;
    }
}

#endif  // AGC_SIMULATION_H_