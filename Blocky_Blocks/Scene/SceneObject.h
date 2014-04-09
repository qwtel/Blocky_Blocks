#pragma once
class SceneObject
{
public:
    SceneObject(void);
    virtual ~SceneObject(void);

    virtual void update(double deltaT);
    virtual void draw();
};

