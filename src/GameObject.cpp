#include "../include/GameObject.h"

#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "../include/DrawingUtility.h"

using std::cout;
using std::pow;

/**Displays the GameObject on the screen, according to its transform*/
void GameObject::Display()
{

    enabled=true;
    if(enabled)
    {

     pushMatrix();
     int tx = abs(transform->x);
     int ty = abs(transform->y);

     int sgnx=1;
     int sgny=1;
     if(transform->x<0)
     {
       sgnx=-1;
     }
     if(transform->y<0)
     {
       sgny=-1;
     }
     float ratio =transform->z/Z0;



     translate(
     sgnx*pow(tx,(1-ratio)) *
     pow(tx*SCALE_HORIZON, (ratio)),
     sgny*pow(ty ,(1-ratio))*
     pow(ty*SCALE_HORIZON ,(ratio))
     );

     //rotate(transform.rot);//Rotation not available in the SDL version

     scale(
     transform->scaleX *(pow(SCALE_HORIZON,ratio)),
     transform->scaleY *(pow(SCALE_HORIZON,ratio)));


     Draw();

     popMatrix();
    }
}

GameObject::GameObject()
{
}

/**Should be overwritten*/
void GameObject::Draw()
{
    //cout<<"\You shouldn't be here !\n";
}

