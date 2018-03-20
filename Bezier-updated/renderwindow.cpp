#include "renderwindow.h"
#include <QTimer>
#include <QMatrix4x4>
#include <QOpenGLContext>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QOpenGLDebugLogger>
#include <QKeyEvent>
#include <QStatusBar>
#include <QDebug>
#include <QMatrix4x4>
#include <QMouseEvent>
#include <QWheelEvent>
#include <ctime>

#include <memory>

#include "shader.h"
#include "mainwindow.h"
#include "matrix4x4.h"
#include "triangle.h"
#include "octahedron.h"
#include "texture.h"
#include "vertex.h"
#include "axis.h"
#include "transform.h"

#include "plane.h"
#include "constants.h"
#include "door.h"
#include "beziercurve.h"
#include "surface3d.h"

RenderWindow::RenderWindow(const QSurfaceFormat &format, MainWindow *mainWindow)
    : mContext{0}, mInitialized{false}, mPosition{0.005f}, mRotation{0.1f}, mMainWindow{mainWindow}

{
    setSurfaceType(QWindow::OpenGLSurface);
    setFormat(format);
    mContext = new QOpenGLContext(this);    //automatically deleted
    mContext->setFormat(requestedFormat());
    if (!mContext->create()) {
        delete mContext;
        mContext = 0;
    }
}

RenderWindow::~RenderWindow()
{
    //    glDeleteVertexArrays( 1, &mBallVAO );
    //    glDeleteBuffers( 1, &vertexBufferObject );

    //should delete all "new" objects - or use unique_ptr / shared_ptr
    //    if (mTextureShaderProgram)
    //        delete mTextureShaderProgram;
    //    if (mColorShaderProgram)
    //        delete mColorShaderProgram;

}

void RenderWindow::init()
{
    srand(time(nullptr));
    //Some general stuff
    if (!mContext->makeCurrent(this)) {
        emit error(tr("makeCurrent() failed"));
        return;
    }

    if (!mInitialized)
        mInitialized = true;

    initializeOpenGLFunctions();
    startOpenGLDebugger();

    //Initialize matrixes
    mMVPmatrix = std::unique_ptr<QMatrix4x4>(new QMatrix4x4);
    mViewMatrix = std::unique_ptr<QMatrix4x4>(new QMatrix4x4);
    mPerspectiveMatrix = std::unique_ptr<QMatrix4x4>(new QMatrix4x4);

    //perspectiv matrix
    setPerspectiveMatrix();

    //view matrix
    mViewMatrix->setToIdentity();   //identity();
    mViewMatrix->translate(0.f, -3.f, -10.f);
    mViewMatrix->lookAt(QVector3D(-11.f, 15.f, 11.f), QVector3D(0.f, 0.f, 0.f), QVector3D(0.f, 1.f, 0.f));

    //general OpenGL stuff:
    glEnable(GL_DEPTH_TEST);    //enables depth sorting - must use GL_DEPTH_BUFFER_BIT in glClear
    //    glEnable(GL_CULL_FACE);     //draws only front side of models
    glClearColor(0.2f, 0.2f, 0.2f,1.0f);    //color used in glClear GL_COLOR_BUFFER_BIT

    //Compile shaders:
    std::string vertexWithPath = gsl::shaderFilePath + "textureshader.vert";
    std::string fragmentWithPath = gsl::shaderFilePath + "textureshader.frag";
    mTextureShaderProgram = std::unique_ptr<Shader>(new Shader(vertexWithPath.c_str(), fragmentWithPath.c_str()));
    vertexWithPath = gsl::shaderFilePath + "plainvertex.vert";
    fragmentWithPath = gsl::shaderFilePath + "plainfragment.frag";
    mColorShaderProgram = std::unique_ptr<Shader>(new Shader(vertexWithPath.c_str(), fragmentWithPath.c_str()));

    //make textures
    std::string texturePath = gsl::assetFilePath + "hund.bmp";
    mTexture1 = std::unique_ptr<Texture>(new Texture(texturePath.c_str()));
    texturePath = gsl::assetFilePath + "uvtemplate.bmp";
    mTexture2 = std::unique_ptr<Texture>(new Texture(texturePath.c_str()));

    //Make Axis
    mAxis = std::unique_ptr<SceneObject>(new Axis);
    plainShaderAttribs();
    glBindVertexArray( 0 );

//    //Make beziercurve
//    mBezier = new beziercurve("controlPoints.txt");
//    plainShaderAttribs();
//    glBindVertexArray(0);

    //Make surface
    mSurface = new surface3d(-2, 3, -2, 3, 4.f);
    plainShaderAttribs();
    glBindVertexArray(0);

    std::vector<Vec3> bezierControlPoints;

    static_cast<surface3d*>(mSurface)->findTriangle(0, Vec3(0.f, 0.f, 0.f), bezierControlPoints);

    //Make beziercurve
    mBezier = new beziercurve(bezierControlPoints);
    plainShaderAttribs();
    glBindVertexArray(0);


    std::vector <Vec3> dataPoints = static_cast <beziercurve*>(mBezier)->createCurvePoints();
    for(auto const& c: dataPoints)
    {
        //std::cout << c << std::endl;
    }

    emit ready();   //tell the mainWindow that init is finished
}

