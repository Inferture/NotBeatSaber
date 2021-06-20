#include <exception>
#include <string>
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <sstream>
#include <fstream>
#include <vector>
#include <dirent.h>
#include <algorithm>
#include <SDL2/SDL2_gfxPrimitives.h>
#include<time.h>

#if SWITCH
#include <switch.h>
#endif // SWITCH

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
using std::string;
using std::ofstream;
using std::endl;

vector<Line*> lines;
vector<Block*> blocks;
vector<Block*> hittableBlocks;

//0:up, 1:left, 2:down, 3:right
Line* limitDown = new Line(2);
Line* limitUp = new Line(0);
Line* limitLeft = new Line(1);
Line* limitRight = new Line(3);


Block* limitFar = new Block(WIDTH, HEIGHT);
Block* limit = new Block(WIDTH, HEIGHT);
Block* limitClose = new Block(WIDTH, HEIGHT);

//
Pattern* pattern;

int currentSpawn = 0;
int startTime;
int lastFrameTime;
int deltaTime = 0;
SDL_Color targetColor;
bool fastTransformation;

Mix_Music* musicSound;
Mix_Chunk* cut;


vector<string> musics(0);
int currentMusic;

char sep = '/';

TTF_Font* displayFont;
TTF_Font* displayFontLarge;

//Motion controls
int last_acc_right_x;
int last_acc_right_y;
int last_acc_left_x;
int last_acc_left_y;

int acc_right_x;
int acc_right_y;
int acc_left_x;
int acc_left_y;

//Will be displayed at the center in play mode
int points = 0;
int combo = 0;
int multiplier = 1;



bool finish = false;
int16_t lastLeftTriggerValue = 0;
SDL_GameController* ControllerHandles[4];

string bottomInfo = "";
int bottomInfoDisplayTime = 2000;//in milliseconds
int bottomInfoAppearTime = 0;


#if SWITCH
PadState pad;
HidSixAxisSensorHandle handles[2];
HidVibrationDeviceHandle VibrationDeviceHandles[2];
HidVibrationValue VibrationValueRight;
HidVibrationValue VibrationValueLeft;

int lastHitRightTime;
int lastHitLeftTime;
int lastMissRightTime;
int lastMissLeftTime;

BlockType lastHitRightType;
BlockType lastHitLeftType;


float MIN_FREQUENCY_MISS=5;
float MAX_FREQUENCY_MISS=1;

float MIN_FREQUENCY_HIT=10;
float MAX_FREQUENCY_HIT=250;

float DELTA_FREQUENCY_NOTES = 30;

float MAX_AMPLITUDE_HIT=0.03;
float MAX_AMPLITUDE_MISS=50;




float MISS_RUMBLE_TIME=300;
float HIT_RUMBLE_TIME=250;



bool debugMotionControlValues = true;
int writeMotionControlIndex = 0;
stringstream motionControlStream;


int MIN_ACCELERATION_FRAMES = 2;//minimum number of frames the acceleration has to be held to be counted.

int framesRightUp = 0;
int framesRightDown = 0;
int framesRight = 0;
int framesLeftUp = 0;
int framesLeftDown = 0;
int framesLeft = 0;

int lastRightUpTime = -5000;
int lastRightDownTime = -5000;
int lastRightLeftTime = -5000;
int lastRightRightTime = -5000;
int lastLeftUpTime = -5000;
int lastLeftDownTime = -5000;
int lastLeftRightTime = -5000;
int lastLeftLeftTime = -5000;

float ACCELERATION_TRESHOLD_RIGHTUP = 2.7;
float ACCELERATION_TRESHOLD_RIGHTDOWN = 2.4;
float ACCELERATION_TRESHOLD_RIGHT = 2.1;
float ACCELERATION_TRESHOLD_RIGHTLEFT = 2.1;
float ACCELERATION_TRESHOLD_LEFTUP = 2.7;
float ACCELERATION_TRESHOLD_LEFTDOWN = 2.6;
float ACCELERATION_TRESHOLD_LEFT = 2.1;
float ACCELERATION_TRESHOLD_LEFTRIGHT = 2.1;





//When you end your motion in a direction, the deceleration will count as an acceleration in the other direction. To avoid this we make it harder
//to activate the other direction during INVERSE_DIRECTION_BLOCKING_TIME ms.
int INVERSE_DIRECTION_BLOCKING_TIME = 120;
float INVERSE_DIRECTION_THRESHOLD_MULTIPLIER = 2;
#endif // SWITCH

struct Inputs
{
    bool rightInput;
    bool rightUpInput;
    bool rightDownInput;
    bool leftInput;
    bool leftUpInput;
    bool leftDownInput;
    bool selectInput;
    bool quitInput;
    bool mainMenuInput;
    bool rewindInput;
    bool saveInput;
};
Inputs lastFrameInputs;


void Display();
void Ditch();
void Rewind(int timeMs);


///Time since the beginning in milliseconds.
int millis()
{
    return SDL_GetTicks();
}

///Called at the beginning to initialize some values
void setup()
{

    srand (time(NULL));

    SCALE_HORIZON = (float)HORIZON_WIDTH / (float)WIDTH;

#ifdef SWITCH
    MAIN_FOLDER = "./NotBeatSaber_data";
#else
    MAIN_FOLDER = "./NotBeatSaber_data";
#endif // SWITCH


    linePeriod = (int)(1000 * (float)60 / (float)bpm);

    lines = vector<Line*>();
    blocks = vector<Block*>();

    limitFar->transform->z = (int)(Z0 * (Z_HIT_RATIO + Z_HIT_RATIO_TOLERANCE));
    limit->transform->z = (int)(Z0 * Z_HIT_RATIO);
    limitClose->transform->z = (int)(Z0 * (Z_HIT_RATIO - Z_HIT_RATIO_TOLERANCE));

    targetColor = RandomWallColor();


    //FONT

    TTF_Init();

    char sep = '/';
    stringstream fontFileStream;
    fontFileStream << MAIN_FOLDER << sep << "fonts" << sep << "animeace2_reg.ttf";
    string fontFile = fontFileStream.str();

    displayFont = TTF_OpenFont(fontFile.c_str(), 12);
    displayFontLarge = TTF_OpenFont(fontFile.c_str(), 16);
    textFont(displayFont);


    //SOUND

    Mix_AllocateChannels(3);//Channel 0 : Music, Channel 1 : sound effect (cut)

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) == -1) //Initialisation de l'API Mixer
    {
        cout << Mix_GetError() << "\n";
    }

    stringstream cutFileStream;
    cutFileStream << MAIN_FOLDER << sep << "soundeffects" << sep << "cut.wav";
    string cutFile = cutFileStream.str();
    cut = Mix_LoadWAV(cutFile.c_str());

    Mix_Volume(1, MIX_MAX_VOLUME / 2);


    //Controller

    int MaxJoysticks = SDL_NumJoysticks();
    cout << "MAX JOYSTICK: " << MaxJoysticks << "\n";
    int ControllerIndex = 0;
    for (int JoystickIndex = 0; JoystickIndex < MaxJoysticks; ++JoystickIndex)
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


