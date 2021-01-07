#include <exception>
#include <string>
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <sstream>
#include <vector>
#include <dirent.h>
#include <algorithm>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "../include/Transform.h"
#include "../include/Global.h"
#include "../include/Pattern.h"
#include "../include/Line.h"
#include "../include/Block.h"
#include "../include/ColorUtility.h"
#include "../include/DrawingUtility.h"




using std::vector;
using std::cout;
using std::cin;
using std::min;
using std::max;
using std::stringstream;

//using namespace std;


//Graphics -
//Push/Pop matrix -
//SeriliazePattern -
//Fonts (SDL_ttf)
//Sounds (SDL_mixer)
//Triangles (SDL_gfx)


/*
void SDL::draw()
{
    // Clear the window with a black background
    SDL_SetRenderDrawColor( m_renderer, 0, 0, 0, 255 );
    SDL_RenderClear( m_renderer );

    // Show the window
    SDL_RenderPresent( m_renderer );

    int rgb[] = { 203, 203, 203, // Gray
                  254, 254,  31, // Yellow
                    0, 255, 255, // Cyan
                    0, 254,  30, // Green
                  255,  16, 253, // Magenta
                  253,   3,   2, // Red
                   18,  14, 252, // Blue
                    0,   0,   0  // Black
                };

    SDL_Rect colorBar;
    colorBar.x = 0; colorBar.y = 0; colorBar.w = 90; colorBar.h = 480;

    // Render a new color bar every 0.5 seconds
    for ( int i = 0; i != sizeof rgb / sizeof *rgb; i += 3, colorBar.x += 90 )
    {
        SDL_SetRenderDrawColor( m_renderer, rgb[i], rgb[i + 1], rgb[i + 2], 255 );
        SDL_RenderFillRect( m_renderer, &colorBar );
        SDL_RenderPresent( m_renderer );
        SDL_Delay( 500 );
    }
}
*/

//2

vector<Line*> lines;
vector<Block*> blocks;
vector<Block*> hittableBlocks;

//0:up, 1:left, 2:down, 3:right
Line* limitDown = new Line(2);
Line* limitUp = new Line(0);
Line* limitLeft = new Line(1);
Line* limitRight = new Line(3);

//Block* limitFar = new Block(-WIDTH/2, -HEIGHT/2,WIDTH, HEIGHT);//-WIDTH/2, -HEIGHT/2,
//Block* limit = new Block(-WIDTH/2, -HEIGHT/2,WIDTH, HEIGHT);
//Block* limitClose = new Block(-WIDTH/2, -HEIGHT/2,WIDTH, HEIGHT);
Block* limitFar = new Block(WIDTH, HEIGHT);//-WIDTH/2, -HEIGHT/2,
Block* limit = new Block(WIDTH, HEIGHT);
Block* limitClose = new Block(WIDTH, HEIGHT);

//
Pattern* pattern;

int currentSpawn=0;
int startTime;
int lastFrameTime;
int deltaTime=0;
SDL_Color targetColor;
bool fastTransformation;

/*
SoundFile musicFile;
SoundFile cut;

*/
Mix_Music *musicSound;
Mix_Chunk *cut;


vector<string> musics(0);
int currentMusic;

char sep = '/';

/*
PFont displayFont;
PFont displayFontLarge;
*/
TTF_Font *displayFont;
TTF_Font *displayFontLarge;

//Motion controls
int last_acc_right_x;
int last_acc_right_y;
int last_acc_left_x;
int last_acc_left_y;

int acc_right_x;
int acc_right_y;
int acc_left_x;
int acc_left_y;



int ACCELERATION_TRESHOLD=50;



void Display();
void getAccelerometerValues();
void accelerometerAction(int, int, int, int);
void Ditch();
void Rewind(int timeMs);



bool finish=false;


int16_t lastLeftTriggerValue=0;

SDL_GameController *ControllerHandles[4];

int millis()
{
    return SDL_GetTicks();
}


