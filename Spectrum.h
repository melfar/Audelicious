/*
 * Spectrum.h
 *
 *  Created on: Mar 19, 2010
 *      Author: buzinov
 */

#ifndef SPECTRUM_H_
#define SPECTRUM_H_

#include "Scroller2D.h"
#include "Sound.h"
#include <cmath>
#include <fftw3.h>
#include "ColorTheme.h"

class Spectrum {
public:
	Spectrum(QGLShaderProgram *program, int width, int height, const QString& background = "");
	~Spectrum();

	void prepare();
	void paint();
	void toggleRange(bool update_background);

private:
	Scroller2D scroller;

	double       *in, *out;
	fftw_complex *out_complex;
	fftw_plan     p;

	GLubyte *bitmap;
	size_t width, height, columns;

	int current_range;
};

#endif /* SPECTRUM_H_ */
