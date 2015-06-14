/****************************************************************************
**
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia. For licensing terms and
** conditions see http://qt.digia.com/licensing. For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights. These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QDebug>
#include "scene.h"
#include <QtGui/qmatrix4x4.h>
#include <QtGui/qvector3d.h>

#include "3rdparty/fbm.h"

void checkGLErrors(const QString& prefix)
{
    switch (glGetError()) {
    case GL_NO_ERROR:
        //qDebug() << prefix << tr("No error.");
        break;
    case GL_INVALID_ENUM:
        qDebug() << prefix << QObject::tr("Invalid enum.");
        break;
    case GL_INVALID_VALUE:
        qDebug() << prefix << QObject::tr("Invalid value.");
        break;
    case GL_INVALID_OPERATION:
        qDebug() << prefix << QObject::tr("Invalid operation.");
        break;
    case GL_STACK_OVERFLOW:
        qDebug() << prefix << QObject::tr("Stack overflow.");
        break;
    case GL_STACK_UNDERFLOW:
        qDebug() << prefix << QObject::tr("Stack underflow.");
        break;
    case GL_OUT_OF_MEMORY:
        qDebug() << prefix << QObject::tr("Out of memory.");
        break;
    default:
        qDebug() << prefix << QObject::tr("Unknown error.");
        break;
    }
}



//============================================================================//
//                                    Scene                                   //
//============================================================================//

const static char environmentShaderText[] =
    "uniform samplerCube env;"
    "void main() {"
        "gl_FragColor = textureCube(env, gl_TexCoord[1].xyz);"
    "}";

Scene::Scene(int width, int height, int maxTextureSize)
    : m_distExp(600)
    , m_frame(0)
    , m_maxTextureSize(maxTextureSize)
    , m_currentShader(0)
    , m_currentTexture(0)
    , m_box(0)
    , m_vertexShader(0)
    , m_environmentShader(0)
    , m_environmentProgram(0)
{
    setSceneRect(0, 0, width, height);

    m_TrackBallModels = TrackBall(0.05f, QVector3D(0, 1, 0), TrackBall::Sphere); // model
    m_TrackBallDrag = TrackBall(0.005f, QVector3D(0, 0, 1), TrackBall::Drag);
    m_TrackBallCamera = TrackBall(0.0f, QVector3D(0, 1, 0), TrackBall::Plane); // camera

    initGL();

    m_timer = new QTimer(this);
    m_timer->setInterval(20);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(update()));
    m_timer->start();

    m_time.start();
}

Scene::~Scene()
{
    if (m_box)
        delete m_box;
    foreach (GLTexture *texture, m_textures)
        if (texture) delete texture;

    foreach (QGLShaderProgram *program, m_programs)
        if (program) delete program;
    if (m_vertexShader)
        delete m_vertexShader;
    foreach (QGLShader *shader, m_fragmentShaders)
        if (shader) delete shader;

    if (m_environmentShader)
        delete m_environmentShader;
    if (m_environmentProgram)
        delete m_environmentProgram;
}

void Scene::initGL()
{
    m_box = new GLRoundedBox(0.25f, 1.0f, 10);

    m_vertexShader = new QGLShader(QGLShader::Vertex);
    m_vertexShader->compileSourceFile(QLatin1String(":/res/boxes/basic.vsh"));

    QStringList list;
    list << ":/res/boxes/cubemap_posx.jpg" << ":/res/boxes/cubemap_negx.jpg" << ":/res/boxes/cubemap_posy.jpg"
         << ":/res/boxes/cubemap_negy.jpg" << ":/res/boxes/cubemap_posz.jpg" << ":/res/boxes/cubemap_negz.jpg";
    m_environment = new GLTextureCube(list, qMin(1024, m_maxTextureSize));
    m_environmentShader = new QGLShader(QGLShader::Fragment);
    m_environmentShader->compileSourceCode(environmentShaderText);
    m_environmentProgram = new QGLShaderProgram;
    m_environmentProgram->addShader(m_vertexShader);
    m_environmentProgram->addShader(m_environmentShader);
    m_environmentProgram->link();

    QStringList filter;
    QList<QFileInfo> files;

    // Load all .png files as textures
    m_currentTexture = 0;
    filter = QStringList("*.png");
    files = QDir(":/res/boxes/").entryInfoList(filter, QDir::Files | QDir::Readable);

    foreach (QFileInfo file, files) {
        GLTexture *texture = new GLTexture2D(file.absoluteFilePath(), qMin(256, m_maxTextureSize), qMin(256, m_maxTextureSize));
        if (texture->failed()) {
            delete texture;
            continue;
        }
        m_textures << texture;

    }

    if (m_textures.size() == 0)
        m_textures << new GLTexture2D(qMin(64, m_maxTextureSize), qMin(64, m_maxTextureSize));

    // Load all .fsh files as fragment shaders
    m_currentShader = 0;
    filter = QStringList("*.fsh");
    files = QDir(":/res/boxes/").entryInfoList(filter, QDir::Files | QDir::Readable);
    foreach (QFileInfo file, files) {
        QGLShaderProgram *program = new QGLShaderProgram;
        QGLShader* shader = new QGLShader(QGLShader::Fragment);
        shader->compileSourceFile(file.absoluteFilePath());
        // The program does not take ownership over the shaders, so store them in a vector so they can be deleted afterwards.
        program->addShader(m_vertexShader);
        program->addShader(shader);
        if (!program->link()) {
            qWarning("Failed to compile and link shader program");
            qWarning("Vertex shader log:");
            qWarning() << m_vertexShader->log();
            qWarning() << "Fragment shader log ( file =" << file.absoluteFilePath() << "):";
            qWarning() << shader->log();
            qWarning("Shader program log:");
            qWarning() << program->log();

            delete shader;
            delete program;
            continue;
        }

        m_fragmentShaders << shader;
        m_programs << program;
    }

    if (m_programs.size() == 0)
        m_programs << new QGLShaderProgram;
}

static void loadMatrix(const QMatrix4x4& m)
{
    // static to prevent glLoadMatrixf to fail on certain drivers
    static GLfloat mat[16];
    const float *data = m.constData();
    for (int index = 0; index < 16; ++index)
        mat[index] = data[index];
    glLoadMatrixf(mat);
}

// If one of the boxes should not be rendered, set excludeBox to its index.
// If the main box should not be rendered, set excludeBox to -1.
void Scene::renderBoxes(const QMatrix4x4 &view, int excludeBox)
{
    QMatrix4x4 invView = view.inverted();

    // If multi-texturing is supported, use three saplers.
    if (glActiveTexture) {
        glActiveTexture(GL_TEXTURE0);
        m_textures[m_currentTexture]->bind();

        glActiveTexture(GL_TEXTURE1);
    } else {
        m_textures[m_currentTexture]->bind();
    }

    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);

    QMatrix4x4 viewRotation(view);
    viewRotation(3, 0) = viewRotation(3, 1) = viewRotation(3, 2) = 0.0f;
    viewRotation(0, 3) = viewRotation(1, 3) = viewRotation(2, 3) = 0.0f;
    viewRotation(3, 3) = 1.0f;
    loadMatrix(viewRotation);
    glScalef(20.0f, 20.0f, 20.0f);

    // Don't render the environment if the environment texture can't be set for the correct sampler.
    if (glActiveTexture) {
        m_environment->bind();
        m_environmentProgram->bind();
        m_environmentProgram->setUniformValue("tex", GLint(0));
        m_environmentProgram->setUniformValue("env", GLint(1));
        m_environmentProgram->setUniformValue("noise", GLint(2));
        m_box->draw();
        m_environmentProgram->release();
        m_environment->unbind();
    }

    loadMatrix(view);

    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);

    if (-1 != excludeBox) {
        QMatrix4x4 m;

        m.rotate(m_TrackBallModels.rotation());

        glMultMatrixf(m.constData());

        if (glActiveTexture) {

                m_environment->bind();
        }

        m_programs[m_currentShader]->bind();
        m_programs[m_currentShader]->setUniformValue("tex", GLint(0));
        m_programs[m_currentShader]->setUniformValue("env", GLint(1));
        m_programs[m_currentShader]->setUniformValue("noise", GLint(2));
        m_programs[m_currentShader]->setUniformValue("view", view);
        m_programs[m_currentShader]->setUniformValue("invView", invView);
        m_box->draw();
        m_programs[m_currentShader]->release();

        if (glActiveTexture) {
            m_environment->unbind();
        }
    }

    if (glActiveTexture) {
        glActiveTexture(GL_TEXTURE0);
    }
    m_textures[m_currentTexture]->unbind();
}

void Scene::setStates()
{
    //glClearColor(0.25f, 0.25f, 0.5f, 1.0f);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
    //glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_NORMALIZE);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    setLights();

    float materialSpecular[] = {0.5f, 0.5f, 0.5f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, materialSpecular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 32.0f);
}

void Scene::setLights()
{
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    //float lightColour[] = {1.0f, 1.0f, 1.0f, 1.0f};
    float lightDir[] = {0.0f, 0.0f, 1.0f, 0.0f};
    //glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColour);
    //glLightfv(GL_LIGHT0, GL_SPECULAR, lightColour);
    glLightfv(GL_LIGHT0, GL_POSITION, lightDir);
    glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, 1.0f);
    glEnable(GL_LIGHT0);
}

void Scene::defaultStates()
{
    //glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_LIGHTING);
    //glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHT0);
    glDisable(GL_NORMALIZE);

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, 0.0f);
    float defaultMaterialSpecular[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, defaultMaterialSpecular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0.0f);
}

void Scene::drawBackground(QPainter *painter, const QRectF &)
{
    float width = float(painter->device()->width());
    float height = float(painter->device()->height());

    painter->beginNativePainting();
    setStates();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    qgluPerspective(60.0, width / height, 0.01, 15.0);

    glMatrixMode(GL_MODELVIEW);

    QMatrix4x4 view;

    view.translate(m_TrackBallDrag.GetDragPos());

    view.rotate(m_TrackBallCamera.rotation());

    view(2, 3) -= 2.0f * exp(m_distExp / 1200.0f);

    renderBoxes(view);

    defaultStates();
    ++m_frame;

    painter->endNativePainting();
}

QPointF Scene::pixelPosToViewPos(const QPointF& p)
{
    return QPointF(2.0 * float(p.x()) / width() - 1.0,
                   1.0 - 2.0 * float(p.y()) / height());
}

void Scene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mouseMoveEvent(event);
    if (event->isAccepted())
        return;

    if (event->buttons() & Qt::LeftButton) {
        m_TrackBallModels.move(pixelPosToViewPos(event->scenePos()), m_TrackBallCamera.rotation().conjugate());
        event->accept();
    } else {
        m_TrackBallModels.release(pixelPosToViewPos(event->scenePos()), m_TrackBallCamera.rotation().conjugate());
    }

    if (event->buttons() & Qt::RightButton) {
        m_TrackBallDrag.move(pixelPosToViewPos(event->scenePos()), m_TrackBallCamera.rotation().conjugate());
        event->accept();
    } else {
        m_TrackBallDrag.release(pixelPosToViewPos(event->scenePos()), m_TrackBallCamera.rotation().conjugate());
    }

    if (event->buttons() & Qt::MidButton) {
        m_TrackBallCamera.move(pixelPosToViewPos(event->scenePos()), QQuaternion());
        event->accept();
    } else {
        m_TrackBallCamera.release(pixelPosToViewPos(event->scenePos()), QQuaternion());
    }
}

void Scene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mousePressEvent(event);
    if (event->isAccepted())
        return;

    if (event->buttons() & Qt::LeftButton) {
        m_TrackBallModels.push(pixelPosToViewPos(event->scenePos()), m_TrackBallCamera.rotation().conjugate());
        event->accept();
    }

    if (event->buttons() & Qt::RightButton) {
        m_TrackBallDrag.push(pixelPosToViewPos(event->scenePos()), m_TrackBallCamera.rotation().conjugate());
        event->accept();
    }

    if (event->buttons() & Qt::MidButton) {
        m_TrackBallCamera.push(pixelPosToViewPos(event->scenePos()), QQuaternion());
        event->accept();
    }
}

void Scene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mouseReleaseEvent(event);
    if (event->isAccepted())
        return;

    if (event->button() == Qt::LeftButton) {
        m_TrackBallModels.release(pixelPosToViewPos(event->scenePos()), m_TrackBallCamera.rotation().conjugate());
        event->accept();
    }

    if (event->button() == Qt::RightButton) {
        m_TrackBallDrag.release(pixelPosToViewPos(event->scenePos()), m_TrackBallCamera.rotation().conjugate());
        event->accept();
    }

    if (event->button() == Qt::MidButton) {
        m_TrackBallCamera.release(pixelPosToViewPos(event->scenePos()), QQuaternion());
        event->accept();
    }
}

void Scene::wheelEvent(QGraphicsSceneWheelEvent * event)
{
    QGraphicsScene::wheelEvent(event);
    if (!event->isAccepted()) {
        m_distExp += event->delta();
        if (m_distExp < -8 * 120)
            m_distExp = -8 * 120;
        if (m_distExp > 10 * 120)
            m_distExp = 10 * 120;
        event->accept();
    }
}

void Scene::setShader(int index)
{
    if (index >= 0 && index < m_fragmentShaders.size())
        m_currentShader = index;
}

void Scene::setTexture(int index)
{
    if (index >= 0 && index < m_textures.size())
        m_currentTexture = index;
}




