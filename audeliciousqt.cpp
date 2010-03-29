#include "audeliciousqt.h"

AudeliciousQT::AudeliciousQT(QWidget *parent)
    : QGLWidget(parent)
{
	state = MINIMIZED;
	mag   = -15.0;
	shift = 0;
	highlighted = 0;
}

AudeliciousQT::~AudeliciousQT()
{
  glDeleteTextures(1, &logoTexture);
  glDeleteTextures(2, backdrops);

	delete spectrum;
	delete lowres_spectrum;
	delete amplitude;
	delete lowres_amplitude;
	delete equalizer;
	delete lowres_equalizer;
	delete btnSensitivity;
	delete btnRange;
	delete btnColor;
}

void AudeliciousQT::initializeGL()
{
	frames = 0;
	time.start();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);

    QGLShader *vshader1 = new QGLShader(QGLShader::Vertex, this);
    const char *vsrc1 =
   	     "attribute highp vec4 vertex;\n"
   	     "uniform mediump mat4 matrix;\n"
         "attribute highp vec4 texCoord;\n"
         "varying highp vec4 texc;\n"
   	     "void main(void)\n"
   	     "{\n"
   	     "   gl_Position = matrix * vertex;\n"
         "   texc = texCoord;\n"
   	     "}";
    assert(vshader1->compileSourceCode(vsrc1));

    QGLShader *fshader1 = new QGLShader(QGLShader::Fragment, this);
	const char *fsrc1 =
		"varying highp vec4 texc;\n"
		"uniform sampler2D tex;\n"
		"void main(void)\n"
		"{\n"
		"    highp vec4 color = texture2D(tex, texc.st);\n"
		"	 gl_FragColor = color;\n"
		"}\n";
	assert(fshader1->compileSourceCode(fsrc1));

	program.addShader(vshader1);
	program.addShader(fshader1);

	program.link();

	// logo
	QImage tmpLogo = QGLWidget::convertToGLFormat(QImage(":/images/background.jpg"));
	glGenTextures(1, &logoTexture);
	glBindTexture(GL_TEXTURE_2D, logoTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tmpLogo.width(), tmpLogo.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, tmpLogo.bits());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	spectrum         = new Spectrum(&program, 800, 480, ":/images/spectrum.jpg");
	lowres_spectrum  = new Spectrum(&program, 200, 120);

	amplitude        = new Amplitude(&program, 800, 480, ":/images/amplitude.jpg");
	lowres_amplitude = new Amplitude(&program, 200, 120);

  equalizer        = new Equalizer(&program, 800, 480);
  lowres_equalizer = new Equalizer(&program, 200, 120);

  btnSensitivity   = new Button(&program, 3, ":images/btn_sensitivity1.png", ":images/btn_sensitivity2.png", ":images/btn_sensitivity3.png");
  btnRange         = new Button(&program, 2, ":images/btn_range1.png", ":images/btn_range2.png");
  btnColor         = new Button(&program, 6, ":images/btn_color1.png", ":images/btn_color2.png", ":images/btn_color3.png",
                                             ":images/btn_color4.png", ":images/btn_color5.png", ":images/btn_color6.png");


  glGenTextures(2, backdrops);

  GLubyte normal_backdrop_bits[] = {0, 0, 0, 255};
  glBindTexture(GL_TEXTURE_2D, backdrops[0]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, normal_backdrop_bits);

  GLubyte highlight_backdrop_bits[] = {96, 96, 96, 255};
  glBindTexture(GL_TEXTURE_2D, backdrops[1]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, highlight_backdrop_bits);

  matrixUniform = program.uniformLocation("matrix");

	timer = new QTimer(this);
	timer->setInterval(30);
	connect(timer, SIGNAL(timeout()), this, SLOT(timerClick()));
	timer->start(0);
}

void AudeliciousQT::resizeGL(int width, int height)
{
//	glViewport(0, 0, width, height);
//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
//    glFrustum(-0.5, +0.5, -0.5, +0.5, 4.0, 30.0);
//    glMatrixMode(GL_MODELVIEW);
}