void setup()
{
    SCALE_HORIZON=(float)HORIZON_WIDTH/(float)WIDTH;
    MAIN_FOLDER="./data";
    LINE_PERIOD=(int) (1000*(float)60/(float)bpm);

    lines= vector<Line*>();
    blocks= vector<Block*>();

    limitFar->transform->z = (int)(Z0* (Z_HIT_RATIO+Z_HIT_RATIO_TOLERANCE));
    limit->transform->z = (int)(Z0* Z_HIT_RATIO);
    limitClose->transform->z = (int)(Z0* (Z_HIT_RATIO-Z_HIT_RATIO_TOLERANCE));

    //Initiates the screen
    //size(1200, 900);

    targetColor = RandomWallColor();


    //FONT
    TTF_Init();

    char sep = '/';
    stringstream fontFileStream;
    fontFileStream<<MAIN_FOLDER<<sep<<"fonts"<<sep<<"animeace2_reg.ttf";
    string fontFile = fontFileStream.str();

    displayFont = TTF_OpenFont(fontFile.c_str(), 12);
    displayFontLarge = TTF_OpenFont(fontFile.c_str(), 16);
    //displayFontLarge= createFont("data/animeace2_reg.ttf", 16);
    //displayFont = createFont("data/animeace2_reg.ttf", 12);
    textFont(displayFont);

    //SOUND

    Mix_AllocateChannels(3);//Channel 0 : Music, Channel 1 : sound effect (cut)

    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) == -1) //Initialisation de l'API Mixer
    {
        cout<<Mix_GetError()<<"\n";
    }

    stringstream cutFileStream;
    cutFileStream<<MAIN_FOLDER<<sep<<"soundeffects"<<sep<<"cut.wav";
    string cutFile = cutFileStream.str();
    cut = Mix_LoadWAV(cutFile.c_str());
    //cut = new SoundFile(this, "data/" + "cut.wav");

    Mix_Volume(1, MIX_MAX_VOLUME/2);





    //Controller

    int MaxJoysticks = SDL_NumJoysticks();
    cout<<"MAX JOYSTICK: " << MaxJoysticks<<"\n";
    int ControllerIndex = 0;
    for(int JoystickIndex=0; JoystickIndex < MaxJoysticks; ++JoystickIndex)
    {
        if (!SDL_IsGameController(JoystickIndex))
        {
            continue;
        }
        if (ControllerIndex >= 4)
        {
            break;
        }
        ControllerHandles[ControllerIndex] = SDL_GameControllerOpen(JoystickIndex);
        ControllerIndex++;
    }

}






void StartGame(string musicFileName)
{
  //text("Loading...",WIDTH/2,HEIGHT/2,WIDTH, HEIGHT);
    text("Loading...",WIDTH/2,HEIGHT/2);
  if(mode==Playing)
  {
      cout<<"\nStarting game in Playing Mode\n";
     pattern = Pattern::Deserialize(musicFileName);
     cout<<"ZSPEED:"<<Z_SPEED<<"\n";
     if(Z_SPEED<0)
     {
         Z_SPEED=-Z_SPEED;
     }
     //Z_SPEED=abs(Z_SPEED);
     cout<<"ZSPEED after:"<<Z_SPEED<<"\n";
  }
  else
  {
      cout<<"\nStarting game in Making Mode\n";
     pattern = new Pattern(musicFileName);
     if(Z_SPEED>0)
     {
         Z_SPEED=-Z_SPEED;
     }
     //Z_SPEED=-abs(Z_SPEED);
  }

  //musicFile = new SoundFile(this, "data/" + musicFileName + ".mp3");
  //musicFile.play();

  stringstream musicPathStream;
  musicPathStream<<MAIN_FOLDER<<sep<<musicFileName<<".mp3";
  string musicPath = musicPathStream.str();
  musicSound = Mix_LoadMUS(musicPath.c_str());

  Mix_PlayMusic(musicSound, 0);
  step=Game;

  startTime=millis();
  lastFrameTime=startTime;
  lastColorUpdateTime=startTime;
  render();
}



void draw()  //loops forever
{

  //Center in the center of the screen
  pushMatrix();
  translate(WIDTH/2, HEIGHT/2);
  //clear();

//    SDL_SetRenderDrawColor( m_renderer, 0, 0, 0, 255 );
 //   SDL_RenderClear( m_renderer );

  if(step==ChooseMode || step==ChooseMusic)
  {
    Display();
  }

  if(step==Game)
  {
      /**/
    //Check the accelerometer values
    getAccelerometerValues();


    int right_x=acc_right_x;
    int right_y=acc_right_y;
    int left_x=acc_left_x;
    int left_y=acc_left_y;

    accelerometerAction(right_x, right_y, left_x, left_y);

    //ARDUINOHERE

    /****/
    //Changes road color
    if(wallColor.r==targetColor.r && wallColor.g==targetColor.g && wallColor.b==targetColor.b)
    {
        fastTransformation=false;
        targetColor=RandomWallColor();
    }
    if(millis()>lastColorUpdateTime+COLOR_UPDATE_PERIOD)
    {
      if(fastTransformation)
      {
        wallColor = GetNextColor(wallColor, targetColor, 10) ;
      }

      else
      {
        wallColor = GetNextColor(wallColor, targetColor, 1) ;
      }
      lastColorUpdateTime=millis();
    }

    //Spawn moving lines
    if(millis()>lastLineTime+LINE_PERIOD)
    {
      lines.push_back(new Line(0));
      lines.push_back(new Line(1));
      lines.push_back(new Line(2));
      lines.push_back(new Line(3));
      lastLineTime=millis();
    }

    //Spawn blocks
     if(mode==Playing)
    {
        if(currentSpawn<pattern->spawns.size() && pattern->spawns[currentSpawn].time - Z0*(1-Z_HIT_RATIO)/Z_SPEED<=millis()-startTime)
        {
          blocks.push_back(new Block(pattern->spawns[currentSpawn].type));
          currentSpawn++;
        }
    }





    //Move blocks

    for(int i=0;i<blocks.size();i++)
    {
       //cout<<"\nBefore translation: " <<"(Speed = " << Z_SPEED << " and deltaTime = " <<deltaTime<< blocks[i]->transform->z;
       blocks[i]->transform->z-=Z_SPEED*deltaTime;
       blocks[i]->hittable= blocks[i]->transform->z>=Z0*(Z_HIT_RATIO-Z_HIT_RATIO_TOLERANCE) && blocks[i]->transform->z<=Z0*(Z_HIT_RATIO+Z_HIT_RATIO_TOLERANCE);
        //cout<<" / After translation: " << blocks[i]->transform->z<<"\n";
       if(blocks[i]->transform->z<0)
       {
         if(!blocks[i]->hit)
         {
           combo=0;
         }
          blocks[i]->enabled=false;
       }
    }

    //Move lines
    for(int i=0;i<lines.size();i++)
    {
       lines[i]->transform->z-=Z_SPEED*deltaTime;
       if(lines[i]->transform->z<0)
       {
          lines[i]->enabled=false;
       }
    }

    Ditch();//Deletes disabled GameObjects
    Display();




  }

  popMatrix();
  deltaTime=millis()-lastFrameTime;
  lastFrameTime=millis();
}



