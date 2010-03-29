/*
 * Scroller2D.h
 *
 *  Created on: Mar 17, 2010
 *      Author: buzinov
 */

#ifndef SCROLLER2D_H_
#define SCROLLER2D_H_

#include <QtOpenGL/QGLWidget>
#include <QtOpenGL/qglshaderprogram.h>


class Scroller2D {
public:
	Scroller2D(QGLShaderProgram *program, int width, int height, int columns, const QString& background);
	~Scroller2D();

	void paint(GLubyte *bitmap);

private:
  QGLShaderProgram *program;
  int  vertexAttr;
  int  texCoordAttr;
  int  textureUniform;

	int  width, height, texture_width, texture_height, columns;
	int  offset;
	GLuint texture;

public:
	QString background;
  void makeTexture();
};

#endif /* SCROLLER2D_H_ */