void AudeliciousQT::paintGL()
{
#if 0
    QPainter painter;
    painter.begin(this);
    painter.beginNativePainting();
#endif

    program.bind();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    paintLogo();

	Sound::prepare();
    if (state != MAXIMIZED) {
    	lowres_spectrum->prepare();
    	lowres_amplitude->prepare();
    	lowres_equalizer->prepare();

    	QMatrix4x4 modelview;
      modelview.setToIdentity();
    	modelview.frustum(-0.5, 0.5, -0.5, 0.5, 4.0, 30.0);
    	modelview.translate(-1.5, 0.0, -4.0 + mag);
    	program.setUniformValue(matrixUniform, modelview);
    	lowres_amplitude->paint();
    	paintBackdrop(modelview, highlighted == 1);

      modelview.setToIdentity();
    	modelview.frustum(-0.5, 0.5, -0.5, 0.5, 4.0, 30.0);
      modelview.translate(0.0, 0.0, -4.0 + mag);
    	program.setUniformValue(matrixUniform, modelview);
    	lowres_spectrum->paint();
    	paintBackdrop(modelview, highlighted == 2);

      modelview.setToIdentity();
    	modelview.frustum(-0.5, 0.5, -0.5, 0.5, 4.0, 30.0);
      modelview.translate(1.5, 0.0, -4.0 + mag);
    	program.setUniformValue(matrixUniform, modelview);
    	lowres_equalizer->paint();
    	paintBackdrop(modelview, highlighted == 3);

      // buttons
      modelview.setToIdentity();
      modelview.frustum(-0.5, 0.5, -0.5, 0.5, 4.0, 30.0);
      modelview.translate(-0.2*2, -0.34*2, -4.0*2); // put buttons behind a bit
      modelview.scale(2.0);                         // correct scaling
      modelview.scale(btnSensitivity->mag);
      program.setUniformValue(matrixUniform, modelview);
      btnSensitivity->paint();

      modelview.setToIdentity();
      modelview.frustum(-0.5, 0.5, -0.5, 0.5, 4.0, 30.0);
      modelview.translate(0.0*2, -0.34*2, -4.0*2);
      modelview.scale(2.0);
      modelview.scale(btnRange->mag);
      program.setUniformValue(matrixUniform, modelview);
      btnRange->paint();

      modelview.setToIdentity();
      modelview.frustum(-0.5, 0.5, -0.5, 0.5, 4.0, 30.0);
      modelview.translate(0.2*2, -0.34*2, -4.0*2);
      modelview.scale(2.0);
      modelview.scale(btnColor->mag);
      program.setUniformValue(matrixUniform, modelview);
      btnColor->paint();
    }

    if (state != MINIMIZED) {
    	amplitude->prepare();
    	spectrum->prepare();
    	equalizer->prepare();

		QMatrix4x4 modelview;
		modelview.setToIdentity();
		modelview.frustum(-0.5, 0.5, -0.5, 0.5, 4.0, 30.0);
		modelview.translate(0.0, shift, -4.0);
		program.setUniformValue(matrixUniform, modelview);

		switch(maximized_view) {
		case 1:
			amplitude->paint();
			break;
		case 2:
			spectrum->paint();
			break;
		case 3:
		  equalizer->paint();
      paintBackdrop(modelview, false);
			break;
		}


		switch(state) {
			case MAXIMIZING:
				shift += sqrt(-shift/200);
				if (shift >=    0)   { state = MAXIMIZED; shift =   0; }
			break;
			case MINIMIZING:
				shift -= sqrt((1.0+shift)/100);
				if (shift <= -1.0)  { state = MINIMIZED; shift = -1.0; }
			break;
		}
    }

    program.release();



    if (frames % 100 == 0) {
    	qDebug("fps: %f", frames / (time.elapsed() / 1000.0));
    	time.start();
    	frames = 0;
    }
    frames++;
}

