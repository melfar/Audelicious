/*
 * Equalizer.cpp
 *
 *  Created on: Mar 26, 2010
 *      Author: buzinov
 */

#include "Equalizer.h"

Equalizer::Equalizer( QGLShaderProgram *program, int width, int height)
 	: columns(1), bands_count(22), texture(0) {

  this->program = program;
	this->width   = width;
	this->height  = height;
  bands = new float[bands_count];

  glGenTextures(1, &texture);

	makeTexture();
}

Equalizer::~Equalizer() {
  delete[] bands;
  glDeleteTextures(1, &texture);
}

void Equalizer::makeTexture() {
  glBindTexture(GL_TEXTURE_2D, texture);

  for(int i = 0, c = 0; i < 256; i++) {
    texture_bits[c++] = ColorTheme::colors()[i].r;
    texture_bits[c++] = ColorTheme::colors()[i].g;
    texture_bits[c++] = ColorTheme::colors()[i].b;
    texture_bits[c++] = 255;
  }

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture_bits);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void Equalizer::prepare() {
  int freqs_in_band = Sound::periodsize / bands_count / 8;
  for (int i = 0; i < bands_count; i++) {
    bands[i] = 0;
    for (int j = i*freqs_in_band; j < (i+1)*freqs_in_band; j++) {
      bands[i] += fabs(Sound::out[j] / 256);
    }
    bands[i] /= freqs_in_band;
    if (bands[i] > 1.0)  bands[i] = 1.0;
  }
}

void Equalizer::paint() {
    glBindTexture(GL_TEXTURE_2D, texture);

	int vertexAttr     = program->attributeLocation("vertex");
  int texCoordAttr   = program->attributeLocation("texCoord");

  float step = 1.0 / bands_count;
  float gap = step / 8;
  for (int i = 0; i < bands_count; i++) {
  	GLfloat rectangle_vertices[] = {-0.5,              -0.5,
  									                -0.5 + step - gap, -0.5,
  						                 			-0.5,               0.5,
  									                -0.5 + step - gap,  0.5};
  	GLfloat tex_coords[] = {0.0, 0.0,
  							1.0, 0.0,
  							0.0, 1.0,
  							1.0, 1.0};

    rectangle_vertices[0] += i*step;
    rectangle_vertices[2] += i*step;
    rectangle_vertices[4] += i*step;
    rectangle_vertices[6] += i*step;
              
    rectangle_vertices[5] = -0.5 + bands[i];
    rectangle_vertices[7] = -0.5 + bands[i];
    tex_coords[5] = bands[i];
    tex_coords[7] = bands[i];

  	program->setAttributeArray(vertexAttr, rectangle_vertices, 2);
  	program->setAttributeArray(texCoordAttr, tex_coords, 2);
  	program->enableAttributeArray(vertexAttr);
  	program->enableAttributeArray(texCoordAttr);
  	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  	program->disableAttributeArray(vertexAttr);
  	program->disableAttributeArray(texCoordAttr);
  }
	
}
