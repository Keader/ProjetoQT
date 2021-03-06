
/*
 * This code was created by Jeff Molofee 1999
 * And ported to C++/Qt4 by Wesley Stessens 2009
 *
 * Contact Jeff through his website: http://nehe.gamedev.net/
 * Contact Wesley for port-specific comments: wesley@ubuntu.com
 */

#include "glwidget.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <QKeyEvent>
#include <QTimer>
#include <QDebug>
#include <chrono>
#include <thread>
#include "timercpp.h"
#include <QMessageBox>
#include <QRandomGenerator>

enum Botoes
{
    BOTAO_AZUL     = 0,
    BOTAO_VERMELHO = 1,
    BOTAO_VERDE    = 2,
    BOTAO_AMARELO  = 3,
    MAX_BOTOES
};

GLMmodel* cilindro = nullptr;
GLMmodel* botaoAzul = nullptr;
GLMmodel* botaoVerde = nullptr;
GLMmodel* botaoAmarelo = nullptr;
GLMmodel* botaoVermelho = nullptr;
GLMmodel* botaoAzul_on = nullptr;
GLMmodel* botaoVerde_on = nullptr;
GLMmodel* botaoAmarelo_on = nullptr;
GLMmodel* botaoVermelho_on = nullptr;
GLMmodel* centro = nullptr;
Timer t = Timer();

// Constructor
GLWidget::GLWidget() {
    setWindowTitle("Genius 3D - Aperte o botão '3' para iniciar o jogo.");
    time = QTime::currentTime();
    timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateGL()));
}

// Destructor
GLWidget::~GLWidget() {
    glDeleteTextures(1, &_textureGrass);
    glDeleteTextures(1, &_textureSky);
}

// Initialize OpenGL
void GLWidget::initializeGL() {
    glShadeModel(GL_SMOOTH); // Enable smooth shading
    qglClearColor(Qt::black); // Set the clear color to a black background

    glClearDepth(1.0f); // Depth buffer setup
    glEnable(GL_DEPTH_TEST); // Enable depth testing
    glDepthFunc(GL_LEQUAL); // Set type of depth test

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Really nice perspective calculations


//   glClearColor(0.0, 0.0, 0.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    //glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
    glEnable(GL_TEXTURE_2D);

    //glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    QImage img;
    img = convertToGLFormat(QImage("grass.bmp"));
    _textureGrass = loadTexture(img);
    img = convertToGLFormat(QImage("sky.bmp"));
    _textureSky = loadTexture(img);
    img = convertToGLFormat(QImage("modelos/centro/concrete.jpg"));
    _textureCenter = loadTexture(img);
    for (int i = 0; i < MAX_BOTOES; ++i)
        _buttonIsOn.push_back(false);


    cilindro = glmReadOBJ("modelos/cilindro.obj");
    centro = glmReadOBJ("modelos/centro.obj");

    botaoAzul =  glmReadOBJ("modelos/azul.obj");
    botaoVerde = glmReadOBJ("modelos/verde.obj");
    botaoAmarelo = glmReadOBJ("modelos/amarelo.obj");
    botaoVermelho = glmReadOBJ("modelos/vermelho.obj");
    botaoAzul_on = glmReadOBJ("modelos/azul_claro.obj");
    botaoVerde_on = glmReadOBJ("modelos/verde_claro.obj");
    botaoAmarelo_on = glmReadOBJ("modelos/amarelo_claro.obj");
    botaoVermelho_on = glmReadOBJ("modelos/vermelho_claro.obj");
}

