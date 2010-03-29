/*
 * Button.cpp
 *
 *  Created on: Mar 27, 2010
 *      Author: buzinov
 */

#include "Button.h"

Button::Button(QGLShaderProgram *program, int num_states, const char* url, ...) {
  this->program = program;
  state   = 0;
  mag     = 1.0;
  pressed = false;

  va_list list;
  va_start(list, url);

  for (const char *p = url; num_states != 0; num_states--) {
    states.push_back(p);
    p = va_arg(list, const char *);
  }
  va_end(list);

  makeTextures();
}

Button::~Button() {
  delete[] textures;
}

void Button::makeTextures() {
  textures = new GLuint[states.size()];

  glGenTextures(states.size(), textures);

  for(int i = 0; i < states.size(); i++) {
    glBindTexture(GL_TEXTURE_2D, textures[i]);

    QImage img = QGLWidget::convertToGLFormat(QImage(states[i]));

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width(), img.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img.bits());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  }
}

void Button::paint() {
  if (!pressed && mag > 1.0)  mag -= 0.05;

  glBindTexture(GL_TEXTURE_2D, textures[state]);

  int vertexAttr     = program->attributeLocation("vertex");
  int texCoordAttr   = program->attributeLocation("texCoord");

  // 88 pixels
  float btn_width  = 88.0 / 800;
  float btn_height = 88.0 / 480;

  GLfloat rectangle_vertices[] = {-btn_width/2, -btn_height/2,
                                   btn_width/2, -btn_height/2,
                                  -btn_width/2,  btn_height/2,
                                   btn_width/2,  btn_height/2};
  GLfloat tex_coords[] = {0.0, 0.0,
                          1.0, 0.0,
                          0.0, 1.0,
                          1.0, 1.0};

  program->setAttributeArray(vertexAttr, rectangle_vertices, 2);
  program->setAttributeArray(texCoordAttr, tex_coords, 2);
  program->enableAttributeArray(vertexAttr);
  program->enableAttributeArray(texCoordAttr);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  program->disableAttributeArray(vertexAttr);
  program->disableAttributeArray(texCoordAttr);
}

void Button::switchState() {
  state++;
  if (state >= states.size())  state = 0;
}

void Button::press() {
  mag     = 1.3;
  pressed = true;
}

void Button::release() {
  switchState();
  pressed = false;
}
