#pragma once

#include "Object.h"
#include "Map.h"

#ifndef CHECKPOINT_H
#define CHECKPOINT_H
const float WIDTH = 9.0f;
const float CHECKPOINT_OFFSET = 3.0f;

#endif // !CHECKPOINT_H

class Map;

class Checkpoint : public Object
{
private:
    MyMesh mesh;
    Map* map;
    bool reached;

    virtual void addMesh();
    void updateMesh();
public:
    Checkpoint::Checkpoint(float x, float z, float angle, Map* map);

    virtual void setReached();

    void setNotReached();

    bool isReached() { return reached; }

    void handleCollision();

    bool hasReachedAllCheckpoints();

    bool hasReachedAllCheckpointsExceptGoal();

    MyMesh* getMesh() { return &mesh; }
};

class Goal : public Checkpoint
{
private:
    void setReached();
    virtual void addMesh();
public:
    Goal::Goal(float x, float z, float angle, Map* map);

};