void RenderWindow::render(int deltaTime)
{
    mContext->makeCurrent(this); //must be called every frame (every time mContext->swapBuffers is called)
    initializeOpenGLFunctions();

    if (mWireframe)
    {
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );    //turn on wireframe mode
    }
    else
    {
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );    //turn off wireframe mode
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    QMatrix4x4 mvpMatrix;   //for use with plainShader
    glUseProgram(mColorShaderProgram->getProgram());

    //Draw axis
    glBindVertexArray(mAxis->mVAO);
    mvpMatrix = *mPerspectiveMatrix * *mViewMatrix * *(mAxis->getModelMatrix());
    glUniformMatrix4fv( mMVPUniform, 1, GL_FALSE, mvpMatrix.constData());
    glDrawArrays(GL_LINES, 0, mAxis->mNumberOfVertices);
    checkForGLerrors();

    //Draw bezier
    glBindVertexArray(mBezier->mVAO);
    mvpMatrix = *mPerspectiveMatrix * *mViewMatrix * *(mBezier->getModelMatrix());
    glUniformMatrix4fv( mMVPUniform, 1, GL_FALSE, mvpMatrix.constData());
    glDrawArrays(GL_LINE_STRIP, 0, mBezier->mNumberOfVertices);
    checkForGLerrors();

    //Draw surface
    glBindVertexArray(mSurface->mVAO);
    mvpMatrix = *mPerspectiveMatrix * *mViewMatrix * *(mSurface->getModelMatrix());
    glUniformMatrix4fv( mMVPUniform, 1, GL_FALSE, mvpMatrix.constData());
    glDrawElements(GL_TRIANGLES, static_cast<surface3d*>(mSurface)->getNumberofIndices() , GL_UNSIGNED_INT, 0);
    checkForGLerrors();

    //Draw surface-points
    glBindVertexArray(mSurface->mVAO);
    mvpMatrix = *mPerspectiveMatrix * *mViewMatrix * *(mSurface->getModelMatrix());
    glUniformMatrix4fv( mMVPUniform, 1, GL_FALSE, mvpMatrix.constData());
    glPointSize(8);
    glDrawElements(GL_POINTS, static_cast<surface3d*>(mSurface)->getNumberofIndices() , GL_UNSIGNED_INT, 0);
    checkForGLerrors();

    mContext->swapBuffers(this);
}

void RenderWindow::plainShaderAttribs()
{
    // 1rst attribute buffer : vertices
    glVertexAttribPointer(
                0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
                3,                  // size
                GL_FLOAT,           // type
                GL_FALSE,           // normalized?
                sizeof(Vertex),  // stride
                (GLvoid*)0            // array buffer offset
                );
    glEnableVertexAttribArray(0);

    // 2nd attribute buffer : colors
    glVertexAttribPointer(
                1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
                3,                                // size
                GL_FLOAT,                         // type
                GL_FALSE,                         // normalized?
                sizeof(Vertex),            // stride
                (GLvoid*)( 3 * sizeof( GLfloat ) )   // array buffer offset
                );
    glEnableVertexAttribArray(1);
    // 3rd attribute buffer : uvs
    glVertexAttribPointer(
                2,                                // attribute. No particular reason for 2, but must match the layout in the shader.
                2,                                // size
                GL_FLOAT,                         // type
                GL_FALSE,                         // normalized?
                sizeof(Vertex),            // stride
                (GLvoid*)( 6 * sizeof( GLfloat ) )   // array buffer offset
                );
    glEnableVertexAttribArray(2);

    //enable the matrixUniform
    mMVPUniform = glGetUniformLocation( mColorShaderProgram->getProgram(), "matrix" );
}

