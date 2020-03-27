QT += opengl
QT += multimedia

TARGET = AbreModelo

TEMPLATE = app

SOURCES +=  main.cpp \
            glm.c \
            glwidget.cpp

HEADERS +=  glwidget.h \
            materials.h \
            glm.h \
            timercpp.h

RESOURCES += res.qrc

win32 {
    INCLUDEPATH += \
        C:/freeglut-build-3.2.1/install/include

    LIBS += \
        -LC:/freeglut-build-3.2.1/install/lib \
        -lfreeglut \
        -lopengl32 \
        -lglu32
}

unix {
    INCLUDEPATH += /usr/include

    LIBS += /usr/lib/x86_64-linux-gnu/libglut.so \
        -lGL \
        -lGLU
}

DISTFILES +=



