/**
* @file  wavefilehelper.hpp
*
* @brief Header file to define helper functions dealing with wav files.
*
* @author cucumber 2022
*/

#ifndef WAVE_FILE_HELPER
#define WAVE_FILE_HELPER

namespace wavehelper {
  typedef struct AudioFileData {
    char *filename;
    unsigned int sample_rate;
    int data;
    unsigned int data_length;
  } WaveAudio;

  class WavefileHelper{
  private:
    //Fields
    char *chunk_id = "RIFF";
    int file_size;
    char *format = "WAVE";
    char *subchunk1_id = "fmt ";
    int subchunk1_size = 16;
    int audio_format = 1; // PCM
    int num_channels = 2;
    int sample_rate;
    int byte_rate;
    int block_align;
    int bits_per_sample;
    char *subchunk2_id = "data";
    int subchunk2_size; //size of data
    
  public:
    void make_audio (WaveAudio wave);

    WaveAudio audio_reader ();
  }
}

#endif