// This is called when the OpenGL window is resized
void GLWidget::resizeGL(int width, int height) {
    const float ar = (float) width / (float) height;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-ar, ar, -1.0, 1.0, 2.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// OpenGL painting code goes here
void GLWidget::paintGL() {

    // Verifica qual modelo de botao sera carregado
    //qDebug() << _angle;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Sky
    glPushMatrix();
        glBindTexture(GL_TEXTURE_2D, _textureSky);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTranslatef(0,0,-10);
        glBegin(GL_QUADS);
            glTexCoord3f(0.0,1.0,0.1);  glVertex3f(-10,10,0);
            glTexCoord3f(1.0,1.0,0.1);  glVertex3f(10,10,0);
            glTexCoord3f(1.0,0.0,0.1);  glVertex3f(10,-10,0);
            glTexCoord3f(0.0,0.0,0.1);  glVertex3f(-10,-10,0);
        glEnd();
    glPopMatrix();

    // Grass
    glPushMatrix();
        glBindTexture(GL_TEXTURE_2D, _textureGrass);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTranslatef(0,0,-6);
        glRotatef(_angle, 0.0, 1.0, 0.0);
        glBegin(GL_QUADS);
            glTexCoord3f(0.0,70.0,1);  glVertex3f(-50,-1.5,50);
            glTexCoord3f(0.0,0.0,-1);  glVertex3f(-50,-1.5,-50);
            glTexCoord3f(70.0,0.0,-1);  glVertex3f(50,-1.5,-50);
            glTexCoord3f(70.0,70.0,1);  glVertex3f(50,-1.5,50);
        glEnd();
    glPopMatrix();

    //Carrega o Modelo do cilindro
    glPushMatrix();
        glEnable(GL_LIGHTING);
        glBindTexture(GL_TEXTURE_2D, _textureCenter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTranslatef(0,1.1,-8);
        // Deixa o objeto posicionado de frente
        glRotatef(-90, 1, 0, 0.0);
        // Faz a rotacao pelos botoes
        // glRotatef(_angle, 0, 1, 0.0); rotacao em Z
        // glRotatef(_angle, 1, 0, 0.0); rotacao em X
        // glRotatef(_angle, 0, 0, 1.0); rotacao em Y
        glRotatef(_angle, 0, 0, 1.0);
        glScalef(2.5, 5, 2.5);
        carregaModelo(cilindro);
        glDisable(GL_LIGHTING);
    glPopMatrix();

    if (_angle < 0 && (_angle >= -80 || _angle < -280))
    {
        //Carrega o Modelo do botao azul (cima)
        glPushMatrix();
            glEnable(GL_LIGHTING);
            glTranslatef(0,2.2,-6);
            // Deixa o objeto posicionado de frente
            glRotatef(-80, 1, 0, 0.0);
            glRotatef(_angle, 0, 0, 1.0);
            carregaModelo(_buttonIsOn[BOTAO_AZUL] ? botaoAzul_on : botaoAzul);
            glDisable(GL_LIGHTING);
        glPopMatrix();
    }
    else if (_angle >= 0  && (_angle <= 80 || _angle > 280))
    {
        //Carrega o Modelo do botao azul (cima)
        glPushMatrix();
            glEnable(GL_LIGHTING);
            glTranslatef(0,2.2,-6);
            // Deixa o objeto posicionado de frente
            glRotatef(-80, 1, 0, 0.0);
            glRotatef(_angle, 0, 0, 1.0);
            carregaModelo(_buttonIsOn[BOTAO_AZUL] ? botaoAzul_on : botaoAzul);
            glDisable(GL_LIGHTING);
        glPopMatrix();
    }

    if (_angle < 0 && (_angle >= -80 || _angle < -280))
    {
        //Carrega o Modelo do botao verde (baixo)
        glPushMatrix();
            glEnable(GL_LIGHTING);
            glTranslatef(0,-0.4,-6);
            // Deixa o objeto posicionado de frente
            glRotatef(-80, 1, 0, 0.0);
            glRotatef(_angle, 0, 0, 1.0);
            carregaModelo(_buttonIsOn[BOTAO_VERDE] ? botaoVerde_on : botaoVerde);
            glDisable(GL_LIGHTING);
        glPopMatrix();
    }
    else if (_angle >= 0  && (_angle <= 80 || _angle > 280))
    {
        //Carrega o Modelo do botao verde (baixo)
        glPushMatrix();
            glEnable(GL_LIGHTING);
            glTranslatef(0,-0.4,-6);
            // Deixa o objeto posicionado de frente
            glRotatef(-80, 1, 0, 0.0);
            glRotatef(_angle, 0, 0, 1.0);
            carregaModelo(_buttonIsOn[BOTAO_VERDE] ? botaoVerde_on : botaoVerde);
            glDisable(GL_LIGHTING);
        glPopMatrix();
    }

    // Giro pra esquerda
    if (_angle < 0 && (_angle >= -30 || _angle < -290))
    {
        //Carrega o Modelo do botao amarelo (esquerda)
        glPushMatrix();
            glEnable(GL_LIGHTING);
            glTranslatef(-1.2,0.85,-6);
            // Deixa o objeto posicionado de frente
            glRotatef(-80, 1, 0, 0.0);
            glRotatef(_angle, 0, 0, 1.0);
            carregaModelo(_buttonIsOn[BOTAO_AMARELO] ? botaoAmarelo_on : botaoAmarelo);
            glDisable(GL_LIGHTING);
        glPopMatrix();
    }
    else if (_angle >= 0 && (_angle < 50 || _angle > 310))
    {
        //Carrega o Modelo do botao amarelo (esquerda)
        glPushMatrix();
            glEnable(GL_LIGHTING);
            glTranslatef(-1.2,0.85,-6);
            // Deixa o objeto posicionado de frente
            glRotatef(-80, 1, 0, 0.0);
            glRotatef(_angle, 0, 0, 1.0);
            carregaModelo(_buttonIsOn[BOTAO_AMARELO] ? botaoAmarelo_on : botaoAmarelo);
            glDisable(GL_LIGHTING);
        glPopMatrix();
    }

    // Giro pra esquerda
    if (_angle < 0 && (_angle >= -40 || _angle < -320))
    {
        //Carrega o Modelo do botao vermelho (direita)
        glPushMatrix();
            glEnable(GL_LIGHTING);
            glTranslatef(1.2,0.85,-6);
            // Deixa o objeto posicionado de frente
            glRotatef(-80, 1, 0, 0.0);
            glRotatef(_angle, 0, 0, 1.0);
            carregaModelo(_buttonIsOn[BOTAO_VERMELHO] ? botaoVermelho_on : botaoVermelho);
            glDisable(GL_LIGHTING);
        glPopMatrix();
    }
    // Giro pra direita
    else if (_angle >= 0 && (_angle < 40 || _angle > 290))
    {

        //Carrega o Modelo do botao vermelho (direita)
        glPushMatrix();
            glEnable(GL_LIGHTING);
            glTranslatef(1.2,0.85,-6);
            // Deixa o objeto posicionado de frente
            glRotatef(-80, 1, 0, 0.0);
            glRotatef(_angle, 0, 0, 1.0);
            carregaModelo(_buttonIsOn[BOTAO_VERMELHO] ? botaoVermelho_on : botaoVermelho);
            glDisable(GL_LIGHTING);
         glPopMatrix();
    }

    if (_angle < 0 && (_angle >= -80 || _angle < -280))
    {
        //Carrega o Modelo do centro
        glPushMatrix();
            glEnable(GL_LIGHTING);
            glTranslatef(0,0.8,-6);
            // Deixa o objeto posicionado de frente
            glRotatef(-80, 1, 0, 0.0);
            glRotatef(_angle, 0, 0, 1.0);
            glScalef(0.5, 0.5, 0.5);
            carregaModelo(centro);
            glDisable(GL_LIGHTING);
        glPopMatrix();
    }
    else if (_angle >= 0  && (_angle <= 80 || _angle > 280))
    {
        //Carrega o Modelo do centro
        glPushMatrix();
            glEnable(GL_LIGHTING);
            glTranslatef(0,0.8,-6);
            // Deixa o objeto posicionado de frente
            glRotatef(-80, 1, 0, 0.0);
            glRotatef(_angle, 0, 0, 1.0);
            glScalef(0.5, 0.5, 0.5);
            carregaModelo(centro);
            glDisable(GL_LIGHTING);
        glPopMatrix();
    }

    // Framerate control
    int delay = time.msecsTo(QTime::currentTime());
    if (delay == 0)
        delay = 1;
    time = QTime::currentTime();
    timer->start(qMax(0, 20 - delay));
}

// Key handler
void GLWidget::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
    case Qt::Key_Escape:
        close(); // Quit on Escape
        break;
    case Qt::Key_F1:
        setWindowState(windowState() ^ Qt::WindowFullScreen); // Toggle fullscreen on F1
        break;
    case Qt::Key_1:
        _angle -= 10;
        if (_angle < -360)
            _angle = 0.0;
        break;
    case Qt::Key_2:
        _angle += 10;
        if (_angle > 360)
            _angle = 0.0;
        break;
    case Qt::Key_3:
        InicializaJogo();
        break;
    case Qt::Key_Left:
    case Qt::Key_Up:
    case Qt::Key_Right:
    case Qt::Key_Down:
        if (!_isAllowedCommands)
            return;

        HandlePlayerCommand(event->key());
        break;
    default:
        QGLWidget::keyPressEvent(event); // Let base class handle the other keys
    }
}