void Display()
{

  if(step==ChooseMode)
  {

    fill(MENU_SCREEN_COLOR);
    stroke(MENU_SCREEN_COLOR);
    rect(-WIDTH/2,-HEIGHT/2,WIDTH,HEIGHT);
     if(mode==Playing)
     {
         textFont(displayFontLarge);
         fill(SDL_Color{0,255,255});
         //text(">Play", -WIDTH/2+60,-HORIZON_HEIGHT/2, WIDTH, HORIZON_HEIGHT);
         text(">Play", -WIDTH/2+60,-HORIZON_HEIGHT/2);
         textFont(displayFont);
         fill(SDL_Color{255,255,255});
         //text("Make a pattern", -WIDTH/2+60,-HORIZON_HEIGHT/2+60, WIDTH, HORIZON_HEIGHT);
         text("Make a pattern", -WIDTH/2+60,-HORIZON_HEIGHT/2+60);
     }
     else
     {
         textFont(displayFont);
         fill(SDL_Color{255,255,255});
         //text("Play", -WIDTH/2+60,-HORIZON_HEIGHT/2, WIDTH, HORIZON_HEIGHT);
         text("Play", -WIDTH/2+60,-HORIZON_HEIGHT/2);
         textFont(displayFontLarge);
         fill(SDL_Color{0,255,255});
         //text(">Make a pattern", -WIDTH/2+60,-HORIZON_HEIGHT/2+60, WIDTH, HORIZON_HEIGHT);
         text(">Make a pattern", -WIDTH/2+60,-HORIZON_HEIGHT/2+60);
     }
  }

  if(step==ChooseMusic)
  {
       fill(MENU_SCREEN_COLOR);
       stroke(MENU_SCREEN_COLOR);//24, 71, 74);
       rect(-WIDTH/2,-HEIGHT/2,WIDTH,HEIGHT);
       textFont(displayFont);

       fill(SDL_Color{255,255,255});
       if(musics.size()>0)
       {
           for(int i=0;i<currentMusic;i++)
            {
                text(musics[i], -WIDTH/2+120,-HEIGHT/2 + 60*(i+1));// WIDTH, HORIZON_HEIGHT);
            }
       }

       textFont(displayFontLarge);
       fill(SDL_Color{0,255,255});
       if(musics.size()>currentMusic)
       {
           //cout<< "Current music: " << musics[currentMusic]<<"\n";
        text(musics[currentMusic], -WIDTH/2+120,-HEIGHT/2 + 60*(currentMusic+1));//, WIDTH, HORIZON_HEIGHT);
       }
       textFont(displayFont);
       fill(SDL_Color{255,255,255});
       for(int i=currentMusic+1;i<musics.size();i++)
       {
           text(musics[i], -WIDTH/2+120,-HEIGHT/2 + 60*(i+1));// WIDTH, HORIZON_HEIGHT);
       }

  }
  if(step==Game)
  {
    //Display Road
    stroke(SDL_Color{10,30,0});


    fill(wallColor);

    //Temp road
    //rect(-WIDTH/2,-HEIGHT/2,WIDTH,HEIGHT);

    //Actual road
    triangle(0,0,WIDTH/2, HEIGHT/2, -WIDTH/2, HEIGHT/2);
    triangle(0,0,-WIDTH/2, -HEIGHT/2, WIDTH/2, -HEIGHT/2);

    fill(SDL_Color{0.7*wallColor.r,0.7*wallColor.g,0.7*wallColor.b});
    triangle(0,0,WIDTH/2, -HEIGHT/2, WIDTH/2, HEIGHT/2);
    triangle(0,0,-WIDTH/2, HEIGHT/2, -WIDTH/2, -HEIGHT/2);



    //Display lines

    SDL_Color newStrokeColor = MultiplyColor(wallColor,1.3);
    newStrokeColor = SDL_Color{strokeColor.r, strokeColor.g, strokeColor.b, 255};//190};
    //newStrokeColor = SDL_Color{255,255,255,255};
    stroke(newStrokeColor,255);

    for(int i=0;i<lines.size();i++)
    {
        if(lines[i]->transform->z<Z0)
        {
            lines[i]->Display();
        }
       //lines[i]->Display();
    }

    //Display goal
    stroke(SDL_Color{235,232,52});

    fill(SDL_Color{255,255,255,0},0);
    limit->Display();
    stroke(SDL_Color{235, 158, 52});
    fill(SDL_Color{255,255,255,0},0);
    limitFar->Display();
    //fill(SDL_Color{255,255,255,0},0);
    //limitFar->Display();
    limitClose->Display();


    //Display Void
    stroke(SDL_Color{5,255,10});
    fill(SDL_Color{5,5,10});

    //rect(-HORIZON_WIDTH,-HORIZON_HEIGHT, HORIZON_WIDTH, HORIZON_HEIGHT);
    rect(-HORIZON_WIDTH/2,-HORIZON_HEIGHT/2, HORIZON_WIDTH, HORIZON_HEIGHT);

    //Display infos
    if(mode==Playing)
    {
        /*stringstream infoStream;
        infoStream<<"Score: " << points << std::endl;
        infoStream<<points << std::endl;
        infoStream << "Combo: " << combo <<std::endl;
        infoStream << "Multiplier: " << multiplier;
        string infos = infoStream.str();*/
        stringstream scoreStream;
        scoreStream<<"Score: "<<points;
        string scoreString = scoreStream.str();

        stringstream comboStream;
        comboStream<<"Combo: "<<combo;
        string comboString = comboStream.str();

        stringstream multiplierStream;
        multiplierStream<<"Multiplier: "<<multiplier;
        string multiplierString = multiplierStream.str();

        fill(wallColor);
        //text(infos, -HORIZON_WIDTH/2,-HORIZON_HEIGHT/2);//, HORIZON_WIDTH, HORIZON_HEIGHT);

        text(scoreString, -HORIZON_WIDTH/2 + 10,-HORIZON_HEIGHT/2 + 5);//, HORIZON_WIDTH, HORIZON_HEIGHT);

        text(comboString, -HORIZON_WIDTH/2 + 10,-HORIZON_HEIGHT/2 + HORIZON_HEIGHT / 3 + 5);//, HORIZON_WIDTH, HORIZON_HEIGHT);
        text(multiplierString, -HORIZON_WIDTH/2 + 10,-HORIZON_HEIGHT/2 + 2 *HORIZON_HEIGHT / 3 + 5);//, HORIZON_WIDTH, HORIZON_HEIGHT);
    }


    //Display blocks
    SDL_Color blockColor1 = RGBtoGBR(wallColor);
    SDL_Color blockColor2 = RGBtoGBR(blockColor1);

    stroke(SDL_Color{70,0,70,240});
    for(int k=blocks.size()-1;k>=0;k--)
    {
      int i=k;
      if(mode==Saving)
      {
        i=blocks.size()-1-k;
      }
      SDL_Color blockColor = blockColor1;
      if(GetTypeValue(blocks[i]->type)<=3)
      {
          blockColor = blockColor2;
      }

      fill(blockColor,240);
      if(blocks[i]->hit)
      {
        float alpha = 240 - (float)blocks[i]->disappearTimer/DISAPPEAR_TIME * 255;
        if(alpha<=0)
        {
           blocks[i]->enabled=false;
        }
        else
        {
          SDL_Color wonColor = MultiplyColor(blockColor, WON_BLOCK_COLOR_MULTIPLIER);
          fill(wonColor,alpha);
          stroke(wonColor,alpha);
          blocks[i]->Display();
          fill(blockColor,240);
          stroke(blockColor,240);
        }
        blocks[i]->disappearTimer+=deltaTime;
      }
      else if(blocks[i]->transform->z<(Z_HIT_RATIO-Z_HIT_RATIO_TOLERANCE)*Z0)
      {
          SDL_Color lostColor = MultiplyColor(blockColor,LOST_BLOCK_COLOR_MULTIPLIER);
          fill(lostColor,240);
          stroke(lostColor,240);

          blocks[i]->Display();

          fill(blockColor,240);
          stroke(blockColor,240);
      }
      else
      {
        blocks[i]->Display();
      }

    }


    //Display credits
    if(millis() - startTime < CREDIT_DISPLAY_TIME)
    {

      if(millis() - startTime < CREDIT_DISPLAY_TIME/2)
      {
        fill(Negative(wallColor));
      }
      else
      {
        fill(Negative(wallColor),255 * 2*(1- ((float)(millis() - startTime)/ CREDIT_DISPLAY_TIME)));
      }

        stringstream creditsStream;
        creditsStream<<pattern->musicName << " ~ ";
        string credits = creditsStream.str();
        credits+=pattern->artistName;
        text(credits, WIDTH/2-CREDIT_WIDTH, HEIGHT/2-CREDIT_HEIGHT);// CREDIT_WIDTH, CREDIT_HEIGHT);
    }



/**/
  }
    render();

}

