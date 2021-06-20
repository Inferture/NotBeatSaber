#include "../include/ColorUtility.h"

#include <SDL2/SDL.h>
#include "../include/Global.h"



using std::max;
using std::min;
/**Generates a random color for the road (not too dark, not too bright)*/
SDL_Color RandomWallColor()
{

   float min_brightness=255*3*MIN_BRIGHTNESS;
   float max_brightness=255*3*MAX_BRIGHTNESS;
   uint8_t r = rand() %255;
   uint8_t g = rand() %255;
   uint8_t b = rand() %255;

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
SDL_Color GetNextColor(SDL_Color current, SDL_Color target, uint8_t speed)
{
    if(current.r>target.r)
    {
      current = SDL_Color{(uint8_t)max((int)target.r,current.r-speed),current.g, current.b};
    }
    if(current.r<target.r)
    {
      current = SDL_Color{(uint8_t)min((int)target.r,current.r+speed),current.g, current.b};
    }

    if(current.b>target.b)
    {
      current = SDL_Color{current.r,current.g, (uint8_t)max((int)target.b,current.b-speed)};
    }
    if(current.b<target.b)
    {
      current = SDL_Color{current.r,current.g, (uint8_t)min((int)target.b,current.b+speed)};
    }

    if(current.g>target.g)
    {
      current = SDL_Color{current.r, (uint8_t)max((int)target.g,current.g-speed), current.b};
    }
    if(current.g<target.g)
    {
      current = SDL_Color{current.r, (uint8_t)min((int)target.g,current.g+speed), current.b};
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
    uint8_t newR = min(255,(int)(c.r*f));
    uint8_t newG = min(255,(int)(c.g*f));
    uint8_t newB = min(255,(int)(c.b*f));
    return SDL_Color{newR, newG, newB};
}
/**Returns the negative color of c*/
SDL_Color Negative(SDL_Color c)
{
   return SDL_Color{(uint8_t) (255-c.r), (uint8_t) (255-c.g), (uint8_t) (255-c.b)};
}
