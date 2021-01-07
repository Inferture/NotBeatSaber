#ifndef CONSTANTS_H_INCLUDED
#define CONSTANTS_H_INCLUDED

#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

using std::string;


enum GameMode
{
   Playing,
   Saving
};


enum GameStep
{
   ChooseMode,
   ChooseMusic,
   Game,//Used both for Playing and Making a pattern (Saving) mode
   End
};


//Global variable

//0
extern GameStep step;
/***GAME PARAMETERS TO CHANGE***/
extern  string music;
extern GameMode mode;
extern int bpm;//the higher the bpm, the higher the frequency of lines coming towards you. Just for aesthetic purpose.
/******************************/


//1
extern int BLOCK_WIDTH;
extern int BLOCK_HEIGHT;

extern float Z0;
extern float Z_SPEED;


extern int HORIZON_WIDTH;
extern int HORIZON_HEIGHT;

extern int WIDTH;
extern int HEIGHT;

extern float SCALE_HORIZON;

extern int points;
extern int combo;
extern int multiplier;


extern int LINE_PERIOD;
extern int lastLineTime;

extern int COLOR_UPDATE_PERIOD;
extern int lastColorUpdateTime;
extern SDL_Color wallColor;
extern SDL_Color MENU_SCREEN_COLOR;
extern float Z_HIT_RATIO;
extern float Z_HIT_RATIO_TOLERANCE;

extern int DISAPPEAR_TIME;

extern float WON_BLOCK_COLOR_MULTIPLIER;
extern float LOST_BLOCK_COLOR_MULTIPLIER;

extern float MIN_BRIGHTNESS;
extern float MAX_BRIGHTNESS;


extern int CREDIT_DISPLAY_TIME;

extern int CREDIT_WIDTH;
extern int CREDIT_HEIGHT;

extern string MAIN_FOLDER;



//+
extern SDL_Color fillColor;
extern SDL_Color strokeColor;


extern TTF_Font* font;



#endif // CONSTANTS_H_INCLUDED
