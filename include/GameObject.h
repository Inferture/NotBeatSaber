#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "Transform.h"
#include "Global.h"

/**2D objects that should be drawn on the screen*/
class GameObject
{
    public:
        GameObject();
        Transform* transform;
        bool enabled;
        void Display();
        virtual void Draw();
    protected:

    private:
};

#endif // GAMEOBJECT_H
