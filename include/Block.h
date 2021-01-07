#ifndef BLOCK_H
#define BLOCK_H

#include "../include/GameObject.h"


enum BlockType
{
  None,
  Right,
  RightUp,
  RightDown,
  Left,
  LeftUp,
  LeftDown
};

int GetTypeValue(BlockType type);

BlockType GetValueType(int typeValue);

class Block : public GameObject
{
    public:
        Block(int x, int y, int block_width, int block_height);
        Block(BlockType type);
        Block(bool standing);
        Block(int block_width, int block_height);

        bool standing;
        bool hittable=false;
        bool hit=false;
        int disappearTimer=0;
        BlockType type;

        int block_width;
        int block_height;

        void Draw();
    protected:

    private:
};

#endif // BLOCK_H
