#include "../include/Global.h"

#include "../include/DrawingUtility.h"














GameStep step = ChooseMode;
/***GAME PARAMETERS TO CHANGE***/
string music="MeltyBlood";
GameMode mode=Playing;
int bpm=153;//The higher the bpm, the higher the frequency of lines coming towards you. Just for aesthetic purpose.
/******************************/



//1
int BLOCK_WIDTH=200;
int BLOCK_HEIGHT=600;

float Z0=1000;
float Z_SPEED=0.5;//0.5;


//Black rectangle at the center
int HORIZON_WIDTH=128;//100;
int HORIZON_HEIGHT=72;//75;

//Screen resolution
int WIDTH=1280;
int HEIGHT=720;

//Difference between the the scale at the front and at the back (where there is the black rectangle)
float SCALE_HORIZON = 0.125;




int LINE_PERIOD=500;
int lastLineTime;

int COLOR_UPDATE_PERIOD=200;
int lastColorUpdateTime;
SDL_Color wallColor={20,65,0};
SDL_Color MENU_SCREEN_COLOR = {24, 71, 74};
float Z_HIT_RATIO=0.1;
float Z_HIT_RATIO_TOLERANCE=0.05;

int DISAPPEAR_TIME=200;

float WON_BLOCK_COLOR_MULTIPLIER=1.3;
float LOST_BLOCK_COLOR_MULTIPLIER=0.5;

float MIN_BRIGHTNESS=1;
float MAX_BRIGHTNESS=1;


int CREDIT_DISPLAY_TIME=5000;

int CREDIT_WIDTH=300;
int CREDIT_HEIGHT=30;

#ifdef SWITCH
string MAIN_FOLDER = "./NotBeatSaber/data";
#else
string MAIN_FOLDER = "./data";
#endif // SWITCH




//+
SDL_Color fillColor;
SDL_Color strokeColor;


TTF_Font* font;