void Reinitialize()
{
    currentSpawn = 0;

    while (blocks.size() > 0)
    {
        delete blocks.back();
        blocks.pop_back();
    }

    while (lines.size() > 0)
    {
        delete lines.back();
        lines.pop_back();
    }

    Mix_HaltMusic();

    points = 0;
    combo = 0;
    multiplier = 1;

}

void StartGame(string musicFileName)
{
    wallColor = RandomWallColor();
    text("Loading...", WIDTH / 2, HEIGHT / 2);

    if (mode == Playing)
    {
        cout << "\nStarting game in Playing Mode\n";
        pattern = Pattern::Deserialize(musicFileName);
        cout << "ZSPEED:" << Z_SPEED << "\n";
        if (Z_SPEED < 0)
        {
            Z_SPEED = -Z_SPEED;
        }
        cout << "ZSPEED after:" << Z_SPEED << "\n";
    }
    else
    {
        cout << "\nStarting game in Making Mode\n";
        pattern = new Pattern(musicFileName);
        if (Z_SPEED > 0)
        {
            Z_SPEED = -Z_SPEED;
        }
    }

    stringstream musicPathStream;
    musicPathStream << MAIN_FOLDER << sep << musicFileName << ".mp3";
    string musicPath = musicPathStream.str();
    musicSound = Mix_LoadMUS(musicPath.c_str());

    Mix_PlayMusic(musicSound, 0);
    step = Game;

    startTime = millis();
    lastFrameTime = startTime;
    lastColorUpdateTime = startTime;
    render();

}


void draw()  //loops forever
{

    //Center in the center of the screen
    pushMatrix();
    translate(WIDTH / 2, HEIGHT / 2);
    if (step == ChooseMode || step == ChooseMusic)
    {
        Display();
    }

    if (step == Game)
    {
        int right_x = acc_right_x;
        int right_y = acc_right_y;
        int left_x = acc_left_x;
        int left_y = acc_left_y;

        /****/
        //Changes road color
        if (wallColor.r == targetColor.r && wallColor.g == targetColor.g && wallColor.b == targetColor.b)
        {
            fastTransformation = false;
            targetColor = RandomWallColor();
        }
        if (millis() > lastColorUpdateTime + COLOR_UPDATE_PERIOD)
        {
            if (fastTransformation)
            {
                wallColor = GetNextColor(wallColor, targetColor, 10);
            }

            else
            {
                wallColor = GetNextColor(wallColor, targetColor, 1);
            }
            lastColorUpdateTime = millis();
        }

        //Spawn moving lines
        if (millis() > lastLineTime + linePeriod)
        {
            lines.push_back(new Line(0));
            lines.push_back(new Line(1));
            lines.push_back(new Line(2));
            lines.push_back(new Line(3));
            lastLineTime = millis();
        }

        //Spawn blocks
        if (mode == Playing)
        {
            if (currentSpawn < pattern->spawns.size() && pattern->spawns[currentSpawn].time - Z0 * (1 - Z_HIT_RATIO) / Z_SPEED <= millis() - startTime)
            {
                blocks.push_back(new Block(pattern->spawns[currentSpawn].type));
                currentSpawn++;
            }
        }

        //Move blocks

        for (int i = 0; i < blocks.size(); i++)
        {
            blocks[i]->transform->z -= Z_SPEED * deltaTime;

            bool hittable = blocks[i]->transform->z >= Z0 * (Z_HIT_RATIO - Z_HIT_RATIO_TOLERANCE) && blocks[i]->transform->z <= Z0 * (Z_HIT_RATIO + Z_HIT_RATIO_TOLERANCE);
            if(blocks[i]->hittable && !hittable && !blocks[i]->hit)//Block was just missed
            {
                combo = 0;
                #if SWITCH
                if(GetTypeValue(blocks[i]->type) <= 3)
                {
                    lastMissRightTime = millis();
                }
                else
                {
                    lastMissLeftTime = millis();
                }
                #endif // SWITCH
            }
            blocks[i]->hittable = hittable;

            if (blocks[i]->transform->z < 0)
            {
                blocks[i]->enabled = false;
            }
        }

        //Move lines
        for (int i = 0; i < lines.size(); i++)
        {
            lines[i]->transform->z -= Z_SPEED * deltaTime;
            if (lines[i]->transform->z < 0)
            {
                lines[i]->enabled = false;
            }
        }

        Ditch();//Deletes disabled GameObjects
        Display();




    }

    popMatrix();
    deltaTime = millis() - lastFrameTime;
    lastFrameTime = millis();
}


