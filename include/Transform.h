#ifndef TRANSFORM_H
#define TRANSFORM_H


class Transform
{
    public:
        Transform();
        Transform(int x, int y, int z);
        Transform(int x, int y,int z, float rot);
        Transform(int x, int y, int z, float rot, float scaleX, float scaleY);


        int x;
        int y;
        int z;
        float rot;
        float scaleX;
        float scaleY;
    protected:
    private:
};

#endif // TRANSFORM_H