/**Deletes unnecessary objects*/
void Ditch()
{

  for(int i=0;i<lines.size();i++)
  {
    if(!lines[i]->enabled)
    {
      delete lines[i];
      lines.erase(lines.begin() + i);
      i--;
    }
  }
   for(int i=0;i<blocks.size();i++)
  {
     if(!blocks[i]->enabled)
    {
      delete blocks[i];
      blocks.erase(blocks.begin() +i);
      i--;
    }
  }

}




/**Interprets accelerometer values and acts accordingly*/
void accelerometerAction(int right_x, int right_y, int left_x, int left_y)
{

  //Right
   if(right_x>ACCELERATION_TRESHOLD && right_x>abs(right_y))
   {
     for(int i=0;i<blocks.size();i++)
      {

         if(blocks[i]->hittable && !blocks[i]->hit && blocks[i]->type==Right)
         {
             blocks[i]->hit=true;
             points+=multiplier;
             combo++;
             Mix_PlayChannel(1, cut, 0);
             //cut.play();
         }
      }
   }

   //RightUP
   if(right_y>ACCELERATION_TRESHOLD && right_y>right_x)
   {
     for(int i=0;i<blocks.size();i++)
      {

         if(blocks[i]->hittable && !blocks[i]->hit && blocks[i]->type==RightUp)
         {
             blocks[i]->hit=true;
             points+=multiplier;
             combo++;
             Mix_PlayChannel(1, cut, 0);
             //cut.play();
         }
      }
   }

   //RightDOWN
   if(right_y<-ACCELERATION_TRESHOLD && right_y<-right_x)
   {
     for(int i=0;i<blocks.size();i++)
      {

         if(blocks[i]->hittable && !blocks[i]->hit && blocks[i]->type==RightDown)
         {
             blocks[i]->hit=true;
             points+=multiplier;
             combo++;
             Mix_PlayChannel(1, cut, 0);
             //cut.play();
         }
      }
   }

   //Left

   if(left_x<-ACCELERATION_TRESHOLD && left_x<-abs(left_y))
   {
     for(int i=0;i<blocks.size();i++)
      {
         if(blocks[i]->hittable && !blocks[i]->hit && blocks[i]->type==Left)
         {
             blocks[i]->hit=true;
             points+=multiplier;
             combo++;
             Mix_PlayChannel(1, cut, 0);
             //cut.play();
         }
      }
   }

   //LeftUp

   if(left_y>ACCELERATION_TRESHOLD && left_y>-left_x)
   {
     for(int i=0;i<blocks.size();i++)
      {
         if(blocks[i]->hittable && !blocks[i]->hit && blocks[i]->type==LeftUp)
         {
             blocks[i]->hit=true;
             points+=multiplier;
             combo++;
             Mix_PlayChannel(1, cut, 0);
             //cut.play();
         }
      }
   }

   //LeftDown
   if(left_y<-ACCELERATION_TRESHOLD && left_y<-left_x)
   {
     for(int i=0;i<blocks.size();i++)
      {
         if(blocks[i]->hittable && !blocks[i]->hit && blocks[i]->type==LeftDown)
         {
             blocks[i]->hit=true;
             points+=multiplier;
             combo++;
             Mix_PlayChannel(1, cut, 0);
             //cut.play();
         }
      }
   }

}