void Display()
{

    if (step == ChooseMode)
    {

        fill(MENU_SCREEN_COLOR);
        stroke(MENU_SCREEN_COLOR);
        rect(-WIDTH / 2, -HEIGHT / 2, WIDTH, HEIGHT);
        if (mode == Playing)
        {
            textFont(displayFontLarge);
            fill(SDL_Color{ 0,255,255 });
            text(">Play", -WIDTH / 2 + 60, -HORIZON_HEIGHT / 2);
            textFont(displayFont);
            fill(SDL_Color{ 255,255,255 });
            text("Make a pattern", -WIDTH / 2 + 60, -HORIZON_HEIGHT / 2 + 60);
        }
        else
        {
            textFont(displayFont);
            fill(SDL_Color{ 255,255,255 });
            text("Play", -WIDTH / 2 + 60, -HORIZON_HEIGHT / 2);
            textFont(displayFontLarge);
            fill(SDL_Color{ 0,255,255 });
            text(">Make a pattern", -WIDTH / 2 + 60, -HORIZON_HEIGHT / 2 + 60);
        }
    }

    if (step == ChooseMusic)
    {
        fill(MENU_SCREEN_COLOR);
        stroke(MENU_SCREEN_COLOR);
        rect(-WIDTH / 2, -HEIGHT / 2, WIDTH, HEIGHT);
        textFont(displayFont);

        fill(SDL_Color{ 255,255,255 });
        if (musics.size() > 0)
        {
            for (int i = 0; i < currentMusic; i++)
            {
                text(musics[i], -WIDTH / 2 + 120, -HEIGHT / 2 + 60 * (i + 1));
            }
        }

        textFont(displayFontLarge);
        fill(SDL_Color{ 0,255,255 });
        if (musics.size() > currentMusic)
        {
            text(musics[currentMusic], -WIDTH / 2 + 120, -HEIGHT / 2 + 60 * (currentMusic + 1));
        }
        textFont(displayFont);
        fill(SDL_Color{ 255,255,255 });
        for (int i = currentMusic + 1; i < musics.size(); i++)
        {
            text(musics[i], -WIDTH / 2 + 120, -HEIGHT / 2 + 60 * (i + 1));
        }

    }
    if (step == Game)
    {
        //Display Road
        stroke(SDL_Color{ 10,30,0 });


        fill(wallColor);
        triangle(0, 0, WIDTH / 2, HEIGHT / 2, -WIDTH / 2, HEIGHT / 2);
        triangle(0, 0, -WIDTH / 2, -HEIGHT / 2, WIDTH / 2, -HEIGHT / 2);

        fill(SDL_Color{ (uint8_t)(0.7 * wallColor.r),(uint8_t)(0.7 * wallColor.g),(uint8_t)(0.7 * wallColor.b) });
        triangle(0, 0, WIDTH / 2, -HEIGHT / 2, WIDTH / 2, HEIGHT / 2);
        triangle(0, 0, -WIDTH / 2, HEIGHT / 2, -WIDTH / 2, -HEIGHT / 2);



        //Display lines

        SDL_Color newStrokeColor = MultiplyColor(wallColor, 1.3);
        newStrokeColor = SDL_Color{ strokeColor.r, strokeColor.g, strokeColor.b, 255 };
        stroke(newStrokeColor, 255);

        for (int i = 0; i < lines.size(); i++)
        {
            if (lines[i]->transform->z < Z0)
            {
                lines[i]->Display();
            }
        }

        //Display goal
        stroke(SDL_Color{ 235,232,52 });

        fill(SDL_Color{ 255,255,255,0 }, 0);
        limit->Display();
        stroke(SDL_Color{ 235, 158, 52 });
        fill(SDL_Color{ 255,255,255,0 }, 0);
        limitFar->Display();
        limitClose->Display();


        //Display Void
        stroke(SDL_Color{ 5,255,10 });
        fill(SDL_Color{ 5,5,10 });

        rect(-HORIZON_WIDTH / 2, -HORIZON_HEIGHT / 2, HORIZON_WIDTH, HORIZON_HEIGHT);

        //Display infos
        if (mode == Playing)
        {
            stringstream scoreStream;
            scoreStream << "Score: " << points;
            string scoreString = scoreStream.str();

            stringstream comboStream;
            comboStream << "Combo: " << combo;
            string comboString = comboStream.str();

            stringstream multiplierStream;
            multiplierStream << "Multiplier: " << multiplier;
            string multiplierString = multiplierStream.str();

            fill(wallColor);

            text(scoreString, -HORIZON_WIDTH / 2 + 10, -HORIZON_HEIGHT / 2 + 5);

            text(comboString, -HORIZON_WIDTH / 2 + 10, -HORIZON_HEIGHT / 2 + HORIZON_HEIGHT / 3 + 5);
            text(multiplierString, -HORIZON_WIDTH / 2 + 10, -HORIZON_HEIGHT / 2 + 2 * HORIZON_HEIGHT / 3 + 5);
        }


        //Display blocks
        SDL_Color blockColor1 = RGBtoGBR(wallColor);
        SDL_Color blockColor2 = RGBtoGBR(blockColor1);

        stroke(SDL_Color{ 70,0,70,240 });
        for (int k = blocks.size() - 1; k >= 0; k--)
        {
            int i = k;
            if (mode == Saving)
            {
                i = blocks.size() - 1 - k;
            }
            SDL_Color blockColor = blockColor1;
            if (GetTypeValue(blocks[i]->type) <= 3)
            {
                blockColor = blockColor2;
            }

            fill(blockColor, 240);
            if (blocks[i]->hit)
            {
                float alpha = 240 - (float)blocks[i]->disappearTimer / DISAPPEAR_TIME * 255;
                if (alpha <= 0)
                {
                    blocks[i]->enabled = false;
                }
                else
                {
                    SDL_Color wonColor = MultiplyColor(blockColor, WON_BLOCK_COLOR_MULTIPLIER);
                    fill(wonColor, alpha);
                    stroke(wonColor, alpha);
                    blocks[i]->Display();
                    fill(blockColor, 240);
                    stroke(blockColor, 240);
                }
                blocks[i]->disappearTimer += deltaTime;
            }
            else if (blocks[i]->transform->z < (Z_HIT_RATIO - Z_HIT_RATIO_TOLERANCE) * Z0)
            {
                SDL_Color lostColor = MultiplyColor(blockColor, LOST_BLOCK_COLOR_MULTIPLIER);
                fill(lostColor, 240);
                stroke(lostColor, 240);

                blocks[i]->Display();

                fill(blockColor, 240);
                stroke(blockColor, 240);
            }
            else if (blocks[i]->transform->z < (Z_HIT_RATIO + Z_HIT_RATIO_TOLERANCE) * Z0)
            {
                SDL_Color sliceableColor = MultiplyColor(blockColor, SLICEABLE_BLOCK_COLOR_MULTIPLIER);
                fill(sliceableColor, 240);
                stroke(sliceableColor, 240);

                blocks[i]->Display();

                fill(blockColor, 240);
                stroke(blockColor, 240);
            }
            else
            {
                blocks[i]->Display();
            }

        }


        //Display credits
        if (millis() - startTime < CREDIT_DISPLAY_TIME)
        {

            if (millis() - startTime < CREDIT_DISPLAY_TIME / 2)
            {
                fill(Negative(wallColor));
            }
            else
            {
                fill(Negative(wallColor), 255 * 2 * (1 - ((float)(millis() - startTime) / CREDIT_DISPLAY_TIME)));
            }

            stringstream creditsStream;
            creditsStream << pattern->musicName << " ~ ";
            string credits = creditsStream.str();
            credits += pattern->artistName;


            int w;
            int h;
            const char* textChar = credits.c_str();
            TTF_SizeText(font, textChar, &w, &h);

            text(credits, WIDTH / 2 - w - 10, HEIGHT / 2 - CREDIT_HEIGHT);// CREDIT_WIDTH, CREDIT_HEIGHT);
        }



        //Display other text

        if (millis() < bottomInfoAppearTime + bottomInfoDisplayTime)
        {
            if (millis() - bottomInfoAppearTime < bottomInfoDisplayTime / 2)
            {
                fill(Negative(wallColor));
            }
            else
            {
                fill(Negative(wallColor), 255 * 2 * (1 - ((float)(millis() - bottomInfoAppearTime) / bottomInfoDisplayTime)));
            }

            text(bottomInfo, 0, HEIGHT / 2 - CREDIT_HEIGHT);
        }

        /**/
    }
    render();

}

