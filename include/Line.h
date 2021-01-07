#ifndef LINE_H
#define LINE_H

#include "../include/GameObject.h"


class Line : public GameObject
{
    public:

        Line(int direction);
        int direction;//0:up, 1:left, 2:down, 3:right
        void Draw();

    protected:

    private:
};

#endif // LINE_H
