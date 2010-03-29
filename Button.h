/*
 * Button.h
 *
 *  Created on: Mar 27, 2010
 *      Author: buzinov
 */

#ifndef BUTTON_H_
#define BUTTON_H_

#include <vector>
#include <QString>
#include <cstdarg>
#include <QtOpenGL/QGLWidget>
#include <QtOpenGL/qglshaderprogram.h>

class Button {
public:
  Button(QGLShaderProgram *program, int num_states, const char *url, ...);
  ~Button();

  void makeTextures();
  void paint();
  void switchState();
  void press();
  void release();

private:
  QGLShaderProgram *program;
  std::vector<QString> states;

  GLuint *textures;

  bool pressed;

public:
  int state;
  float mag;
};

#endif /* BUTTON_H_ */
