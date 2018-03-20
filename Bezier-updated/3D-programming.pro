QT          += core gui widgets

TEMPLATE    = app
CONFIG      += c++11

TARGET      = 3D-programming

SOURCES += main.cpp \
    matrix4x4.cpp \
    vec3.cpp \
    vec2.cpp \
    vec4.cpp \
    renderwindow.cpp \
    shader.cpp \
    mainwindow.cpp \
    vertex.cpp \
    triangle.cpp \
    octahedron.cpp \
    sceneobject.cpp \
    texture.cpp \
    axis.cpp \
    transform.cpp \
    plane.cpp \
    door.cpp \
    beziercurve.cpp \
    surface3d.cpp

HEADERS += \
    matrix4x4.h \
    vec3.h \
    vec2.h \
    vec4.h \
    renderwindow.h \
    shader.h \
    mainwindow.h \
    vertex.h \
    triangle.h \
    octahedron.h \
    sceneobject.h \
    texture.h \
    axis.h \
    transform.h \
    plane.h \
    door.h \
    beziercurve.h \
    constants.h \
    surface3d.h

FORMS += \
    mainwindow.ui

DISTFILES += \
    plainfragment.frag \
    plainvertex.vert \
    textureshader.frag \
    textureshader.vert

RESOURCES += \
    programicons.qrc