/**Deletes unnecessary objects*/
void Ditch()
{

    for (int i = 0; i < lines.size(); i++)
    {
        if (!lines[i]->enabled)
        {
            delete lines[i];
            lines.erase(lines.begin() + i);
            i--;
        }
    }
    for (int i = 0; i < blocks.size(); i++)
    {
        if (!blocks[i]->enabled)
        {
            delete blocks[i];
            blocks.erase(blocks.begin() + i);
            i--;
        }
    }

}

///Writes info at the bottom of the screen
void writeBottomInfo(string info)
{
    bottomInfo = info;
    bottomInfoAppearTime = millis();
}


///Checks if a string ends with a given ending string
bool hasEnding(std::string const& fullString, std::string const& ending) {
    if (fullString.length() >= ending.length())
    {
        return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
    }
    else
    {
        return false;
    }
}

///Lists all files in a given directory
vector<string> ListAllFilesIn(string directory, bool print = false)
{
    DIR* dir;
    struct dirent* ent;

    vector<string> files;

    if (print)
    {
        cout << "Checking files in folder: " << directory << "\n";
    }


    if ((dir = opendir(directory.c_str())) != NULL)
    {
        /* print all the files and directories within directory */

        while ((ent = readdir(dir)) != NULL)
        {
            if (print)
            {
                cout << ent->d_name << "\n";
            }
            files.push_back(ent->d_name);
        }
        closedir(dir);
    }
    else
    {
        cout << "Error: could not open directory";
    }
    return files;
}

/**What to do when a certain key is pressed*/
void handleEvent(SDL_Event* eventPtr, Inputs* inputs)
{
    SDL_Event event = *eventPtr;
    if (event.type == SDL_QUIT)
    {
        finish = true;
        return;
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
        if (ControllerHandles[ControllerIndex] != 0 && SDL_GameControllerGetAttached(ControllerHandles[ControllerIndex]))
        {

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


            if (abs(SDL_GameControllerGetAxis(ControllerHandles[ControllerIndex], SDL_CONTROLLER_AXIS_LEFTX)) > abs(Controller_LeftStickX))
            {
                Controller_LeftStickX = SDL_GameControllerGetAxis(ControllerHandles[ControllerIndex], SDL_CONTROLLER_AXIS_LEFTX);
            }
            if (abs(SDL_GameControllerGetAxis(ControllerHandles[ControllerIndex], SDL_CONTROLLER_AXIS_LEFTY)) > abs(Controller_LeftStickY))
            {
                Controller_LeftStickY = SDL_GameControllerGetAxis(ControllerHandles[ControllerIndex], SDL_CONTROLLER_AXIS_LEFTY);
            }

            if (abs(SDL_GameControllerGetAxis(ControllerHandles[ControllerIndex], SDL_CONTROLLER_AXIS_RIGHTX)) > abs(Controller_RightStickX))
            {
                Controller_RightStickX = SDL_GameControllerGetAxis(ControllerHandles[ControllerIndex], SDL_CONTROLLER_AXIS_RIGHTX);
            }
            if (abs(SDL_GameControllerGetAxis(ControllerHandles[ControllerIndex], SDL_CONTROLLER_AXIS_RIGHTY)) > abs(Controller_RightStickY))
            {
                Controller_RightStickY = SDL_GameControllerGetAxis(ControllerHandles[ControllerIndex], SDL_CONTROLLER_AXIS_RIGHTY);
            }

            if (abs(SDL_GameControllerGetAxis(ControllerHandles[ControllerIndex], SDL_CONTROLLER_AXIS_TRIGGERLEFT)) > abs(Controller_LeftTrigger))
            {
                Controller_LeftTrigger = SDL_GameControllerGetAxis(ControllerHandles[ControllerIndex], SDL_CONTROLLER_AXIS_TRIGGERLEFT);
            }
            if (abs(SDL_GameControllerGetAxis(ControllerHandles[ControllerIndex], SDL_CONTROLLER_AXIS_TRIGGERRIGHT)) > abs(Controller_RightTrigger))
            {
                Controller_RightTrigger = SDL_GameControllerGetAxis(ControllerHandles[ControllerIndex], SDL_CONTROLLER_AXIS_TRIGGERRIGHT);
            }

        }
    }
    lastLeftTriggerValue = Controller_LeftTrigger;


    SDL_Keycode keyCode = event.key.keysym.sym;


    inputs->rightUpInput = inputs->rightUpInput || ((event.type == SDL_KEYDOWN && (keyCode == SDLK_o || keyCode == SDLK_UP)) || (event.type == SDL_CONTROLLERBUTTONDOWN && Controller_YButton));
    inputs->rightInput = inputs->rightInput || ((event.type == SDL_KEYDOWN && (keyCode == SDLK_m || keyCode == SDLK_SEMICOLON || keyCode == SDLK_RIGHT)) || (event.type == SDL_CONTROLLERBUTTONDOWN && Controller_BButton));
    inputs->rightDownInput = inputs->rightDownInput || ((event.type == SDL_KEYDOWN && (keyCode == SDLK_l || keyCode == SDLK_DOWN)) || (event.type == SDL_CONTROLLERBUTTONDOWN && Controller_AButton));
    inputs->leftUpInput = inputs->leftUpInput || ((event.type == SDL_KEYDOWN && (keyCode == SDLK_z || keyCode == SDLK_w)) || (event.type == SDL_CONTROLLERBUTTONDOWN && Controller_Up));
    inputs->leftInput = inputs->leftInput || ((event.type == SDL_KEYDOWN && (keyCode == SDLK_q || keyCode == SDLK_a)) || (event.type == SDL_CONTROLLERBUTTONDOWN && Controller_Left));
    inputs->leftDownInput = inputs->leftDownInput || ((event.type == SDL_KEYDOWN && (keyCode == SDLK_s)) || (Controller_Down));
    inputs->rewindInput = inputs->rewindInput || ((event.type == SDL_KEYDOWN && (keyCode == SDLK_LEFT)));
    inputs->saveInput = inputs->saveInput || ((event.type == SDL_KEYDOWN && (keyCode == SDLK_g || keyCode == SDLK_F5)) || (event.type == SDL_CONTROLLERBUTTONDOWN && (Controller_Start || Controller_RightShoulder)));
    inputs->mainMenuInput = inputs->mainMenuInput || ((event.type == SDL_KEYUP && keyCode == SDLK_SPACE) || (event.type == SDL_CONTROLLERBUTTONDOWN && Controller_LeftShoulder));
    inputs->quitInput = inputs->quitInput || ((event.type == SDL_KEYUP && keyCode == SDLK_ESCAPE));
    inputs->selectInput = inputs->selectInput || ((event.type == SDL_KEYDOWN && keyCode == SDLK_RETURN) || (event.type == SDL_CONTROLLERBUTTONDOWN && Controller_BButton));

}

