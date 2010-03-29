/*
 * Amplitude.h
 *
 *  Created on: Mar 24, 2010
 *      Author: buzinov
 */

#ifndef AMPLITUDE_H_
#define AMPLITUDE_H_

#include "Scroller2D.h"
#include "Sound.h"
#include "ColorTheme.h"
#include <limits>
#include <cmath>

class Amplitude {
public:
	Amplitude(QGLShaderProgram *program, int width, int height, const QString& background = "");
	~Amplitude();

	void prepare();
	void paint();

private:
	Scroller2D scroller;

	GLubyte *bitmap;
	size_t width, height, columns;
};

#endif /* AMPLITUDE_H_ */
