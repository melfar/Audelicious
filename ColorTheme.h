/*
 * ColorTheme.h
 *
 *  Created on: Mar 28, 2010
 *      Author: buzinov
 */

#ifndef COLORTHEME_H_
#define COLORTHEME_H_

#include <QtOpenGL/QGLWidget>

class ColorTheme {
  struct palette_t { GLubyte r, g, b; };
  palette_t colors_[256];

  static ColorTheme *instance;
  static void makePalette(int num_transitions, int palette[][4]);
public:
  static void choose(int option);
  static palette_t* colors();

};

#endif /* COLORTHEME_H_ */
