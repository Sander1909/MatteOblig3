#ifndef RENDERWINDOW_H
#define RENDERWINDOW_H

#include <QWindow>
#include <QOpenGLFunctions_4_1_Core>
#include <QBasicTimer>
#include <QTime>



#include <memory>
#include <vector>

class QOpenGLContext;
class Shader;
class MainWindow;
class Triangle;
class Octahedron;
class SceneObject;
class Texture;
class Matrix4x4;
class QMatrix4x4;
class Door;
class Surface3D;

class RenderWindow : public QWindow, protected QOpenGLFunctions_4_1_Core
{
    Q_OBJECT
public:
    RenderWindow(const QSurfaceFormat &format, MainWindow *mainWindow);
    ~RenderWindow();
    QOpenGLContext *context() { return mContext; }
    void exposeEvent(QExposeEvent *) override;

    void setWireframe(bool wireframe);

    void playerMovement();

signals:
    void ready();
    void error(const QString &msg);

private slots:
    void render(int deltaTime);

private:
    void init();

    void startOpenGLDebugger();

    QOpenGLContext *mContext;
    bool mInitialized;
    std::unique_ptr<Shader> mTextureShaderProgram;
    std::unique_ptr<Shader> mColorShaderProgram;

    GLint mModelMatrixUniform;
    GLint mViewMatrixUniform;
    GLint mPerspectiveMatrixUniform;

    GLint mMVPUniform;

    GLuint mBallVAO;
    GLuint mTriangleVAO;
    GLuint mAxisVAO;

    float mPosition;
    float mRotation;

    std::vector<std::unique_ptr<SceneObject>> mSceneObject;
    std::unique_ptr<SceneObject> mDoor;


    std::unique_ptr<SceneObject> mOctahedron1;
    std::unique_ptr<SceneObject> mOctahedron2;
    std::unique_ptr<SceneObject> mAxis;
    SceneObject* mBezier;
    SceneObject* mSurface;

    QBasicTimer mTimer;     //timer that drives the gameloop
    QTime mTimeStart;       //time variable that reads the actual FPS

    MainWindow *mMainWindow;

    class QOpenGLDebugLogger *mOpenGLDebugLogger{nullptr};

    std::unique_ptr<Texture> mTexture1;
    GLuint mTextureUniform1;

    std::unique_ptr<Texture> mTexture2;
    GLuint mTextureUniform2;

    std::unique_ptr<QMatrix4x4> mViewMatrix;
    std::unique_ptr<QMatrix4x4> mPerspectiveMatrix;
    std::unique_ptr<QMatrix4x4> mMVPmatrix;

    bool mOrtho{false};
    bool mWireframe{false};
    int mouseXlast{0};
    int mouseYlast{0};

    int renderLoop{0};

    void checkForGLerrors();

    void setPerspectiveMatrix();

    void plainShaderAttribs();

    void textureShaderAttribs();

    //character movement
    float moveSpeed{};
    bool moveUnitForward{false};
    bool moveUnitBackward{false};
    bool moveUnitRight{false};
    bool moveUnitLeft{false};
    //-----------------------

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

    void timerEvent(QTimerEvent *) override;
};

#endif // RENDERWINDOW_H
