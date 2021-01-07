#include "../include/ColorUtility.h"

#include <SDL2/SDL.h>
#include "../include/Global.h"
#include <cstdlib>
#include <stdlib.h>
#include <algorithm>



using std::max;
using std::min;
/**Generates a random color for the road (not too dark, not too bright)*/
SDL_Color RandomWallColor()
{

  float min_brightness=255*3*MIN_BRIGHTNESS;
  float max_brightness=255*3*MAX_BRIGHTNESS;
   int r = rand() %255;
   int g = rand() %255;
   int b = rand() %255;

   float brightness = max(r+g+b,1);

   if(r+g+b<min_brightness)
   {
      r*= min_brightness/brightness;
      g*= min_brightness/brightness;
      b*= min_brightness/brightness;
   }
   if(r+g+b>max_brightness)
   {
      r*= max_brightness/brightness;
      g*= max_brightness/brightness;
      b*= max_brightness/brightness;
   }
   return SDL_Color{r,g,b};
}

/**Gets from a color current to a color target with the speed speed*/
SDL_Color GetNextColor(SDL_Color current, SDL_Color target, int speed)
{
    if(current.r>target.r)
    {
      current = SDL_Color{max((int)target.r,current.r-speed),current.g, current.b};
    }
    if(current.r<target.r)
    {
      current = SDL_Color{min((int)target.r,current.r+speed),current.g, current.b};
    }

    if(current.b>target.b)
    {
      current = SDL_Color{current.r,current.g, max((int)target.b,current.b-speed)};
    }
    if(current.b<target.b)
    {
      current = SDL_Color{current.r,current.g, min((int)target.b,current.b+speed)};
    }

    if(current.g>target.g)
    {
      current = SDL_Color{current.r, max((int)target.g,current.g-speed), current.b};
    }
    if(current.g<target.g)
    {
      current = SDL_Color{current.r, min((int)target.g,current.g+speed), current.b};
    }
    return current;
}


/**RGB inversion*/
SDL_Color RGBtoGBR(SDL_Color origin)
{
  return SDL_Color{origin.g, origin.b, origin.r};
}
/**intensify or attenuates a color with a float f*/
SDL_Color MultiplyColor(SDL_Color c, float f)
{
    int newR = min(255,(int)(c.r*f));
    int newG = min(255,(int)(c.g*f));
    int newB = min(255,(int)(c.b*f));
    return SDL_Color{newR, newG, newB};
}
/**Returns the negative color of c*/
SDL_Color Negative(SDL_Color c)
{
   return SDL_Color{255-c.r, 255-c.g, 255-c.b};
}
