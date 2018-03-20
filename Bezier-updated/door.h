#ifndef DOOR_H
#define DOOR_H

#include "sceneobject.h"
#include "vertex.h"
#include "vec3.h"
#include "matrix4x4.h"
#include <QMatrix4x4>


class Vertex;
class Matrix4x4;

class Door : public SceneObject
{
public:
    Door();
    ~Door();
    void createObject() override;
    void drawObject() override;

    //void interactDoor();
};

#endif // DOOR_H