void RenderWindow::textureShaderAttribs()
{
    // 1rst attribute buffer : vertices
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0 );
    glEnableVertexAttribArray(0);

    // 2nd attribute buffer : colors
    glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)( 3 * sizeof( GLfloat )));
    glEnableVertexAttribArray(1);

    // 3rd attribute buffer : uvs
    glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)( 6 * sizeof( GLfloat )));
    glEnableVertexAttribArray(2);

    //enable the matrixUniform
    mModelMatrixUniform = glGetUniformLocation( mTextureShaderProgram->getProgram(), "mMatrix" );
    mViewMatrixUniform = glGetUniformLocation( mTextureShaderProgram->getProgram(), "vMatrix" );
    mPerspectiveMatrixUniform = glGetUniformLocation( mTextureShaderProgram->getProgram(), "pMatrix" );

    mTextureUniform1 = glGetUniformLocation(mTextureShaderProgram->getProgram(), "textureSampler1");
    mTextureUniform2 = glGetUniformLocation(mTextureShaderProgram->getProgram(), "textureSampler2");

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mTexture1->id());
}

void RenderWindow::startOpenGLDebugger()
{
    QOpenGLContext * temp = this->context();
    if (temp)
    {
        QSurfaceFormat format = temp->format();
        qDebug() << "Can this system run QOpenGLDebugLogger? :" << format.testOption(QSurfaceFormat::DebugContext);

        if(temp->hasExtension(QByteArrayLiteral("GL_KHR_debug")))
        {
            qDebug() << "System can log OpenGL errors!";
            mOpenGLDebugLogger = new QOpenGLDebugLogger(this);  //will be deleted automatically
            if (mOpenGLDebugLogger->initialize()) // initializes in the current context
                qDebug() << "Started OpenGL debug logger!";
            //Try to silence some messages that is not critical
            mOpenGLDebugLogger->disableMessages(QOpenGLDebugMessage::AnySource, QOpenGLDebugMessage::AnyType, QOpenGLDebugMessage::NotificationSeverity);
        }
    }
}

void RenderWindow::setWireframe(bool wireframe)
{
    mWireframe = wireframe;
}

void RenderWindow::checkForGLerrors()
{
    if(mOpenGLDebugLogger)
    {
        const QList<QOpenGLDebugMessage> messages = mOpenGLDebugLogger->loggedMessages();
        for (const QOpenGLDebugMessage &message : messages)
            qDebug() << message;
    }
    else
    {
        GLenum err = GL_NO_ERROR;
        while((err = glGetError()) != GL_NO_ERROR)
        {
            qDebug() << "glGetError returns " << err;
        }
    }
}

//This function is called from Qt when window is exposed / shown
void RenderWindow::exposeEvent(QExposeEvent *)
{
    if (!mInitialized)
        init();

    const qreal retinaScale = devicePixelRatio();
    glViewport(0, 0, width() * retinaScale, height() * retinaScale);
    setPerspectiveMatrix();

    if (isExposed())
    {
        mTimer.start(16, this);
        mTimeStart.start();
    }
}

void RenderWindow::mouseMoveEvent(QMouseEvent *event)
{
    //using mouseXYlast as deltaXY so we don't need extra variables
    mouseXlast = event->pos().x() - mouseXlast;
    mouseYlast = event->pos().y() - mouseYlast;

    //if delta is to big the movement will be jerky
    //Happens if mouse is moved much between presses.
    if (mouseXlast > 40 || mouseYlast > 40 || mouseXlast < -40 || mouseYlast < -40)
    {
        mouseXlast = 0;
        mouseYlast = 0;
    }

    // Rotate camera
    float cameraSpeed{0.05f};

    mViewMatrix->rotate(mouseXlast*cameraSpeed, 0.f, 1.f, 0.f);
    mViewMatrix->rotate(mouseYlast*cameraSpeed, 1.f, 0.f, 0.f);

    //    mCurrentCamera->mTransform->rotate(0.0f, -cameraRotationSpeed*mouseXlast, 0.0f);
    //    mCurrentCamera->mTransform->rotate(-cameraRotationSpeed*mouseYlast, 0.0f, 0.0f);

    mouseXlast = event->pos().x();
    mouseYlast = event->pos().y();
}

