TEMPLATE = app
TARGET = AudeliciousQT
QT += core \
    gui \
    opengl
HEADERS += ColorTheme.h \
    Button.h \
    Equalizer.h \
    Amplitude.h \
    Sound.h \
    Scroller2D.h \
    audeliciousqt.h \
    Spectrum.h
SOURCES += ColorTheme.cpp \
    Button.cpp \
    Equalizer.cpp \
    Amplitude.cpp \
    Sound.cpp \
    Scroller2D.cpp \
    main.cpp \
    audeliciousqt.cpp \
    Spectrum.cpp
FORMS += audeliciousqt.ui
RESOURCES += audelicious.qrc
LIBS += -lasound \
    -lfftw3
