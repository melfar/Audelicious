/*
 * Amplitude.cpp
 *
 *  Created on: Mar 24, 2010
 *      Author: buzinov
 */

#include "Amplitude.h"

Amplitude::Amplitude(QGLShaderProgram *program, int width, int height, const QString& background)
	: columns(1), scroller(program, width, height, 1, background) {

	bitmap = new unsigned char[columns*height*4];
	this->width  = width;
	this->height = height;
}

Amplitude::~Amplitude() {
	delete[] bitmap;
}
void Amplitude::prepare() {
	// find peak amplitude (* 2 for stereo)
	short peak = 0, val;
	for (snd_pcm_uframes_t i = 0; i < Sound::num_samples; i++) {
		val = ((short*) Sound::data)[i];
		val = val < 0 ? -val : val;      // abs
		if (val > peak)  peak = val;
	}

	// normalize
	peak /= (std::numeric_limits<short>::max() / height / 8);

	// paint new column
	for (size_t i = 0; i < height; i++) {
		GLubyte level = (i >= height/2 - peak/2 && i < height/2 + peak/2) ? 128 : 0;
		bitmap[i * 4 + 0] = level > 0 ? ColorTheme::colors()[level].r : 0;
		bitmap[i * 4 + 1] = level > 0 ? ColorTheme::colors()[level].g : 0;
		bitmap[i * 4 + 2] = level > 0 ? ColorTheme::colors()[level].b : 0;
		bitmap[i * 4 + 3] = 255;
	}
}

void Amplitude::paint() {
	scroller.paint(bitmap);
}
