/*
 * Scroller2D.cpp
 *
 *  Created on: Mar 17, 2010
 *      Author: buzinov
 */

#include "Scroller2D.h"

Scroller2D::Scroller2D(QGLShaderProgram *program, int width, int height, int columns, const QString& background)
 : offset(0), texture(0) {

	this->program  = program;
	vertexAttr     = program->attributeLocation("vertex");
    texCoordAttr   = program->attributeLocation("texCoord");
    textureUniform = program->uniformLocation("tex");

    this->width      = width;
    this->height     = height;
    this->columns    = columns;
    this->background = background;
    glGenTextures(1, &texture);
    makeTexture();
}

Scroller2D::~Scroller2D() {
	glDeleteTextures(1, &texture);
}

void Scroller2D::makeTexture() {
	glBindTexture(GL_TEXTURE_2D, texture);

	offset = 0;
	texture_width = texture_height = 1;
  // find the closest power of two
  while (texture_width  < width)   texture_width  <<= 1;
  while (texture_height < height)  texture_height <<= 1;

  GLubyte *bits = NULL;
  QImage back;
  if (!background.isEmpty()) {
    back = QGLWidget::convertToGLFormat(QImage(background));
    bits = back.bits();
  } else {
    bits = new GLubyte[texture_width*texture_height*4]; // :TODO: memory leak
    memset(bits, 0, texture_width*texture_height*4);
  }

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_width,   texture_height,   0, GL_RGBA, GL_UNSIGNED_BYTE, bits);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_REPEAT);

  program->setUniformValue(textureUniform, 0);
}

void Scroller2D::paint(GLubyte *bitmap) {
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexSubImage2D(GL_TEXTURE_2D, 0, offset, 0, columns, height, GL_RGBA, GL_UNSIGNED_BYTE, bitmap);

  float step = 1.0 / texture_width;

	GLfloat rectangle_vertices[] = {-0.5, -0.5,
                                   0.5, -0.5,
                                  -0.5,  0.5,
                                   0.5,  0.5};
	GLfloat tex_coords[] = {0.0,                        0.0,
                          float(width)/texture_width, 0.0,
                          0.0,                        float(height)/texture_height,
                          float(width)/texture_width, float(height)/texture_height};

	tex_coords[0] += (offset+(texture_width-width))*step;
	tex_coords[2] += (offset+(texture_width-width))*step;
	tex_coords[4] += (offset+(texture_width-width))*step;
	tex_coords[6] += (offset+(texture_width-width))*step;

	program->setAttributeArray(vertexAttr, rectangle_vertices, 2);
	program->setAttributeArray(texCoordAttr, tex_coords, 2);
	program->enableAttributeArray(vertexAttr);
	program->enableAttributeArray(texCoordAttr);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	program->disableAttributeArray(vertexAttr);
	program->disableAttributeArray(texCoordAttr);

	offset += columns;
  if (offset >= texture_width)  offset = 0;
}