#if SWITCH
//SWITCH CONTROLS EXAMPLE: https://github.com/switchbrew/switch-examples/blob/master/network/ldn/source/main.c
///Checks the inputs from the switch and fills the input struct
void checkSwitchInputs(Inputs* inputs)
{
    u64 kDown = padGetButtonsDown(&pad);
    u64 kHeld = padGetButtons(&pad);


    inputs->quitInput = (kDown & HidNpadButton_Minus);
    inputs->saveInput = (kDown & HidNpadButton_Plus);
    inputs->rightUpInput = (kDown & HidNpadButton_X);
    inputs->rightInput = (kDown & HidNpadButton_A);
    inputs->rightDownInput = (kDown & HidNpadButton_B);
    inputs->leftUpInput = (kDown & HidNpadButton_Up);
    inputs->leftInput = (kDown & HidNpadButton_Left);
    inputs->leftDownInput = (kDown & HidNpadButton_Down);
    inputs->saveInput = (kDown & HidNpadButton_Plus);
    inputs->mainMenuInput = (kDown & HidNpadButton_R) && (kDown & HidNpadButton_L);
    inputs->quitInput = (kDown & HidNpadButton_Minus);
    inputs->selectInput = (kDown & HidNpadButton_A);
    inputs->rewindInput = (kHeld & HidNpadButton_ZL);
    lastLeftTriggerValue = 0;


    HidSixAxisSensorState sixaxis0 = { 0 };
    HidSixAxisSensorState sixaxis1 = { 0 };


    u64 attrib = padGetAttributes(&pad);
    if (attrib & HidNpadAttribute_IsLeftConnected)
        hidGetSixAxisSensorStates(handles[0], &sixaxis0, 1);
    if (attrib & HidNpadAttribute_IsRightConnected)
        hidGetSixAxisSensorStates(handles[1], &sixaxis1, 1);

    float multiplier = (millis() - lastRightLeftTime) > INVERSE_DIRECTION_BLOCKING_TIME ? 1 : INVERSE_DIRECTION_THRESHOLD_MULTIPLIER;
    if (sixaxis1.acceleration.x < -ACCELERATION_TRESHOLD_RIGHT * multiplier)
    {
        framesRight++;
        lastRightRightTime = millis();
    }
    else
    {
        framesRight = 0;
    }

    multiplier = (millis() - lastRightDownTime) > INVERSE_DIRECTION_BLOCKING_TIME ? 1 : INVERSE_DIRECTION_THRESHOLD_MULTIPLIER;
    if (sixaxis1.acceleration.z < -ACCELERATION_TRESHOLD_RIGHTUP * multiplier - 1)
    {
        framesRightUp++;
        lastRightUpTime = millis();
    }
    else
    {
        framesRightUp = 0;
    }
    multiplier = (millis() - lastRightUpTime) > INVERSE_DIRECTION_BLOCKING_TIME ? 1 : INVERSE_DIRECTION_THRESHOLD_MULTIPLIER;
    if (sixaxis1.acceleration.z > ACCELERATION_TRESHOLD_RIGHTDOWN * multiplier - 1)
    {
        framesRightDown++;
        lastRightDownTime = millis();
    }
    else
    {
        framesRightDown = 0;
    }
    multiplier = (millis() - lastRightRightTime) > INVERSE_DIRECTION_BLOCKING_TIME ? 1 : INVERSE_DIRECTION_THRESHOLD_MULTIPLIER;
    if (sixaxis1.acceleration.x > ACCELERATION_TRESHOLD_RIGHTLEFT * multiplier)
    {
        lastRightLeftTime = millis();
    }


    multiplier = (millis() - lastLeftRightTime) > INVERSE_DIRECTION_BLOCKING_TIME ? 1 : INVERSE_DIRECTION_THRESHOLD_MULTIPLIER;
    if (sixaxis0.acceleration.x > ACCELERATION_TRESHOLD_LEFT * multiplier)//???????
    {
        framesLeft++;
        lastLeftLeftTime = millis();
    }
    else
    {
        framesLeft = 0;
    }
    multiplier = (millis() - lastLeftDownTime) > INVERSE_DIRECTION_BLOCKING_TIME ? 1 : INVERSE_DIRECTION_THRESHOLD_MULTIPLIER;
    if (sixaxis0.acceleration.z < -ACCELERATION_TRESHOLD_LEFTUP * multiplier - 1)
    {
        framesLeftUp++;
        lastLeftUpTime = millis();
    }
    else
    {
        framesLeftUp = 0;
    }
    multiplier = (millis() - lastLeftUpTime) > INVERSE_DIRECTION_BLOCKING_TIME ? 1 : INVERSE_DIRECTION_THRESHOLD_MULTIPLIER;
    if (sixaxis0.acceleration.z > ACCELERATION_TRESHOLD_LEFTDOWN * multiplier - 1)
    {
        framesLeftDown++;
        lastLeftDownTime = millis();
    }
    else
    {
        framesLeftDown = 0;
    }
    multiplier = (millis() - lastLeftLeftTime) > INVERSE_DIRECTION_BLOCKING_TIME ? 1 : INVERSE_DIRECTION_THRESHOLD_MULTIPLIER;
    if (sixaxis0.acceleration.x > ACCELERATION_TRESHOLD_LEFTRIGHT * multiplier)
    {
        lastLeftRightTime = millis();
    }

    inputs->rightInput = inputs->rightInput || framesRight > MIN_ACCELERATION_FRAMES;
    inputs->rightUpInput = inputs->rightUpInput || framesRightUp > MIN_ACCELERATION_FRAMES;
    inputs->rightDownInput = inputs->rightDownInput || framesRightDown > MIN_ACCELERATION_FRAMES;
    inputs->leftInput = inputs->leftInput || framesLeft > MIN_ACCELERATION_FRAMES;
    inputs->leftUpInput = inputs->leftUpInput || framesLeftUp > MIN_ACCELERATION_FRAMES;
    inputs->leftDownInput = inputs->leftDownInput || framesLeftDown > MIN_ACCELERATION_FRAMES;


    //Check the acceleration values for calbration
    if (debugMotionControlValues)
    {
        if (kDown & HidNpadButton_Y)
        {
            motionControlStream << writeMotionControlIndex << ":" << endl;
            motionControlStream << "(" << sixaxis0.acceleration.x << "," << sixaxis0.acceleration.y << "," << sixaxis0.acceleration.z << ") | ";
            motionControlStream << "(" << sixaxis1.acceleration.x << "," << sixaxis1.acceleration.y << "," << sixaxis1.acceleration.z << ")" << endl;
            motionControlStream << "(" << sixaxis0.angular_velocity.x << "," << sixaxis0.angular_velocity.y << "," << sixaxis0.angular_velocity.z << ") | ";
            motionControlStream << "(" << sixaxis1.angular_velocity.x << "," << sixaxis1.angular_velocity.y << "," << sixaxis1.angular_velocity.z << ")" << endl;
            motionControlStream << "(" << sixaxis0.angle.x << "," << sixaxis0.angle.y << "," << sixaxis0.angle.z << ") | ";
            motionControlStream << "(" << sixaxis1.angle.x << "," << sixaxis1.angle.y << "," << sixaxis1.angle.z << ")" << endl << endl;

            motionControlStream << "(" << sixaxis1.direction.direction[0][0] << "," << sixaxis1.direction.direction[0][1] << "," << sixaxis1.direction.direction[0][2] << ")" << endl;
            motionControlStream << "(" << sixaxis1.direction.direction[1][0] << "," << sixaxis1.direction.direction[1][1] << "," << sixaxis1.direction.direction[1][2] << ")" << endl;
            motionControlStream << "(" << sixaxis1.direction.direction[2][0] << "," << sixaxis1.direction.direction[2][1] << "," << sixaxis1.direction.direction[2][2] << ")" << endl<<endl<<endl;




            stringstream info;
            info << "Wrote value for index: " << writeMotionControlIndex;
            writeBottomInfo(info.str());

            writeMotionControlIndex++;
        }

        if (kDown & HidNpadButton_Right)
        {
            char sep = '/';
            stringstream pathstream;

            pathstream << MAIN_FOLDER << sep << "MotionControlValues.txt";
            string path = pathstream.str();
            ofstream fileStream(path.c_str());

            fileStream << motionControlStream.str();
            fileStream.close();
        }
    }
}
#endif // SWITCH




