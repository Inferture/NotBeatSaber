#include "../include/Block.h"

#include <cmath>
#include <iostream>
#include "../include/DrawingUtility.h"
#include "../include/ColorUtility.h"


using std::cout;

/**Creates a block*/

///Gets an integer from the block type (useful for serialization)
int GetTypeValue(BlockType type)
{
  switch(type)
  {
    case None:return 0;
    case Right:return 1;
    case RightUp: return 2;
    case RightDown: return 3;
    case Left:return 4;
    case LeftUp:return 5;
    case LeftDown: return 6;
  }
  return -1;
}

///Gets a block type from an integer (useful for serialization)
BlockType GetValueType(int typeValue)
{
  switch(typeValue)
  {
    case 0:return None;
    case 1:return Right;
    case 2: return RightUp;
    case 3: return RightDown;
    case 4:return Left;
    case 5:return LeftUp;
    case 6: return LeftDown;
  }
  return None;
}

/**Blocks that comes towards the player and that have to be cut*/



/**Draws the shape of the block as if it was in the origin, GameObject.display will place it correctly*/
void Block::Draw()
{
    int a = BLOCK_WIDTH;
    if(type == LeftDown || type == RightDown || type == Right)
     {
         scale(-1,-1);//Fix to compensate the fact that there is no rotation;
     }
    if(!hit)
    {
      SDL_Color currentFillColor = fillColor;
      SDL_Color currentStrokeColor = strokeColor;

      //Main rectangle
      rect(-block_width/2,-block_height/2,block_width,block_height);
      fill(MultiplyColor(currentFillColor, 1.55),fillColor.a);
      stroke(fillColor);

      //Draws the arrows
      if(type==Right || type==Left)
      {
        rect(-block_width/8,-block_height/16,block_width/4,block_height/8);
        triangle(-block_width/4,0, -block_width/8,-block_height/6,-block_width/8,block_height/6);
      }
      if(type==RightUp || type==RightDown || type == LeftUp || type==LeftDown)
      {
        rect(-block_width/16,-block_height/8,block_width/8,block_height/4);
        triangle(0,-block_height/3, -block_width/8,-block_height/8,block_width/8,-block_height/8);
      }

      fill(currentFillColor, fillColor.a);
      stroke(currentStrokeColor);
    }
    else
    {
       //Draws the rectangle sliced and the hit effect
      if(type==RightUp || type==RightDown || type == LeftUp || type==LeftDown)
      {
         float crack =block_width/20 * disappearTimer/DISAPPEAR_TIME;
         rect(-block_width/2,-block_height/2,block_width/2 - crack,block_height);
         rect(crack,-block_height/2,block_width/2-crack,block_height);

         rect(-crack/4,-block_height/2,crack/2,(block_height) * 1.5*pow((1-disappearTimer/DISAPPEAR_TIME),5));
      }
      if(type==Right|| type==Left)
      {
         float crack =block_height/20* disappearTimer/DISAPPEAR_TIME;;
         rect(-block_width/2,-block_height/2,block_width,block_height/2- crack);
         rect(-block_width/2,crack,block_width,block_height/2-crack);

         rect(-block_width/2,-crack/4,block_width* 1.5*pow((1-disappearTimer/DISAPPEAR_TIME),5),crack/2) ;
      }
    }
}

//Block initializers

Block::Block(bool standing)
{
    block_width=BLOCK_WIDTH;
    block_height=BLOCK_HEIGHT;
    this->standing=standing;
    transform = new Transform(WIDTH/2-block_width/2,HEIGHT/2-block_height/2,(int)Z0);
    enabled=true;
}

Block::Block(int block_width, int block_height)
{
    this->block_width=block_width;
    this->block_height=block_height;
    transform = new Transform(WIDTH/2-block_width/2,HEIGHT/2-block_height/2,(int)Z0);
    enabled=true;
    type=None;
}

Block::Block(int x, int y, int block_width, int block_height)
{
    this->block_width=block_width;
    this->block_height=block_height;
    transform = new Transform(x,y,(int)Z0);
    enabled=true;
}
Block::Block(BlockType type)
{
    this->type=type;
    int z = (int)Z0;
    if(mode==Saving)
    {
       z=(int)(Z_HIT_RATIO*Z0);
    }
    if(type==Right)
    {
      block_width = WIDTH/4;
      block_height=HEIGHT/2;
      transform = new Transform(WIDTH/2-block_width/2, 1, z, M_PI);
      //transform = new Transform(WIDTH/2-block_width/2, 0, z, M_PI);
    }
    else if(type==RightDown)
    {
      block_width = WIDTH/3;
      block_height=HEIGHT/4;
      transform = new Transform(WIDTH/4, HEIGHT/2-block_height/2, z, M_PI);
    }
    else if(type==RightUp)
    {
      block_width = WIDTH/3;
      block_height=HEIGHT/4;
      transform = new Transform(WIDTH/4, -HEIGHT/2+block_height/2, z);
    }
    else if(type==Left)
    {
      block_width = WIDTH/4;
      block_height=HEIGHT/2;
      transform = new Transform(-WIDTH/2+block_width/2, -1, z);
    }
    else if(type==LeftDown)
    {
      block_width = WIDTH/3;
      block_height=HEIGHT/4;
      transform = new Transform(-WIDTH/4, HEIGHT/2-block_height/2, z, M_PI);
    }
    else //(LeftUp)
    {
      block_width = WIDTH/3;
      block_height=HEIGHT/4;
      transform = new Transform(-WIDTH/4, -HEIGHT/2+block_height/2, z);
    }
    enabled=true;
}


