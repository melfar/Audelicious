#ifndef AUDELICIOUSQT_H
#define AUDELICIOUSQT_H

#include <QtOpenGL/QGLWidget>
#include <QtCore/QTimer>
#include <QtOpenGL/qglshaderprogram.h>
#include <QMouseEvent>
#include "Spectrum.h"
#include "Amplitude.h"
#include "Equalizer.h"
#include "Button.h"
#include <ctime>
#include <QTime>
#include <vector>
#include "Sound.h"

class AudeliciousQT : public QGLWidget
{
    Q_OBJECT

public:
    AudeliciousQT(QWidget *parent = 0);
    ~AudeliciousQT();

private:
    QTimer *timer;
    QGLShaderProgram program;
    Sound sound;

    Spectrum  *spectrum,  *lowres_spectrum;
    Amplitude *amplitude, *lowres_amplitude;
    Equalizer *equalizer, *lowres_equalizer;
    Button    *btnSensitivity, *btnRange, *btnColor;
    int matrixUniform;
    int frames;
    GLuint logoTexture;
    QTime time;

    float mag, mag_k, shift, shift_k;
    enum {MINIMIZED, MAXIMIZING, MAXIMIZED, MINIMIZING};
    int state;
    int maximized_view;
    int highlighted;
    GLuint backdrops[2];

    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();
    void paintLogo();
    void paintBackdrop(QMatrix4x4 modelview, bool highlight);

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void maximize(int view);

private slots:
	void timerClick();
};

#endif // AUDELICIOUSQT_H