///Interprets the input of the player according to the current mode
void handleInputs(Inputs inputs)
{

    bool rightUpInput = inputs.rightUpInput;
    bool rightInput = inputs.rightInput;
    bool rightDownInput = inputs.rightDownInput;
    bool leftUpInput = inputs.leftUpInput;
    bool leftInput = inputs.leftInput;
    bool leftDownInput = inputs.leftDownInput;
    bool rewindInput = inputs.rewindInput;
    bool saveInput = inputs.saveInput;
    bool mainMenuInput = inputs.mainMenuInput;
    bool quitInput = inputs.quitInput;
    bool selectInput = inputs.selectInput;


    if (mainMenuInput)
    {
        Reinitialize();
        step = ChooseMode;
    }

    if (quitInput)
    {
        finish = true;
        return;
    }

    if (step == ChooseMode)
    {
        if (rightUpInput || rightDownInput || leftUpInput || leftDownInput)
        {
            if (mode == Playing)
            {
                cout << "\nMode: Saving\n";
                mode = Saving;
            }
            else
            {
                cout << "\nMode: Playing\n";
                mode = Playing;
            }
        }
        if (selectInput)
        {
            step = ChooseMusic;

            if (mode == Saving)
            {

                musics = *(new vector<string>());
                vector<string> files = ListAllFilesIn(MAIN_FOLDER, true);
                for (int i = 0; i < files.size(); i++)
                {
                    if (hasEnding(files[i], ".mp3"))
                    {
                        musics.push_back(files[i].substr(0, files[i].length() - 4));
                    }
                }
            }
            else
            {
                musics = *(new vector<string>());
                vector<string> files = ListAllFilesIn(MAIN_FOLDER, true);

                for (int i = 0; i < files.size(); i++)
                {
                    if (hasEnding(files[i], ".mp3"))
                    {
                        string name = files[i].substr(0, files[i].length() - 4);
                        string patternFileName = name;
                        patternFileName += ".pattern";
                        if (find(files.begin(), files.end(), patternFileName) != files.end())
                        {
                            musics.push_back(name);
                        }
                    }
                }
            }
        }
    }


    else if (step == ChooseMusic)
    {
        if (musics.size() > 0)
        {
            if (leftDownInput || rightDownInput)
            {
                currentMusic = (currentMusic + 1) % musics.size();
                if (musics.size() > 0)
                {
                    cout << "\nSelected: " << musics[currentMusic] << "\n";
                }

            }
            if (leftUpInput || rightUpInput)
            {
                currentMusic = (currentMusic - 1 + musics.size()) % musics.size();
                if (musics.size() > 0)
                {
                    cout << "\nSelected: " << musics[currentMusic] << "\n";
                }
            }
            if (selectInput)
            {
                text("Loading...", 50, 50);//WIDTH, HEIGHT);
                StartGame(musics[currentMusic]);
                if (musics.size() > 0)
                {
                    cout << "\nChoosen Music: " << musics[currentMusic] << "\n";
                }
            }
        }
    }
    else if (step == Game)
    {

        //In saving mode, we add blocks when the buttons are pressed
        if (mode == Saving)
        {
            if (rightUpInput && !lastFrameInputs.rightUpInput)
            {
                pattern->Add(RightUp, millis() - startTime);
                blocks.push_back(new Block(RightUp));
            }
            if (rightInput && !lastFrameInputs.rightInput)
            {
                pattern->Add(Right, millis() - startTime);
                blocks.push_back(new Block(Right));
            }
            if (rightDownInput && !lastFrameInputs.rightDownInput)
            {
                pattern->Add(RightDown, millis() - startTime);
                blocks.push_back(new Block(RightDown));
            }
            if (leftUpInput && !lastFrameInputs.leftUpInput)
            {
                pattern->Add(LeftUp, millis() - startTime);
                blocks.push_back(new Block(LeftUp));
            }
            if (leftInput && !lastFrameInputs.leftInput)
            {
                pattern->Add(Left, millis() - startTime);
                blocks.push_back(new Block(Left));
            }
            if (leftDownInput && !lastFrameInputs.leftDownInput)
            {
                pattern->Add(LeftDown, millis() - startTime);
                blocks.push_back(new Block(LeftDown));
            }
            if (rewindInput)
            {
                Rewind(100);
            }
            if (saveInput)
            {
                int result = pattern->Serialize();

                if (result)
                {
                    writeBottomInfo("Problem when saving...");
                }
                else
                {
                    writeBottomInfo("Saved !");
                }
            }
        }

        //In playing mode, the blocks are destructed when the button is pressed at the right moment
        else if (mode == Playing)
        {
            if (rightUpInput)
            {
                for (int i = 0; i < blocks.size(); i++)
                {
                    if (blocks[i]->hittable && !blocks[i]->hit && blocks[i]->type == RightUp)
                    {
                        blocks[i]->hit = true;
                        points += multiplier;
                        combo++;
                        Mix_PlayChannel(1, cut, 0);
                        #if SWITCH
                        lastHitRightTime = millis();
                        lastHitRightType = RightUp;
                        #endif // SWITCH
                    }
                }
            }
            if (rightInput)
            {
                for (int i = 0; i < blocks.size(); i++)
                {
                    if (blocks[i]->hittable && !blocks[i]->hit && blocks[i]->type == Right)
                    {
                        blocks[i]->hit = true;
                        points += multiplier;
                        combo++;
                        Mix_PlayChannel(1, cut, 0);
                        #if SWITCH
                        lastHitRightTime = millis();
                        lastHitRightType = Right;
                        #endif // SWITCH
                    }
                }
            }
            if (rightDownInput)
            {
                for (int i = 0; i < blocks.size(); i++)
                {

                    if (blocks[i]->hittable && !blocks[i]->hit && blocks[i]->type == RightDown)
                    {
                        blocks[i]->hit = true;
                        points += multiplier;
                        combo++;
                        Mix_PlayChannel(1, cut, 0);
                        #if SWITCH
                        lastHitRightTime = millis();
                        lastHitRightType = RightDown;
                        #endif // SWITCH
                    }
                }
            }
            if (leftUpInput)
            {
                for (int i = 0; i < blocks.size(); i++)
                {

                    if (blocks[i]->hittable && !blocks[i]->hit && blocks[i]->type == LeftUp)
                    {
                        blocks[i]->hit = true;
                        points += multiplier;
                        combo++;
                        Mix_PlayChannel(1, cut, 0);
                        #if SWITCH
                        lastHitLeftTime = millis();
                        lastHitLeftType = LeftUp;
                        #endif // SWITCH
                    }
                }
            }
            if (leftInput)
            {
                for (int i = 0; i < blocks.size(); i++)
                {

                    if (blocks[i]->hittable && !blocks[i]->hit && blocks[i]->type == Left)
                    {
                        blocks[i]->hit = true;
                        points += multiplier;
                        combo++;
                        Mix_PlayChannel(1, cut, 0);
                        #if SWITCH
                        lastHitLeftTime = millis();
                        lastHitLeftType = Left;
                        #endif // SWITCH
                    }
                }
            }
            if (leftDownInput)
            {
                for (int i = 0; i < blocks.size(); i++)
                {

                    if (blocks[i]->hittable && !blocks[i]->hit && blocks[i]->type == LeftDown)
                    {
                        blocks[i]->hit = true;
                        points += multiplier;
                        combo++;
                        Mix_PlayChannel(1, cut, 0);
                        #if SWITCH
                        lastHitLeftTime = millis();
                        lastHitLeftType = LeftDown;
                        #endif // SWITCH
                    }
                }
            }
            multiplier = combo / 10 + 1;
        }
    }
}

