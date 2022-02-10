#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <vector>

#include "wavefilehelper.hpp"

# define M_PI           3.14159265358979323846


bool read_notes(std::ifstream &file, std::vector<double>& song_notes) {
	// reads text in file, inputs into string variable, converts into
	// double type variable then enters into array until loop ends

	std::string line;
	if (file.is_open())
    {
        while (std::getline (file, line))
        {
            song_notes.push_back(std::stod(line)); // frequency
        }
		return true;
    }
   return false;
}

// Mimics a piano's sound envelope
void create_sound_envelope(std::vector <double> *adsr, int sample_rate, int duration) {
	//values for the amplitude equations
	int maxA = 20000; // maximum amplitude
	const double attack = sample_rate * duration * 0.01;
	const double decay = sample_rate * duration * 0.1;
	const double sustain = sample_rate * duration * 0.62;
	const double release = sample_rate * duration * 0.25;

	for (int i_rate = 0 ; i_rate < sample_rate*duration ; i_rate++){
	/*ATTACK*/  if (i_rate <= attack){
			adsr->push_back(maxA * (duration/attack)* i_rate);
		}
	/*DECAY*/   else if (i_rate <= (decay+attack) && i_rate > attack) {
			adsr->push_back((maxA *0.82) + pow ((maxA / (duration/decay)),
				- (i_rate - attack)));
		}
	/*SUSTAIN*/ else if (i_rate <= (sustain+decay+attack) && i_rate > (decay+attack)) {
			adsr->push_back((maxA * .5) + (3.8 *(duration/sustain))
				* pow ((i_rate - sustain), 2));
		}
	/*RELEASE*/ else if (i_rate >= (sustain+decay+attack)) {
			adsr->push_back((8 * (duration/release)) * pow ((i_rate - sample_rate), 2));
		}
	}
}

bool calculate_waveform(std::vector <double> *waveform, double timeInterval, int sample_rate,
	 int n_samples, int duration, std::vector <double> adsr, std::vector <double> song_text) {
   int note = -1; // variable used for changing frequency regularly

   int amp_rate = 0; // variable used for changing amplitude regularly

   if (adsr.size() < (sample_rate * duration) || song_text.size() < n_samples / (duration * sample_rate)) {
	   std::cout << "Invalid vector sizes" << std::endl;
	   std::cout << "Sound envelope vector size: " << adsr.size() << std::endl;
	   std::cout << "Music notes vector size: " << song_text.size() << std::endl;
	   return false;
   }

   for ( int i_sample = 0; i_sample < n_samples ; i_sample++){
	   double t = i_sample * timeInterval;

	   if (i_sample % (sample_rate * duration) == 0){
	      note++;
	      amp_rate = 0; // resets the sound envelope for the following note
     } else {
	   		amp_rate++;
		 }
	   waveform->push_back(adsr.at(amp_rate) * sin(2.0 * M_PI * (song_text.at(note)) * t));
   }
   return true;
}

int main(int argc, char *argv[]){
	//WavSound sound;
	wave_helper::WaveFileHelper helper;
	std::ifstream notesfile;

	std::vector <double> song_notes;
	notesfile.open (argv[1], std::ifstream::in);
	if (!read_notes(notesfile, song_notes)) {
		std::cout << "Could not read file."<< std::endl;
		notesfile.close();
		return -1;
	} 
	notesfile.close();

	int duration = 1; // each note's duration is 1 second
	const int sample_rate = 4140; // samples per second
	double timeInterval = 1.0/sample_rate;
	const unsigned int n_samples = duration * song_notes.size() * sample_rate;

	std::vector<double> adsr; // array for ADSR sound envelope
	create_sound_envelope(&adsr, sample_rate, duration);

	std::vector<double> waveform; // creates the array of size n_samples
	if (!calculate_waveform(&waveform, timeInterval, sample_rate, n_samples, duration,
		adsr, song_notes)){
			return -1;
		}
	wave_helper::WaveAudio wave("tone3.wav", sample_rate, &waveform, n_samples);

	if (helper.make_audio(&wave)) {
		std::cout << "Created sound wave file" << std::endl;
		return 0; 
	} else {
		std::cout << "Failed to create sound wave file" << std::endl;
		return -1;
	}
}
