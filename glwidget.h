/*
 * This code was created by Jeff Molofee 1999
 * And ported to C++/Qt4 by Wesley Stessens 2009
 *
 * Contact Jeff through his website: http://nehe.gamedev.net/
 * Contact Wesley for port-specific comments: wesley@ubuntu.com
 */

#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <QTime>
#include <QHash>

extern "C" {
    #include "glm.h"
}

class QTimer;

class GLWidget : public QGLWidget {
    Q_OBJECT

public:
    GLWidget();
    ~GLWidget();

protected:
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();
    void keyPressEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void changeEvent(QEvent *event);

private:
    void drawCube();
    GLuint loadTexture(QImage image);
    GLuint carregaModelo(GLMmodel* modelo);
    float _angle = 0.0;
    QTimer *timer;
    QTime time;
    GLuint filter;
    GLuint texture[2];
    GLfloat xrot, yrot, xspeed, yspeed, z;
    bool lightChanged, filterChanged;
    int changeAmount;
    QHash<GLuint, QString> filterName;

    GLuint  _textureGrass,
            _textureSky, _textureCenter;
};

#endif // GLWIDGET_H
