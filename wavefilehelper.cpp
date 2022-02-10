/**
* @file  wavefilehelper.cpp
*
* @brief File to implement helper functions dealing with wav files.
*
* @author cucumber 2022
*/

#include "wavefilehelper.hpp"
#include <fstream>
#include <iostream>
#include <iterator>
#include <vector>

namespace wave_helper {
  void int_to_little_endian (int number, std::vector<char> *data, int length) {
    data->push_back((number) & 0xff);
    data->push_back((number >> 8) & 0xff);
    if (length == 4) {
      data->push_back((number >> 16)  & 0xff);
      data->push_back((number >> 24) & 0xff); 
    }
  }

  bool WaveFileHelper::make_audio (WaveAudio *wave) {
    bits_per_sample = 16;
    int data_length_bytes = wave->data_length * (bits_per_sample / 8);
    file_size = 44 + data_length_bytes;
    
    std::vector<char> file_data;

    // RIFF
    file_data.push_back(chunk_id[0]);
    file_data.push_back(chunk_id[1]);
    file_data.push_back(chunk_id[2]);
    file_data.push_back(chunk_id[3]);
    int_to_little_endian(file_size-8, &file_data, 4);
    // WAVE
    file_data.push_back(format[0]);
    file_data.push_back(format[1]);
    file_data.push_back(format[2]);
    file_data.push_back(format[3]);
    //subchunk 1
    // fmt
    file_data.push_back(subchunk1_id[0]);
    file_data.push_back(subchunk1_id[1]);
    file_data.push_back(subchunk1_id[2]);
    file_data.push_back(subchunk1_id[3]);

    int_to_little_endian(subchunk1_size, &file_data, 4);
    int_to_little_endian(audio_format, &file_data, 2);
    int_to_little_endian(num_channels, &file_data, 2);
    int_to_little_endian(wave->sample_rate, &file_data, 4);

    byte_rate = (wave->sample_rate * bits_per_sample * num_channels) / 8;
    int_to_little_endian(byte_rate, &file_data, 4);
    block_align = num_channels * (bits_per_sample / 8);
    int_to_little_endian(block_align,  &file_data, 2);
    int_to_little_endian(bits_per_sample, &file_data, 2);
    // subchunk 2
    file_data.push_back(subchunk2_id[0]);
    file_data.push_back(subchunk2_id[1]);
    file_data.push_back(subchunk2_id[2]);
    file_data.push_back(subchunk2_id[3]);
    
    int_to_little_endian(wave->data_length, &file_data, 4);
    // wave data to little endianss
    for (int i = 0; i < wave->data_length; i++) {
      int_to_little_endian(wave->data->at(i), &file_data, 2);
    }

    std::ofstream output;
  	output.open (wave->file_name, std::ofstream::out | std::ofstream::binary);
    if (!output) {
      std::cout << "Cannot open file" << std::endl;
      return false;
    }
  	if (output.is_open()){
      for (int i = 0; i < file_data.size(); i++) {
        output.write(&file_data.at(i), sizeof(char));
      }
  		output.close();
  	}
    return true; 
  }
}