void GLWidget::HandlePlayerCommand(int key)
{
    int valor = 0;
    if (key == Qt::Key_Left)
        valor = BOTAO_AMARELO;
    else if (key == Qt::Key_Right)
        valor = BOTAO_VERMELHO;
    else if (key == Qt::Key_Up)
        valor = BOTAO_AZUL;
    else if (key == Qt::Key_Down)
        valor = BOTAO_VERDE;

    // Handle animation
    _buttonIsOn[valor] = true;
    t.setTimeout([&, valor]()
    {
        _buttonIsOn[valor] = false;
        ChecaResultado();
    }, 800);

    _playerChoices.push_back(valor);
    SelecionaMusicaCorreta(valor);
    _audio.play();
}

void GLWidget::ChecaResultado()
{
    if (_playerChoices[m_currentPlayerGuess] == _gameValues[m_currentPlayerGuess])
    {
        // Checa se ja verificou todas as escolhas
        if (_playerChoices.size() ==  _gameValues.size())
        {
            _isAllowedCommands = false;
            _playerChoices.clear();
            _gameValues.clear();
            _level++;
            m_currentPlayerGuess = 0;
            setWindowTitle("Nivel: " + QString::number(_level));
            t.setTimeout([&]()
            {
                LogicaDoLevel();
            }, 1000);
            return;
        }

        // Ainda faltam respostas, entao aguarda o proximo comando
        ++m_currentPlayerGuess;
    }
    else
        InicializaJogo();
}