///For saving mode: rewinds the music to remap a part
void Rewind(int timeMs)
{

    double timeSecond = timeMs / 1000.0;
    timeMs = timeSecond * 1000;

    startTime = min(startTime + timeMs, millis());

    Mix_SetMusicPosition((double)(millis() - startTime) / 1000.0);

    //Move blocks
    for (int i = 0; i < blocks.size(); i++)
    {
        blocks[i]->transform->z += Z_SPEED * timeMs;
        blocks[i]->hittable = blocks[i]->transform->z >= Z0 * (Z_HIT_RATIO - Z_HIT_RATIO_TOLERANCE) && blocks[i]->transform->z <= Z0 * (Z_HIT_RATIO + Z_HIT_RATIO_TOLERANCE);

        if (blocks[i]->transform->z < 0)
        {
            if (!blocks[i]->hit)
            {
                combo = 0;
            }
            blocks[i]->enabled = false;
        }
    }


    for (int i = 0; i < pattern->spawns.size(); i++)
    {
        if (pattern->spawns[i].time >= millis() - startTime - Z_SPEED * Z0 * Z_HIT_RATIO)
        {
            pattern->spawns.erase(pattern->spawns.begin() + i);
            i--;
        }
    }


    //Move lines
    for (int i = 0; i < lines.size(); i++)
    {
        lines[i]->transform->z += Z_SPEED * timeMs;
        if (lines[i]->transform->z < 0)
        {
            lines[i]->enabled = false;
        }
    }
}





