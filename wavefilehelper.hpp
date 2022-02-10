/**
* @file  wavefilehelper.hpp
*
* @brief Header file to define helper functions dealing with wav files.
*
* @author cucumber 2022
*/

#ifndef WAVE_FILE_HELPER
#define WAVE_FILE_HELPER
#include <string>
#include <vector>

namespace wave_helper {
  struct WaveAudio {
    std::string file_name;
    unsigned int sample_rate;
    std::vector<double> *data; 
    unsigned int data_length;
    WaveAudio(std::string file_name, unsigned int sample_rate, std::vector<double> *data,
      unsigned int data_length):
      file_name(file_name),
      sample_rate(sample_rate),
      data(data),
      data_length(data_length) {}
  };

  class WaveFileHelper{
    private:
      //Fields
      std::string chunk_id = "RIFF";
      int file_size;
      std::string format = "WAVE";
      std::string subchunk1_id = "fmt ";
      int subchunk1_size = 16;
      int audio_format = 1; // PCM
      int num_channels = 1;
      int sample_rate;
      int byte_rate;
      int block_align;
      int bits_per_sample;
      std::string subchunk2_id = "data";
      int subchunk2_size; //size of data

    public:
      bool make_audio (WaveAudio *wave);

      WaveAudio audio_reader ();
  };
}

#endif
