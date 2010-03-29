/*
 * Sound.cpp
 *
 *  Created on: Mar 23, 2010
 *      Author: buzinov
 */

#include "Sound.h"
#include <stdexcept>

snd_pcm_t           *Sound::pcm_handle;
snd_pcm_hw_params_t *Sound::hwparams;

char             *Sound::SOUND_DEVICE = (char*)"default"; // Maemo sound device is named "default"
int               Sound::SAMPLE_RATE  = 44100;            // sample rate of microphone input

snd_pcm_uframes_t Sound::periodsize = 3840;               // number of samples to capture
int               Sound::frames;                          // frames count  (periodsize * periods)
signed char      *Sound::data;                            // sound data pointer
int               Sound::num_samples;                     // number of samples in data

double           *Sound::in, *Sound::out;
fftw_complex     *Sound::out_complex;
fftw_plan         Sound::p;

int               Sound::sensitivity;
int               Sound::range;

Sound::Sound()
{
	snd_pcm_stream_t stream = SND_PCM_STREAM_CAPTURE; // record sound from mic
	char *pcm_name = strdup(SOUND_DEVICE);

	/* Allocate the snd_pcm_hw_params_t structure on the stack. */
	snd_pcm_hw_params_alloca(&hwparams);

	/* Open PCM. The last parameter of this function is the mode. */
	/* If this is set to 0, the standard mode is used. Possible   */
	/* other values are SND_PCM_NONBLOCK and SND_PCM_ASYNC.       */
	/* If SND_PCM_NONBLOCK is used, read / write access to the    */
	/* PCM device will return immediately. If SND_PCM_ASYNC is    */
	/* specified, SIGIO will be emitted whenever a period has     */
	/* been completely processed by the soundcard.                */
	if (snd_pcm_open(&pcm_handle, pcm_name, stream, SND_PCM_NONBLOCK) < 0) {
		fprintf(stderr, "Error opening PCM device %s\n", pcm_name);
		throw std::runtime_error("sound init error");
	}

	/* Init hwparams with full configuration space */
	if (snd_pcm_hw_params_any(pcm_handle, hwparams) < 0) {
		fprintf(stderr, "Can not configure this PCM device.\n");
		throw std::runtime_error("sound init error");
	}

	unsigned int rate = SAMPLE_RATE; /* Sample rate */
	unsigned int exact_rate; /* Sample rate returned by */
	/* snd_pcm_hw_params_set_rate_near */
	//int dir;          /* exact_rate == rate --> dir = 0 */
	/* exact_rate < rate  --> dir = -1 */
	/* exact_rate > rate  --> dir = 1 */
	int periods = 2; /* Number of periods */

	/* Set access type. This can be either    */
	/* SND_PCM_ACCESS_RW_INTERLEAVED or       */
	/* SND_PCM_ACCESS_RW_NONINTERLEAVED.      */
	/* There are also access types for MMAPed */
	/* access, but this is beyond the scope   */
	/* of this introduction.                  */
	if (snd_pcm_hw_params_set_access(pcm_handle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED) < 0) {
		fprintf(stderr, "Error setting access.\n");
		throw std::runtime_error("sound init error");
	}

	/* Set sample format */
	if (snd_pcm_hw_params_set_format(pcm_handle, hwparams, SND_PCM_FORMAT_S16_LE) < 0) {
		fprintf(stderr, "Error setting format.\n");
		throw std::runtime_error("sound init error");
	}

	/* Set sample rate. If the exact rate is not supported */
	/* by the hardware, use nearest possible rate.         */
	exact_rate = rate;
	if (snd_pcm_hw_params_set_rate_near(pcm_handle, hwparams, &exact_rate, 0) < 0) {
		fprintf(stderr, "Error setting rate.\n");
		throw std::runtime_error("sound init error");
	}
	if (rate != exact_rate) {
		fprintf(stderr, "The rate %d Hz is not supported by your hardware.\n"
                        "==> Using %d Hz instead.\n", rate, exact_rate);
	}

	/* Set number of channels */
	if (snd_pcm_hw_params_set_channels(pcm_handle, hwparams, 2) < 0) {
	  fprintf(stderr, "Error setting channels.\n");
      throw std::runtime_error("sound init error");
	}

#if 0
	/* Set number of periods. Periods used to be called fragments. */
	if (snd_pcm_hw_params_set_periods(pcm_handle, hwparams, periods, 0) < 0) {
		fprintf(stderr, "Error setting periods.\n");
		return (-1);
	}
#endif

	/* Set buffer size (in frames). The resulting latency is given by */
	/* latency = periodsize * periods / (rate * bytes_per_frame)     */
	if (snd_pcm_hw_params_set_buffer_size(pcm_handle, hwparams, (periodsize * periods)>>2) < 0) {
	  fprintf(stderr, "Error setting buffersize.\n");
	  throw std::runtime_error("sound init error");
	}

	/* Apply HW parameter settings to */
	/* PCM device and prepare device  */
	if (snd_pcm_hw_params(pcm_handle, hwparams) < 0) {
		fprintf(stderr, "Error setting HW params.\n");
		throw std::runtime_error("sound init error");
	}

	frames = periodsize >> 2;

	data = new signed char[65536];

  in          = (double*) fftw_malloc(sizeof(double) * Sound::periodsize);
  out         = (double*) fftw_malloc(sizeof(double) * Sound::periodsize);
  out_complex = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * Sound::periodsize);

  p           = fftw_plan_dft_r2c_1d(Sound::frames, in, out_complex, FFTW_MEASURE);

}

void Sound::prepare() {
	// read audio data from mic
	int avail = snd_pcm_avail(Sound::pcm_handle);
	num_samples = avail > 0 ? (avail > 65536 ? 65536 : avail) : Sound::frames;

	int pcmreturn;
	while ((pcmreturn = snd_pcm_readi(pcm_handle, data, num_samples)) < 0) {
		snd_pcm_prepare(Sound::pcm_handle); // buffer overrun
	}

  // merge channels (the input is set to stereo)
  for (snd_pcm_uframes_t i = 0; i < Sound::periodsize; i++) {
    in[i] = ((short*) Sound::data)[i * 2] + ((short*) Sound::data)[i * 2 + 1];
  }

#if 0
  // window function
  for (snd_pcm_uframes_t i = 0; i < periodsize; i++) {
    in[i] = 0.5 * (1 - cos(2 * M_PI * i / (frames - 1))) * in[i];
  }
#endif

  // perform fft
  fftw_execute(p);

  // convert complex to double and normalize
  for (snd_pcm_uframes_t i = 0; i < Sound::periodsize; i++) {
    out[i] = sqrt(out_complex[i][0] * out_complex[i][0]
                + out_complex[i][1] * out_complex[i][1]) / 64 / (sensitivity*8 + 1);
  }
}

void Sound::setSensitivity(int level) {
  sensitivity = level;
}

void Sound::setRange(int range) {
  Sound::range = range;
}

Sound::~Sound() {
	/* Stop PCM device and drop pending frames */
	snd_pcm_drop(pcm_handle);

	/* Stop PCM device after pending frames have been played */
	snd_pcm_drain(pcm_handle);

	fftw_free(in);
	fftw_free(out);
	fftw_free(out_complex);

	delete[] data;
}