void RenderWindow::setPerspectiveMatrix()
{
    GLfloat nearPlane = 0.1f;
    GLfloat farPlane = 1000.f;
    mPerspectiveMatrix->setToIdentity();
    //mOrtho = true;
    if (mOrtho)
    {
        float scale{0.005f};
        mPerspectiveMatrix->ortho(-width()*scale/2, width()*scale/2, -height()*scale/2 , height()*scale/2, nearPlane, farPlane);
    }
    else
    {
        GLfloat aspectRatio = static_cast<float>(width())/static_cast<float>(height() ?height() : 1); //height never == 0
        GLfloat fieldOfView{45.f};
        mPerspectiveMatrix->perspective(fieldOfView, aspectRatio, nearPlane, farPlane);
    }
}

void RenderWindow::mousePressEvent(QMouseEvent *event)
{}

void RenderWindow::wheelEvent(QWheelEvent* event)
{
    //http://doc.qt.io/qt-4.8/qwheelevent.html
    int numDegrees = event->delta() / 8;
    float numSteps = numDegrees / 15;

    if (event->orientation() == Qt::Horizontal) {
        mViewMatrix->translate(numSteps, 0.0, 0.0);
    } else {
        mViewMatrix->translate(0.0, 0.0, numSteps);
    }
    event->accept();
}

void RenderWindow::keyPressEvent(QKeyEvent *event)
{
    if(event->isAutoRepeat())
    {
        event->ignore();
    }
    if(event->key() == Qt::Key_A)
    {
        moveUnitLeft = true;
    }
    if(event->key() == Qt::Key_S)
    {
        moveUnitBackward = true;
    }
    if(event->key() == Qt::Key_D)
    {
        moveUnitRight = true;
    }
    if(event->key() == Qt::Key_W)
    {
        moveUnitForward = true;
    }
    if(event->key() == Qt::Key_Escape)
    {
        mMainWindow->close();
    }
    if(event->key() == Qt::Key_Space)
    {
        mSceneObject[12]->setForce(Vec3(rand()%10, 0.f, rand()%10));
        mSceneObject[13]->setForce(Vec3(rand()%10, 0.f, rand()%10));
    }

}

void RenderWindow::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_A)
    {
        moveUnitLeft = false;
    }
    if(event->key() == Qt::Key_S)
    {
        moveUnitBackward = false;
    }
    if(event->key() == Qt::Key_D)
    {
        moveUnitRight = false;
    }
    if(event->key() == Qt::Key_W)
    {
        moveUnitForward = false;
    }

}

void RenderWindow::playerMovement()
{

    if(moveUnitForward)
    {
        mSceneObject[14]->setForce(mSceneObject[14]->getForce() + Vec3(0.1f, 0.f, -0.1f));

    }
    if(moveUnitBackward)
    {
        mSceneObject[14]->setForce(mSceneObject[14]->getForce() + Vec3(-0.1f, 0.f, 0.1f));

    }
    if(moveUnitLeft)
    {
        mSceneObject[14]->setForce(mSceneObject[14]->getForce() + Vec3(-0.1f, 0.f, -0.1f));

    }
    if(moveUnitRight)
    {
        mSceneObject[14]->setForce(mSceneObject[14]->getForce() + Vec3(0.1f, 0.f, 0.1f));

    }
    mSceneObject[14]->setVelocity(mSceneObject[14]->getVelocity() * 0.9f);
}

void RenderWindow::timerEvent(QTimerEvent *)
{
    int msSinceLastFrame = mTimeStart.restart();    //restart() returns ms since last restart.
    render(msSinceLastFrame);

    //The rest here is just to show the frame rate:

    static int frameCount{0};                       //counting actual frames for a quick "timer" for the statusbar

    if (mMainWindow)    //if no mainWindow, something is really wrong...
    {
        ++frameCount;
        if (frameCount > 60) //once pr 60 frames =  update the message once pr second
        {
            //showing some statistics in status bar
            mMainWindow->statusBar()->showMessage(" FrameDraw: " + QString::number(msSinceLastFrame) + " ms");
            frameCount = 0;     //reset to show a new message in 60 frames
        }
    }
}



