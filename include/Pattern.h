#ifndef PATTERN_H
#define PATTERN_H

#include <string>
#include <vector>
#include "Block.h"


using std::string;
using std::vector;


struct BlockSpawn
{
   BlockType type;
   int time;
};



class Pattern
{
    public:
        static Pattern* Deserialize(string name);
        void Add(BlockType type, int time);
        void Add(BlockSpawn spawn);
        void Serialize();

        Pattern(string name);
        Pattern(string name, string musicName, string artistName);

        string name;
        string musicName;
        string artistName;
        vector<BlockSpawn> spawns;

    protected:

    private:
};

#endif // PATTERN_H