void GLWidget::InicializaJogo()
{
    _level = 1;
    setWindowTitle("Nivel: " + QString::number(_level));
    _playerChoices.clear();
    _gameValues.clear();
    m_currentPlayerGuess = 0;
    _isAllowedCommands = false;
    LogicaDoLevel();
}

void GLWidget::LogicaDoLevel()
{
    for (int i = 0; i < _level; ++i)
    {
        int currentVal = QRandomGenerator::global()->bounded(0, 4);
        t.setTimeout([&, currentVal]()
        {
            LidaComBotaoAnimacao(currentVal);
        }, i * 1000);

        // Libera os comandos do usuario, 100 ms apos o ultimo botao fazer animacao
        t.setTimeout([&]()
        {
            _isAllowedCommands = true;
        }, _level * 1050);
    }
}

void GLWidget::LidaComBotaoAnimacao(int valor)
{
    _gameValues.push_back(valor);
    _buttonIsOn[valor] = true;
    t.setTimeout([&, valor]()
    {
         _buttonIsOn[valor] = false;
    }, 800);
    SelecionaMusicaCorreta(valor);
    _audio.play();
}

void GLWidget::SelecionaMusicaCorreta(int valor)
{
    switch(valor)
    {
        case BOTAO_AZUL:
            _audio.setSource(QUrl::fromLocalFile("sounds/blue.wav"));
        break;
        case BOTAO_VERDE:
            _audio.setSource(QUrl::fromLocalFile("sounds/green.wav"));
            break;
        case BOTAO_VERMELHO:
            _audio.setSource(QUrl::fromLocalFile("sounds/red.wav"));
            break;
        case BOTAO_AMARELO:
            _audio.setSource(QUrl::fromLocalFile("sounds/yellow.wav"));
            break;
        default:
            break;
    }
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    //qDebug() << "Chamado com as posicoes: " << event->pos();
    QGLWidget::mouseMoveEvent(event);
}

void GLWidget::changeEvent(QEvent *event) {
    switch (event->type()) {
        case QEvent::WindowStateChange:
            // Hide cursor if the window is fullscreen, otherwise show it
            if (windowState() == Qt::WindowFullScreen)
                setCursor(Qt::BlankCursor);
            else
                unsetCursor();
            break;
        default:
            break;
    }
}

GLuint GLWidget::loadTexture(QImage image) {
    GLuint textureId;
    glGenTextures(1, &textureId); //Make room for our texture
    glBindTexture(GL_TEXTURE_2D, textureId); //Tell OpenGL which texture to edit
    //Map the image to the texture
    glTexImage2D(GL_TEXTURE_2D,                //Always GL_TEXTURE_2D
                 0,                            //0 for now
                 GL_RGBA,                       //Format OpenGL uses for image
                 image.width(), image.height(),  //Width and height
                 0,                            //The border of the image
                 GL_RGBA, //GL_RGB, because pixels are stored in RGB format
                 GL_UNSIGNED_BYTE, //GL_UNSIGNED_BYTE, because pixels are stored
                                   //as unsigned numbers
                 image.bits());               //The actual pixel data
    return textureId; //Returns the id of the texture
}

GLuint GLWidget::carregaModelo(GLMmodel* model) {
        glmUnitize(model);
        glmFacetNormals(model);
        glmVertexNormals(model, 90.0);

    glmDraw(model, GLM_SMOOTH | GLM_MATERIAL);

}
