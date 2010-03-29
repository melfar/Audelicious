/*
 * Spectrum.cpp
 *
 *  Created on: Mar 19, 2010
 *      Author: buzinov
 */

#include "Spectrum.h"

Spectrum::Spectrum(QGLShaderProgram *program, int width, int height, const QString& background)
  : columns(1), scroller(program, width, height, 1, background) {

	bitmap = new unsigned char[columns*height*4];
	this->width  = width;
	this->height = height;

	current_range = 0;
}

Spectrum::~Spectrum() {
	delete[] bitmap;
}

void Spectrum::prepare() {
	// paint new column
	int c = 0, k = 0;
	for (int j = 0; j < columns; j++) {
		for (size_t i = 0; i < height; i++, c++) {
			GLubyte level = Sound::out[c] < 255 ? Sound::out[c] : 255;

			if (current_range == 1 && height == 480 /* :TODO: hardcoded */) {
			  float divisor = 11.0;  // 2 kHz limit
			  float step = height / divisor / 4;
			  float tmp_level = (1-k/step)*Sound::out[int(c / divisor)] + (k/step)*Sound::out[int(c / divisor) + 1];
			  level = tmp_level < 255 ? tmp_level : 255;
			  k++;
			  if (k >= step)  k = 0;
			}

			bitmap[(i*columns + j) * 4 + 0] = ColorTheme::colors()[level].r;
			bitmap[(i*columns + j) * 4 + 1] = ColorTheme::colors()[level].g;
			bitmap[(i*columns + j) * 4 + 2] = ColorTheme::colors()[level].b;
			bitmap[(i*columns + j) * 4 + 3] = 255;
		}
	}
}

void Spectrum::paint() {
  scroller.paint(bitmap);
}

void Spectrum::toggleRange(bool update_background) {
  current_range++;
  if (current_range > 1)  current_range = 0;

  if (update_background) {
    if (current_range == 0)
      scroller.background = ":/images/spectrum.jpg";
    else
      scroller.background = ":/images/spectrum1khz.jpg";
    scroller.makeTexture();
  }
}