bool hasEnding (std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length())
    {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    }
    else
    {
        return false;
    }
}

vector<string> ListAllFilesIn(string directory, bool print=false)
{
    DIR *dir;
    struct dirent *ent;

    vector<string> files;

    if(print)
    {
        cout<<"Checking files in folder: " << directory<<"\n";
    }


    if ((dir = opendir (directory.c_str())) != NULL)
    {
      /* print all the files and directories within directory */

      while ((ent = readdir (dir)) != NULL)
      {
        if(print)
        {
          cout<<ent->d_name<<"\n";
        }
        files.push_back(ent->d_name);
      }
      closedir (dir);
    }
    else
    {
      /* could not open directory */
      //perror ("");
      cout<<"Error: could not open directory";
      //return EXIT_FAILURE;
    }
    return files;
}

/**What to do when a certain key is pressed*/
void handleEvent(SDL_Event* eventPtr)
{
  SDL_Event event = *eventPtr;
  if(event.type == SDL_QUIT)
  {
      finish=true;
  }


    bool Controller_Up = false;
    bool Controller_Down = false;
    bool Controller_Left = false;
    bool Controller_Right = false;
    bool Controller_Start = false;
    bool Controller_Back = false;
    bool Controller_LeftShoulder = false;
    bool Controller_RightShoulder = false;
    bool Controller_AButton = false;
    bool Controller_BButton = false;
    bool Controller_XButton = false;
    bool Controller_YButton = false;
    int16_t Controller_LeftStickX = 0;
    int16_t Controller_LeftStickY = 0;
    int16_t Controller_RightStickX = 0;
    int16_t Controller_RightStickY = 0;
    int16_t Controller_RightTrigger = 0;
    int16_t Controller_LeftTrigger = 0;

    int16_t thresholdAxis = 64;
  for (int ControllerIndex = 0; ControllerIndex < 4; ++ControllerIndex)
    {
        if(ControllerHandles[ControllerIndex] != 0 && SDL_GameControllerGetAttached(ControllerHandles[ControllerIndex]))
        {
            //cout<<"Checking controller" << ControllerIndex<<"\n";
            // NOTE: We have a controller with index ControllerIndex.
            if(SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_DPAD_UP))
            {

            //cout<<"UUUUUUUUUUUUUPPPPPPPPPPPPPPPPPPPPPP" << ControllerIndex<<"\n";
            }

            Controller_Up = Controller_Up || SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_DPAD_UP);
            Controller_Down = Controller_Down || SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_DPAD_DOWN);
            Controller_Left = Controller_Left || SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_DPAD_LEFT);
            Controller_Right = Controller_Right || SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
            Controller_Start = Controller_Start || SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_START);
            Controller_Back = Controller_Back || SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_BACK);
            Controller_LeftShoulder = Controller_LeftShoulder || SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_LEFTSHOULDER);
            Controller_RightShoulder = Controller_RightShoulder || SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
            Controller_AButton = Controller_AButton || SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_A);
            Controller_BButton = Controller_BButton || SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_B);
            Controller_XButton = Controller_XButton || SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_X);
            Controller_YButton = Controller_YButton || SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_Y);

            if(abs( SDL_GameControllerGetAxis(ControllerHandles[ControllerIndex], SDL_CONTROLLER_AXIS_LEFTX))>abs(Controller_LeftStickX))
            {
                Controller_LeftStickX =SDL_GameControllerGetAxis(ControllerHandles[ControllerIndex], SDL_CONTROLLER_AXIS_LEFTX);
            }
            if(abs(SDL_GameControllerGetAxis(ControllerHandles[ControllerIndex], SDL_CONTROLLER_AXIS_LEFTY))>abs( Controller_LeftStickY))
            {
                Controller_LeftStickY = SDL_GameControllerGetAxis(ControllerHandles[ControllerIndex], SDL_CONTROLLER_AXIS_LEFTY);
            }

            if(abs( SDL_GameControllerGetAxis(ControllerHandles[ControllerIndex], SDL_CONTROLLER_AXIS_RIGHTX))>abs(Controller_RightStickX))
            {
                Controller_RightStickX =SDL_GameControllerGetAxis(ControllerHandles[ControllerIndex], SDL_CONTROLLER_AXIS_RIGHTX);
            }
            if(abs(SDL_GameControllerGetAxis(ControllerHandles[ControllerIndex], SDL_CONTROLLER_AXIS_RIGHTY))>abs( Controller_RightStickY))
            {
                Controller_RightStickY = SDL_GameControllerGetAxis(ControllerHandles[ControllerIndex], SDL_CONTROLLER_AXIS_RIGHTY);
            }

            if(abs(SDL_GameControllerGetAxis(ControllerHandles[ControllerIndex], SDL_CONTROLLER_AXIS_TRIGGERLEFT))>abs( Controller_LeftTrigger))
            {
                Controller_LeftTrigger = SDL_GameControllerGetAxis(ControllerHandles[ControllerIndex], SDL_CONTROLLER_AXIS_TRIGGERLEFT);
            }
             if(abs(SDL_GameControllerGetAxis(ControllerHandles[ControllerIndex], SDL_CONTROLLER_AXIS_TRIGGERRIGHT))>abs( Controller_RightTrigger))
            {
                Controller_RightTrigger = SDL_GameControllerGetAxis(ControllerHandles[ControllerIndex], SDL_CONTROLLER_AXIS_TRIGGERRIGHT);
            }

        }
    }
    lastLeftTriggerValue = Controller_LeftTrigger;

  if(event.type != SDL_KEYDOWN && event.type != SDL_CONTROLLERBUTTONDOWN)
  {
      if(event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_ESCAPE)
      {
        finish = true;
      }
      return;
  }




  SDL_Keycode keyCode = event.key.keysym.sym;
  if(step==ChooseMode)
  {
      if(keyCode==SDLK_UP || keyCode==SDLK_DOWN || Controller_Up || Controller_Down)
      {
        if(mode==Playing)
        {
          cout<<"\nMode: Saving\n";
           mode=Saving;
        }
        else
        {
          cout<<"\nMode: Playing\n";
           mode=Playing;
        }
      }
      if(keyCode==SDLK_RETURN || Controller_AButton)
      {
        step=ChooseMusic;

        if(mode==Saving)
        {

            musics = *(new vector<string>());
            vector<string> files = ListAllFilesIn(MAIN_FOLDER, true);
            for(int i=0;i<files.size();i++)
            {
                if(hasEnding(files[i], ".mp3"))
                {
                    musics.push_back(files[i].substr(0,files[i].length()-4));
                }
            }
        }
        else
        {
            musics = *(new vector<string>());
            vector<string> files = ListAllFilesIn(MAIN_FOLDER, true);

            for(int i=0;i<files.size();i++)
            {
                if(hasEnding(files[i], ".mp3"))
                {
                    string name = files[i].substr(0,files[i].length()-4);
                    string patternFileName = name;
                    patternFileName+= ".pattern";//".json";
                    if (find(files.begin(), files.end(), patternFileName) != files.end())
                    {
                        musics.push_back(name);
                    }
                }
            }
        }
      }

  }


  else if(step==ChooseMusic)
  {
    if(musics.size()>0)
    {
       if(keyCode==SDLK_DOWN ||Controller_Down)
       {
          currentMusic = (currentMusic+1) % musics.size();
            if(musics.size()>0)
            {
                cout<<"\nSelected: "<<musics[currentMusic]<<"\n";
            }

       }
       if(keyCode==SDLK_UP||Controller_Up)
       {
          currentMusic = (currentMusic-1 +musics.size()) % musics.size();
          if(musics.size()>0)
            {
                cout<<"\nSelected: "<<musics[currentMusic]<<"\n";
            }
       }
       if(keyCode==SDLK_RETURN ||Controller_AButton)
       {
           text("Loading...",50,50);//WIDTH, HEIGHT);
           StartGame(musics[currentMusic]);
           if(musics.size()>0)
            {
                cout<<"\nChoosen Music: "<<musics[currentMusic] <<"\n";
            }
       }
    }
  }
  else if(step==Game)
  {
    //In saving mode, we add blocks when the buttons are pressed
    if(mode==Saving)
    {
      if (keyCode==SDLK_o ||keyCode==SDLK_UP ||Controller_YButton)
      {
        pattern->Add(RightUp, millis()-startTime);
        blocks.push_back(new Block(RightUp));
      }
      if (keyCode==SDLK_m || keyCode==SDLK_SEMICOLON ||keyCode==SDLK_RIGHT ||Controller_BButton)
      {
        pattern->Add(Right, millis()-startTime);
        blocks.push_back(new Block(Right));
      }
      if (keyCode==SDLK_l||keyCode==SDLK_DOWN ||Controller_AButton)
      {
        pattern->Add(RightDown, millis()-startTime);
        blocks.push_back(new Block(RightDown));
      }
      if (keyCode==SDLK_z || keyCode==SDLK_w  ||Controller_Up)
      {
        pattern->Add(LeftUp, millis()-startTime);
        blocks.push_back(new Block(LeftUp));
      }
      if (keyCode==SDLK_q || keyCode==SDLK_a  ||Controller_Left)
      {
        pattern->Add(Left, millis()-startTime);
        blocks.push_back(new Block(Left));
      }
      if (keyCode==SDLK_s ||Controller_Down)
      {
        pattern->Add(LeftDown, millis()-startTime);
        blocks.push_back(new Block(LeftDown));
      }
      if(keyCode==SDLK_LEFT)//|| Controller_XButton || Controller_LeftStickX<-thresholdAxis ||Controller_LeftTrigger> thresholdAxis)
      {
        Rewind(100);
      }
      if (keyCode==SDLK_g || keyCode==SDLK_F5 || Controller_Start)
      {
        pattern->Serialize();
      }
    }

    //In playing mode, the blocks are destructed when the button is pressed at the right moment
    else if(mode==Playing)
    {
      if (keyCode==SDLK_o || keyCode==SDLK_UP ||Controller_YButton)
      {
        for(int i=0;i<blocks.size();i++)
        {

           if(blocks[i]->hittable && !blocks[i]->hit && blocks[i]->type==RightUp)
           {
               blocks[i]->hit=true;
               points+=multiplier;
               combo++;
                Mix_PlayChannel(1, cut, 0);
               //cut.play();
           }
        }
      }
      if (keyCode==SDLK_m||keyCode==SDLK_SEMICOLON||keyCode==SDLK_RIGHT ||Controller_BButton)
      {
        for(int i=0;i<blocks.size();i++)
        {

           if(blocks[i]->hittable && !blocks[i]->hit && blocks[i]->type==Right)
           {
               blocks[i]->hit=true;
               points+=multiplier;
               combo++;
                Mix_PlayChannel(1, cut, 0);
               //cut.play();
           }
        }
      }
      if (keyCode==SDLK_l||keyCode==SDLK_DOWN  ||Controller_AButton)
      {
        for(int i=0;i<blocks.size();i++)
        {

           if(blocks[i]->hittable && !blocks[i]->hit && blocks[i]->type==RightDown)
           {
               blocks[i]->hit=true;
               points+=multiplier;
               combo++;
                Mix_PlayChannel(1, cut, 0);
               //cut.play();
           }
        }
      }
      if (keyCode==SDLK_z || keyCode==SDLK_w  ||Controller_Up)
      {
         for(int i=0;i<blocks.size();i++)
        {

           if(blocks[i]->hittable && !blocks[i]->hit && blocks[i]->type==LeftUp)
           {
               blocks[i]->hit=true;
               points+=multiplier;
               combo++;
               Mix_PlayChannel(1, cut, 0);
               //cut.play();
           }
        }
      }
      if (keyCode==SDLK_q|| keyCode==SDLK_a ||Controller_Left)
      {
        for(int i=0;i<blocks.size();i++)
        {

           if(blocks[i]->hittable && !blocks[i]->hit && blocks[i]->type==Left)
           {
               blocks[i]->hit=true;
               points+=multiplier;
               combo++;
               Mix_PlayChannel(1, cut, 0);
               //cut.play();
           }
        }
      }
      if (keyCode==SDLK_s ||Controller_Down)
      {
        for(int i=0;i<blocks.size();i++)
        {

           if(blocks[i]->hittable && !blocks[i]->hit && blocks[i]->type==LeftDown)
           {
               blocks[i]->hit=true;
               points+=multiplier;
               combo++;
               Mix_PlayChannel(1, cut, 0);
               //cut.play();
           }
        }
      }
      multiplier = combo/10 +1;

    }

  }
}

