/**
* @file  wavefilehelper.cpp
*
* @brief File to implement helper functions dealing with wav files.
*
* @author cucumber 2022
*/

#include "wavefilehelper.hpp"
#include <fstream>

namespace wave_helper {
  void int_to_little_endian (int number, int offset, char *data, int length) {
    if (length == 4) {
      data[offset+3]= (number >> 24) & 0xff;
      data[offset+2]= (number >> 16) & 0xff;
    }
    data[offset+1]= (number >> 8)  & 0xff;
    data[offset]=  (number) & 0xff;
  }

  void WaveFileHelper::make_audio (WaveAudio *wave) {
    bits_per_sample = 16;
    int data_length_bytes = wave->data_length * (bits_per_sample / 8);
    file_size = 44 + data_length_bytes;
    char *file_data = new char[file_size];
    // RIFF
    file_data[0] = chunk_id[0];
    file_data[1] = chunk_id[1];
    file_data[2] = chunk_id[2];
    file_data[3] = chunk_id[3];
    int_to_little_endian(file_size-8, 4, file_data, 4);
    // WAVE
    file_data[8] = format[0];
    file_data[9] = format[1];
    file_data[10] = format[2];
    file_data[11] = format[3];
    //subchunk 1
    // fmt
    file_data[12] = subchunk1_id[0];
    file_data[13] = subchunk1_id[1];
    file_data[14] = subchunk1_id[2];
    file_data[15] = subchunk1_id[3];
    int_to_little_endian(subchunk1_size, 16, file_data, 4);
    int_to_little_endian(audio_format, 20, file_data, 2);
    int_to_little_endian(num_channels, 22, file_data, 2);
    int_to_little_endian(wave->sample_rate, 24, file_data, 4);
    byte_rate = (wave->sample_rate * bits_per_sample * num_channels) / 8;
    int_to_little_endian(byte_rate, 28, file_data, 4);
    block_align = num_channels * (bits_per_sample / 8);
    int_to_little_endian(block_align, 32, file_data, 2);
    int_to_little_endian(bits_per_sample, 34, file_data, 2);
    // subchunk 2
    file_data[36] = subchunk2_id[0];
    file_data[37] = subchunk2_id[1];
    file_data[38] = subchunk2_id[2];
    file_data[39] = subchunk2_id[3];
    int_to_little_endian(wave->data_length, 4, file_data, 4);
    // wave data to little endian
    int offset = 44;
    for (int i = 0; i < wave->data_length; i++) {
      int_to_little_endian(*wave->data++, offset +i, file_data, 2);
    }

    std::ofstream output;
  	output.open (wave->file_name, std::ofstream::out | std::ofstream::binary);
  	if (output.is_open()){
  		output.write(file_data, file_size);
  		output.close();
  	}
  }
}
