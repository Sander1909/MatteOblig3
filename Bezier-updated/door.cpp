#include "door.h"
#include "vec3.h"
#include "vec2.h"
#include "vertex.h"

Door::Door()
{
    createObject();
}

Door::~Door()
{
    delete[] mVertices;
}

void Door::createObject()
{
    mNumberOfVertices = 6;

    mVertices = new Vertex[mNumberOfVertices]
    {
        // Positions                // Colors                       // UVs
        {Vec3{0.f, 0.f, 0.0f},      Vec3{0.59f, 0.44f, 0.28f},      Vec2{0.f, 0.f}},
        {Vec3{0.f, 1.f, 0.0f},      Vec3{0.59f, 0.44f, 0.28f},      Vec2{0.f, 0.f}},
        {Vec3{1.f, 1.f, 0.0f},      Vec3{0.59f, 0.44f, 0.28f},      Vec2{0.f, 0.f}},

        {Vec3{1.f, 0.f, 0.0f},      Vec3{0.59f, 0.44f, 0.28f},      Vec2{0.f, 0.f}},
        {Vec3{0.f, 0.f, 0.0f},      Vec3{0.59f, 0.44f, 0.28f},      Vec2{0.f, 0.f}},
        {Vec3{1.f, 1.f, 0.0f},      Vec3{0.59f, 0.44f, 0.28f},      Vec2{0.f, 0.f}}
    };

    initializeOpenGLFunctions();

    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    glGenBuffers(1, &mBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, mBufferObject);
    glBufferData(GL_ARRAY_BUFFER, mNumberOfVertices * sizeof( GLfloat ) * sizeof(Vertex), mVertices, GL_STATIC_DRAW);
}

void Door::drawObject()
{

}

/*void Door::interactDoor()
{
    doorOpen = !doorOpen;

    if(doorOpen)
        getTransform()->getRotation().setY(-45.f);
    else
        getTransform()->getRotation.setY(getTransform()->getRotation().getY() - 90.f);
}*/