int main(int argc, char* argv[])
{

#if SWITCH
    padInitializeDefault(&pad);
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);

    hidGetSixAxisSensorHandles(&handles[0], 2, HidNpadIdType_No1, HidNpadStyleTag_NpadJoyDual);
    hidStartSixAxisSensor(handles[0]);
    hidStartSixAxisSensor(handles[1]);

    hidInitializeVibrationDevices(VibrationDeviceHandles, 2, HidNpadIdType_No1, HidNpadStyleTag_NpadJoyDual);

    VibrationValueRight.amp_low   = 0.0f;
    VibrationValueRight.freq_low  = 10.0f;
    VibrationValueRight.amp_high  = 0.0f;
    VibrationValueRight.freq_high = 20.0f;


    VibrationValueLeft.amp_low   = 0.0f;
    VibrationValueLeft.freq_low  = 10.0f;
    VibrationValueLeft.amp_high  = 0.0f;
    VibrationValueLeft.freq_high = 20.0f;

#endif
    setup();

    int timeDeltaForRewindMs = 20;
    int lastRewindTimeMs = millis();

    while (!finish)
    {
        Inputs inputs;

        inputs.leftDownInput = false;
        inputs.leftUpInput = false;
        inputs.leftInput = false;
        inputs.rightDownInput = false;
        inputs.rightInput = false;
        inputs.rightUpInput = false;
        inputs.saveInput = false;
        inputs.mainMenuInput = false;
        inputs.quitInput = false;
        inputs.rewindInput = false;
        inputs.selectInput = false;

#if SWITCH
        padUpdate(&pad);
        handleInputs(inputs);
        Inputs* inputsPtr = &inputs;
        checkSwitchInputs(inputsPtr);

        //Vibration feedback

        hidSendVibrationValue(VibrationDeviceHandles[0], &VibrationValueLeft);
        hidSendVibrationValue(VibrationDeviceHandles[1], &VibrationValueRight);

        int rightDeltaFrequency=DELTA_FREQUENCY_NOTES;
        int leftDeltaFrequency=0;
        if(lastHitRightType == Right)
        {
            rightDeltaFrequency = 3 * DELTA_FREQUENCY_NOTES;
        }
        else if(lastHitRightType == RightUp)
        {
            rightDeltaFrequency = 5 * DELTA_FREQUENCY_NOTES;
        }
        if(lastHitLeftType == Left)
        {
            leftDeltaFrequency = 2 * DELTA_FREQUENCY_NOTES;
        }
        else if(lastHitLeftType == LeftUp)
        {
            leftDeltaFrequency = 4 * DELTA_FREQUENCY_NOTES;
        }
        if(millis() - lastHitRightTime < HIT_RUMBLE_TIME)
        {
            float t = (millis() - lastHitRightTime) / HIT_RUMBLE_TIME;

            VibrationValueRight.amp_high  = MAX_AMPLITUDE_MISS * (t > 0.5 ? (2-2*t) : 1);
            VibrationValueRight.freq_high = t * MAX_FREQUENCY_HIT + (1 - t) * MIN_FREQUENCY_HIT + rightDeltaFrequency;
        }
        else
        {
            VibrationValueRight.amp_high  = 0;
        }
        if(millis() - lastHitLeftTime < HIT_RUMBLE_TIME)
        {
            float t = (millis() - lastHitLeftTime) / HIT_RUMBLE_TIME;

            VibrationValueLeft.amp_high  = MAX_AMPLITUDE_MISS * (t > 0.5 ? (2-2*t) : 1);
            VibrationValueLeft.freq_high = t * MAX_FREQUENCY_HIT + (1 - t) * MIN_FREQUENCY_HIT + leftDeltaFrequency;
        }
        else
        {
            VibrationValueLeft.amp_high  = 0;
        }
        if(millis() - lastMissRightTime < MISS_RUMBLE_TIME)
        {
            float t = (millis() - lastMissRightTime) / MISS_RUMBLE_TIME;

            VibrationValueRight.amp_low  = MAX_AMPLITUDE_MISS * (t > 0.5 ? (2-2*t) : 1);
            VibrationValueRight.freq_low= t * MIN_FREQUENCY_MISS + (1 - t) * MAX_FREQUENCY_MISS;
        }
        else
        {
            VibrationValueRight.amp_low  = 0;
        }
        if(millis() - lastMissLeftTime < MISS_RUMBLE_TIME)
        {
            float t = (millis() - lastMissLeftTime) / MISS_RUMBLE_TIME;

            VibrationValueLeft.amp_low  = MAX_AMPLITUDE_MISS * (t > 0.5 ? (2-2*t) : 1);
            VibrationValueLeft.freq_low= t * MIN_FREQUENCY_MISS + (1 - t) * MAX_FREQUENCY_MISS;
        }
        else
        {
            VibrationValueLeft.amp_low  = 0;
        }

#else
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            handleEvent(&event, &inputs);
        }
#endif // SWITCH

        handleInputs(inputs);
        lastFrameInputs = inputs;

        if (lastLeftTriggerValue > 64 && millis() - lastRewindTimeMs > timeDeltaForRewindMs)//64: threshold
        {
            lastRewindTimeMs = millis();
            Rewind(100);
        }

        draw();

#if SWITCH
        finish = finish || !appletMainLoop();
        SDL_Delay(16);
#endif // SWITCH
    }
    SDL_Quit();
    return 0;
}
