#include"../include/Transform.h"

/**Position, rotation and scale */


Transform::Transform()
{
  x=0;
  y=0;
  z=0;
  rot=0;
  scaleX=1;
  scaleY=1;
}
Transform::Transform(int x, int y, int z)
{
  this->x=x;
  this->y=y;
  this->z=z;
  rot=0;
  scaleX=1;
  scaleY=1;
}
Transform::Transform(int x, int y,int z, float rot)
{
  this->x=x;
  this->y=y;
  this->z=z;
  this->rot=rot;
  scaleX=1;
  scaleY=1;
}
Transform::Transform(int x, int y, int z, float rot, float scaleX, float scaleY)
{
  this->x=x;
  this->y=y;
  this->z=z;
  this->rot=rot;
  this->scaleX=scaleX;
  this->scaleY=scaleY;
}

