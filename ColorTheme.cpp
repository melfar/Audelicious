/*
 * ColorTheme.cpp
 *
 *  Created on: Mar 28, 2010
 *      Author: buzinov
 */

#include "ColorTheme.h"

ColorTheme *ColorTheme::instance;

ColorTheme::palette_t* ColorTheme::colors() {
  if (instance == NULL) {
    instance = new ColorTheme();
    ColorTheme::choose(0);
  }
  return instance->colors_;
}

void ColorTheme::choose(int option) {
  palette_t *colors = ColorTheme::colors();

  if (option == 0) {
    int palette[][4] = {{  0,   0,   0, 255},
                        { 64,   0, 255, 255},
                        {128,   0, 255,   0},
                        {192, 255, 255,   0},
                        {256, 255,   0,   0}};
    makePalette(4, palette);

  } else if (option == 1) {
    int palette[][4] = {{  0,   0,   0,   0},
                        { 32,  26,   2, 112},
                        { 64, 111,  13, 170},
                        {128, 249,  44,  84},
                        {256, 247, 255, 175}};
    makePalette(4, palette);

  } else if (option == 2) {
    int palette[][4] = {{  0,   0,   0,   0},
                        { 96, 249,  44,  84},
                        {256, 247, 255, 175}};
    makePalette(2, palette);

  } else if (option == 3) {
    int palette[][4] = {{  0,  19,  95,  64},
                        {256, 255, 255, 255}};
    makePalette(1, palette);

  } else if (option == 4) {
    int palette[][4] = {{  0,   0,   0,   0},
                        { 96,  90,  15, 235},
                        {160, 166, 160, 254},
                        {256, 238, 172, 255}};
    makePalette(3, palette);

  } else if (option == 5) {
    int palette[][4] = {{  0,   0,   0,   0},
                        {256, 255, 255, 255}};
    makePalette(1, palette);

  }

}

void ColorTheme::makePalette(int num_transitions, int palette[][4]) {
  for (int i = 0; i < num_transitions; i++) {
    float r = palette[i][1], g = palette[i][2], b = palette[i][3];
    for (int j = palette[i][0]; j < palette[i+1][0]; j++) {
      ColorTheme::colors()[j].r = r;
      ColorTheme::colors()[j].g = g;
      ColorTheme::colors()[j].b = b;

      r += (palette[i+1][1] - palette[i][1]) / float(palette[i+1][0] - palette[i][0]);
      g += (palette[i+1][2] - palette[i][2]) / float(palette[i+1][0] - palette[i][0]);
      b += (palette[i+1][3] - palette[i][3]) / float(palette[i+1][0] - palette[i][0]);
    }
  }
}
