/*
 * Sound.h
 *
 *  Created on: Mar 23, 2010
 *      Author: buzinov
 */

#ifndef SOUND_H_
#define SOUND_H_

#include <alsa/asoundlib.h>
#include <QObject>
#include <cmath>
#include <fftw3.h>

class Sound {
public:
	Sound();
	~Sound();

	static snd_pcm_t  *pcm_handle;
	static snd_pcm_hw_params_t *hwparams;

	static char *SOUND_DEVICE;             // Maemo sound device is named "default"
	static int   SAMPLE_RATE;              // sample rate of microphone input

	static snd_pcm_uframes_t periodsize;   // number of samples to capture
	static int frames;                     // frames count  (periodsize * periods)

	static signed char *data;              // sound data pointer
	static int num_samples;                // number of samples in data

	static void prepare();

  static double       *in, *out;
  static fftw_complex *out_complex;
  static fftw_plan     p;

  static int sensitivity;
  static void setSensitivity(int level);

  static int range;
  static void setRange(int range);
};

#endif /* SOUND_H_ */
