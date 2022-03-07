#ifndef AGORA_AUDIO_EXTERNAL_RESAMPLER_H_
#define AGORA_AUDIO_EXTERNAL_RESAMPLER_H_

class external_resampler {

public:
  external_resampler();
  ~external_resampler();

  int do_resample(short* in,
                  int in_samples,
                  int in_channels,
                  int in_samplerate,
                  short* out,
                  int out_samples,
                  int out_channels,
                  int out_samplerate);

private:
  void* resampler = nullptr;
};

#endif
