#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <math.h>
//#include "wav.hpp" // helper header file from ECS
#include <string>
#include "wavefilehelper.hpp"

int count_lines(std::ifstream &file) {
	// counts the number of lines in the text file
	std::string line;
	int count = 0;
	if (file.is_open() ) {
		while (! file.eof () ) {
			std::getline (file, line);
			count++;
		}
	}
	file.close();

	return count;
}

void read_notes(std::ifstream &file, double song_text[]) {
	// reads text in file, inputs into string variable, converts into
	// double type variable then enters into array until loop ends
	int loop = 0;

	std::string line;
	if (file.is_open())
    {
        while (! file.eof() )
        {
            std::getline (file, line);
            std::string music_note = line;
            song_text[loop] = atof (music_note.c_str()); // frequency
            loop++;
        }
    }
   file.close();
}

// Mimics a piano's sound envelope
void create_sound_envelope(double adsr[], int sample_rate, int duration) {
	//values for the amplitude equations
	int maxA = 20000; // maximum amplitude
	const double attack = sample_rate * duration * 0.01;
	const double decay = sample_rate * duration * 0.1;
	const double sustain = sample_rate * duration * 0.62;
	const double release = sample_rate * duration * 0.25;

	for (int i_rate = 0 ; i_rate < sample_rate*duration ; i_rate++){
	/*ATTACK*/  if (i_rate <= attack){
			adsr[i_rate] = maxA * (duration/attack)* i_rate;
		}
	/*DECAY*/   if (i_rate <= (decay+attack) && i_rate > attack) {
			adsr[i_rate] = (maxA *0.82) + pow ((maxA / (duration/decay)),
				- (i_rate - attack));
		}
	/*SUSTAIN*/ if (i_rate <= (sustain+decay+attack) && i_rate > (decay+attack)) {
			adsr[i_rate] = (maxA * .5) + (3.8 *(duration/sustain))
				* pow ((i_rate - sustain), 2);
		}
	/*RELEASE*/ if (i_rate >= (sustain+decay+attack)) {
			adsr[i_rate] = (8 * (duration/release)) * pow ((i_rate - sample_rate), 2);
		}
	}
}

void calculate_waveform(int waveform[], double timeInterval, int sample_rate,
	 int n_samples, int duration, double adsr[], double song_text[]) {
   int T = 0; // variable used for changing frequency regularly

   int amp_rate = 0; // variable used for changing amplitude regularly

   for ( int i_sample = 0; i_sample < n_samples ; i_sample++){
	   double t = i_sample * timeInterval;

	   if (i_sample % (sample_rate * duration)== 0){
	      T++;
	      amp_rate = 0; // resets the sound envelope for the following note
     } else {
	   		amp_rate++;
		 }

	   waveform[i_sample] = adsr[amp_rate] * sin(2.0 * M_PI * (song_text[T]) * t);
   }
}

int main(int argc, char *argv[]){
	//WavSound sound;
	wave_helper::WaveFileHelper helper;
	std::ifstream notesfile (argv[1]);
	int notes = count_lines(notesfile);

	double song_text[notes];
	notesfile.open (argv[1]);
	read_notes(notesfile, song_text);

	int duration = 1; // each note's duration is 1 second
	const int sample_rate = 4140; // samples per second
	double timeInterval = 1.0/sample_rate;
	unsigned int n_samples = duration * notes * sample_rate;

	double A[sample_rate]; // array for ADSR sound envelope
	create_sound_envelope(A, sample_rate, duration);

	int waveform[n_samples]; // creates the array of size n_samples
	calculate_waveform(waveform, timeInterval, sample_rate, n_samples, duration,
		A, song_text);

	//sound.MakeWavFromInt("tone.wav", sample_rate, waveform, n_samples);
	wave_helper::WaveAudio wave("tone2.wav", sample_rate, waveform, n_samples);
	//wave_helper::WaveAudio wave = {"tone2.wav", sample_rate, waveform, n_samples};
	helper.make_audio(&wave);

	return 0;
}
