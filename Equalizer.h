/*
 * Equalizer.h
 *
 *  Created on: Mar 26, 2010
 *      Author: buzinov
 */

#ifndef EQUALIZER_H_
#define EQUALIZER_H_

#include "Sound.h"
#include <fftw3.h>
#include <QtOpenGL/QGLWidget>
#include <QtOpenGL/qglshaderprogram.h>
#include <cmath>
#include "ColorTheme.h"

class Equalizer {
public:
	Equalizer(QGLShaderProgram *program, int width, int height);
	~Equalizer();
	
	void prepare();
	void paint();
private:
  QGLShaderProgram *program;
	size_t width, height, columns;
	
  float *bands;
  size_t bands_count;

	GLubyte texture_bits[256*4];
	
	GLuint texture;
public:
	void makeTexture();

};

#endif /* EQUALIZER_H_ */