void getAccelerometerValues()
{

}


void Rewind(int timeMs)
{

   double timeSecond = timeMs / 1000.0;
   timeMs = timeSecond * 1000;

   //startTime=min(startTime+(int)(timeSecond*1000),millis());
  startTime=min(startTime+timeMs,millis());
  //musicFile.jump((float)(millis()-startTime)/1000);

  Mix_SetMusicPosition((double)(millis()-startTime) / 1000.0);
  //Move blocks

  for(int i=0;i<blocks.size();i++)
  {
     blocks[i]->transform->z+=Z_SPEED*timeMs;
     blocks[i]->hittable= blocks[i]->transform->z>=Z0*(Z_HIT_RATIO-Z_HIT_RATIO_TOLERANCE) && blocks[i]->transform->z<=Z0*(Z_HIT_RATIO+Z_HIT_RATIO_TOLERANCE);

     if(blocks[i]->transform->z<0)
     {
       if(!blocks[i]->hit)
       {
         combo=0;
       }
        blocks[i]->enabled=false;
     }
  }


  for(int i=0;i<pattern->spawns.size();i++)
  {
      if(pattern->spawns[i].time >=millis()-startTime - Z_SPEED * Z0*Z_HIT_RATIO)
      {
         pattern->spawns.erase(pattern->spawns.begin() + i);
         i--;
      }
  }


  //Move lines
  for(int i=0;i<lines.size();i++)
  {
     lines[i]->transform->z+=Z_SPEED*timeMs;
     if(lines[i]->transform->z<0)
     {
        lines[i]->enabled=false;
     }
  }


}













