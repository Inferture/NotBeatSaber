#include "../include/Pattern.h"

#include <vector>
#include <string>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>

using std::cout;
using std::ofstream;
using std::ifstream;
using std::stringstream;
using std::find;
using std::endl;


//Things to add
/*Speed ?*/
/*Beat ? To associate with speed to get line frequency*/



Pattern::Pattern(string name) : spawns(0)
{
    this->name=name;
    this->musicName=name;
    this->artistName="";
}

Pattern::Pattern(string name, string musicName, string artistName): spawns(0)
{
    this->name=name;
    this->musicName=musicName;
    this->artistName=artistName;
}

void Pattern::Add(BlockType type, int time)
{
    spawns.push_back({type, time});
}
void Pattern::Add(BlockSpawn spawn)
{
    spawns.push_back(spawn);
}


///Creates a pattern from the *name*.pattern file (should be in the MAIN_FOLDER folder)
Pattern* Pattern::Deserialize(string name)
{
     try
      {
        char sep = '/';

        stringstream pathstream;
        pathstream<<MAIN_FOLDER<<sep<<name<<".pattern";
        string path = pathstream.str();
        ifstream fileStream(path.c_str());


        string musicName = name;
        getline(fileStream, name);

        string artistName;
        getline(fileStream, artistName);

        string blockSpawnString;

        Pattern* pattern = new Pattern(name, musicName,artistName);
        while(getline(fileStream, blockSpawnString))
        {
            int endIndex = blockSpawnString.size()-1;
            int slashIndex = blockSpawnString.size()-2;
            string typeString = blockSpawnString.substr(endIndex,1);
            string timeString = blockSpawnString.substr(0,slashIndex);

            int typeNumber = atoi(typeString.c_str());

            BlockType type = GetValueType(typeNumber);
            int time = atoi(timeString.c_str());
            pattern->Add(type, time);
        }
        return pattern;

      }
    catch(string e)
    {
        string message = "problem when trying to deserialize : ";
        message +=e;
        cout<<message;
    }

    return new Pattern("");
}


///Serializes a pattern in a *name*.pattern file (see structure below)
void Pattern::Serialize()
{
    char sep = '/';

    stringstream pathstream;
    pathstream<<MAIN_FOLDER<<sep<<name<<".pattern";
    string path = pathstream.str();
    ofstream fileStream(path.c_str());

    if(fileStream)
    {
        fileStream<<name<<endl<<artistName<<endl;
        for (int i = 0; i < spawns.size(); i++)
        {
            BlockSpawn spawn = spawns[i];
            fileStream<<spawn.time<<"/"<<GetTypeValue(spawn.type)<<endl;
        }
    }
    else
    {
        cout << "error: could not open the file.\n";
    }
}

/*//File structure ([comment not included])

MeltyBlood - Kara no Kyoukai  [Music name]
Yuki Kajiura [Artist name]
786/1 [time in millisecond / type of the block to spawn, the spawns must be listed in order of time]
1065/4
1352/3
1680/6
2056/1
2456/4
2876/3
3013/6
3253/2
3622/5
4047/1
4351/4
4490/3
4790/6
5156/1
...
109720/3
*/