void AudeliciousQT::paintLogo() {
	glBindTexture(GL_TEXTURE_2D, logoTexture);

	QMatrix4x4 modelview;
    modelview.setToIdentity();
	modelview.frustum(-0.5, 0.5, -0.5, 0.5, 4.0, 30.0);
	modelview.translate(0.0, 0.0, -20.0);
	modelview.scale(5.0);
	program.setUniformValue(matrixUniform, modelview);

	int vertexAttr     = program.attributeLocation("vertex");
    int texCoordAttr   = program.attributeLocation("texCoord");

	GLfloat rectangle_vertices[] = {-0.5, -0.5,
									 0.5, -0.5,
									-0.5,  0.5,
									 0.5,  0.5};
	GLfloat tex_coords[] = {0.0, 0.0,
							1.0, 0.0,
							0.0, 1.0,
							1.0, 1.0};

	program.setAttributeArray(vertexAttr, rectangle_vertices, 2);
	program.setAttributeArray(texCoordAttr, tex_coords, 2);
	program.enableAttributeArray(vertexAttr);
	program.enableAttributeArray(texCoordAttr);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	program.disableAttributeArray(vertexAttr);
	program.disableAttributeArray(texCoordAttr);
}

void AudeliciousQT::paintBackdrop(QMatrix4x4 modelview, bool highlight) {
	glBindTexture(GL_TEXTURE_2D, 0);

	int vertexAttr   = program.attributeLocation("vertex");
  int texCoordAttr = program.attributeLocation("texCoord");

	GLfloat rectangle_vertices[] = {-0.5, -0.5,
									 0.5, -0.5,
									-0.5,  0.5,
									 0.5,  0.5};
	GLfloat tex_coords[] = {0.0, 0.0,
							1.0, 0.0,
							0.0, 1.0,
							1.0, 1.0};


	program.setAttributeArray(vertexAttr, rectangle_vertices, 2);
	program.setAttributeArray(texCoordAttr, tex_coords, 2);
	program.enableAttributeArray(vertexAttr);
	program.enableAttributeArray(texCoordAttr);

	// background
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	// border
	glBindTexture(GL_TEXTURE_2D, backdrops[highlight ? 1 : 0]);
  modelview.scale(1.05, 1.09);
  program.setUniformValue(matrixUniform, modelview);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	program.disableAttributeArray(vertexAttr);
	program.disableAttributeArray(texCoordAttr);
}

void AudeliciousQT::timerClick() {
	updateGL();
}

void AudeliciousQT::mousePressEvent(QMouseEvent *event) {
	int x = event->x(), y = event->y();
	qDebug("press %d, %d", x, y);

	// hot zones
	if (state == MINIMIZED) {
		if (y > 160 && y < 320) {
			if (x <  266)             highlighted = 1;
			if (x >= 266 && x < 533)  highlighted = 2;
			if (x >= 533)             highlighted = 3;
		}
		if (y > 320) {
		  if (x > 170 && x < 270)  btnSensitivity->press();
      if (x > 350 && x < 450)  btnRange->press();
      if (x > 500 && x < 600)  btnColor->press();
		}
	}
}

void AudeliciousQT::mouseReleaseEvent(QMouseEvent *event) {
  int x = event->x(), y = event->y();
  qDebug("release %d, %d", x, y);

  highlighted = 0;

  // hot zones
  if (state == MINIMIZED) {
    if (x > 740 && y < 50 ) parentWidget()->close();  // exit
    if (y > 160 && y < 320) {
      if (x < 266)             maximize(1);
      if (x >= 266 && x < 533) maximize(2);
      if (x >= 533)            maximize(3);
    }
    if (y > 320) {
      if (x > 170 && x < 270) {
        btnSensitivity->release();
        Sound::setSensitivity(btnSensitivity->state);
      }
      if (x > 350 && x < 450) {
        btnRange->release();
        spectrum->toggleRange(true);
        lowres_spectrum->toggleRange(false);
        Sound::setRange(btnRange->state);
      }
      if (x > 500 && x < 600) {
        btnColor->release();
        ColorTheme::choose(btnColor->state);
        equalizer->makeTexture();
        lowres_equalizer->makeTexture();
      }
    }
  } else if (state == MAXIMIZED) {
    state = MINIMIZING;
  }
}


void AudeliciousQT::maximize(int view) {
    qDebug("maximize %d", view);

    shift          = -1.0;
    shift_k        = 0.08;
    state          = MAXIMIZING;
    maximized_view = view;
}