/*

int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        SDL_Log("Nem indithato az SDL: %s", SDL_GetError());
        exit(1);
    }
    SDL_Window *window = SDL_CreateWindow("SDL peldaprogram", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 440, 360, 0);
    if (window == NULL) {
        SDL_Log("Nem hozhato letre az ablak: %s", SDL_GetError());
        exit(1);
    }
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    if (renderer == NULL) {
        SDL_Log("Nem hozhato letre a megjelenito: %s", SDL_GetError());
        exit(1);
    }
    SDL_RenderClear(renderer);

    int x, y, r;

    circleRGBA(renderer, x, y, r, 255, 0, 0, 255);
    circleRGBA(renderer, x + r, y, r, 0, 255, 0, 255);
    circleRGBA(renderer, x + r * cos(3.1415 / 3), y - r * sin(3.1415 / 3), r, 0, 0, 255, 255);

    SDL_RenderPresent(renderer);

    SDL_Event ev;
    while (SDL_WaitEvent(&ev) && ev.type != SDL_QUIT) {
    }

    SDL_Quit();
    return 0;
}
*/




int main( int argc, char * argv[] )
{

    setup();
//ListAllFilesIn(".", true);
    Transform* tr = new Transform(1,1,1);

    try
    {
        //SDL sdl( SDL_INIT_VIDEO | SDL_INIT_TIMER );

        //sdl.draw();
        fill({0,125,125,255});
        stroke({255,255,255,255});
        rect(0,0,100,300);

        int timeDeltaForRewindMs = 20;
        int lastRewindTimeMs=millis();
        while(!finish)
        {

            draw();
            SDL_Event event;

            while(SDL_PollEvent(&event))
            {
                handleEvent(&event);
            }

            if(lastLeftTriggerValue>64 && millis() - lastRewindTimeMs > timeDeltaForRewindMs)//64: threshold
            {
                lastRewindTimeMs=millis();
                Rewind(100);
            }

            SDL_Delay(1);
        }


        return 0;
    }
    catch ( const InitError & err )
    {
        std::cerr << "Error while initializing SDL:  "
                  << err.what()
                  << std::endl;
    }

    return 1;
}













