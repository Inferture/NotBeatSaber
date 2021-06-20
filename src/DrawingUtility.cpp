#include "../include/DrawingUtility.h"

#include <SDL2/SDL.h>
#include <iostream>
#include <algorithm>
#include <SDL2/SDL2_gfxPrimitives.h>
#include "../include/Global.h"

using std::cout;
using std::min;
using std::max;

SDL SDL::sdl(SDL_INIT_VIDEO | SDL_INIT_TIMER |SDL_INIT_GAMECONTROLLER);


SDL::SDL( Uint32 flags )
{
    if ( SDL_Init( flags ) != 0 )
        throw InitError();

    if ( SDL_CreateWindowAndRenderer( 1280, 720, SDL_WINDOW_SHOWN,
                                      &m_window, &m_renderer ) != 0 )
        throw InitError();

    SDL_SetWindowTitle(m_window, "NotBeatSaber");

    m_scale = {1,1};
    m_translation = {0,0};
    SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);
}

SDL::~SDL()
{
    SDL_DestroyWindow( m_window );
    SDL_DestroyRenderer( m_renderer );
    SDL_Quit();
}


///Creates a new transformation in the stack (which starts with the identity, with a 0,0 translation and 1,1 scale)/
void SDL::pushMatrix()
{
    m_transformations.push_back({{0,0}, {1,1}});
    m_scale = GetCurrentScale();
    m_translation = GetCurrentTranslation();
}

///Pops the last transformation off the stack
void SDL::popMatrix()
{
    m_transformations.pop_back();
    m_scale = GetCurrentScale();
    m_translation = GetCurrentTranslation();
}


///Calculates the resulting scale with all the transformations.
Vector2 SDL::GetCurrentScale()
{
    float scaleX = 1;
    float scaleY = 1;
    for(int i=0;i<m_transformations.size();i++)
    {
        scaleX*=m_transformations[i].scale.x;
        scaleY*=m_transformations[i].scale.y;
    }
    return Vector2({scaleX, scaleY});
}

///Calculates the resulting translation with all the transformations.
SDL_Rect SDL::GetCurrentTranslation()
{
    float translateX = 0;
    float translateY = 0;
    for(int i=0;i<m_transformations.size();i++)
    {
        translateX+=m_transformations[i].translation.x;
        translateY+=m_transformations[i].translation.y;
    }

    return SDL_Rect({(int)round(translateX), (int)round(translateY)});
}

///Translates everything by (x,y). This transformation is active until the transformation is popped (with popMatrix)
void SDL::translate(int x, int y)
{
    SDL_Rect currentTranslation = m_transformations[m_transformations.size()-1].translation;
    currentTranslation = {currentTranslation.x+x, currentTranslation.y+y};
    m_transformations[m_transformations.size()-1].translation = currentTranslation;
    m_translation = GetCurrentTranslation();
}

///Scales everything by (sx,sy). This transformation is active until the transformation is popped (with popMatrix)
void SDL::scale(float sx, float sy)
{
    Vector2 currentScale =  m_transformations[m_transformations.size()-1].scale;
    currentScale = {currentScale.x *= sx, currentScale.y*sy};
    m_transformations[m_transformations.size()-1].scale = currentScale;
    m_scale = GetCurrentScale();
}

///Draws a rectangle at (x,y) of size (w,h)
void SDL::rect(int x, int y, int w, int h)
{
    //Calculate the coordinates including to the transformations (scales and translations)
    int centerX = x + w/2;
    int centerY = y + h/2;

    centerX *= m_scale.x;
    centerY *= m_scale.y;

    x = centerX - w*m_scale.x/2;
    y = centerY - h*m_scale.y/2;

    SDL_Rect rectangle{x + m_translation.x ,y + m_translation.y,(int)round(w * m_scale.x),(int)round(h * m_scale.y)};


    //Draws Filled rectangle
    SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor( m_renderer,fillColor.r, fillColor.g, fillColor.b, fillColor.a);
    SDL_RenderFillRect( m_renderer, &rectangle );

    //Draws Outline (stroke)
    SDL_SetRenderDrawColor( m_renderer,strokeColor.r, strokeColor.g, strokeColor.b, strokeColor.a);
    SDL_RenderDrawRect(m_renderer, &rectangle);
    SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);
}

///Draws a line from (x0,y0) to (x1,y1)
void SDL::line(int x0, int y0, int x1, int y1)
{
    //Calculate the coordinates including to the transformations (scales and translations)
    int centerX = (x0 + x1) / 2;
    int centerY = (y0 + y1) / 2;

    centerX *= m_scale.x;
    centerY *= m_scale.y;

    x0 = centerX + (x0 - centerX) * m_scale.x;
    y0 = centerY + (y0 - centerY) * m_scale.y;
    x1 = centerX + (x1 - centerX) * m_scale.x;
    y1 = centerY + (y1 - centerY) * m_scale.y;

    x0+=m_translation.x;
    x1+=m_translation.x;
    y0+=m_translation.y;
    y1+=m_translation.y;


    //Draws the line (using SDL_gfx function)
    thickLineRGBA(m_renderer,x0,y0,x1,y1,1,strokeColor.r, strokeColor.g, strokeColor.b, strokeColor.a);
    SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);
}

///Writes a text at x,y with the size w,h
void SDL::text(string text,int x, int y, int w, int h)
{
    //Calculate the coordinates including to the transformations (scales and translations)
    int centerX = x + w/2;
    int centerY = y + h/2;

    centerX *= m_scale.x;
    centerY *= m_scale.y;

    x = centerX - w*m_scale.x/2;
    y = centerY - h*m_scale.y/2;
    SDL_Rect textPosition = {x + m_translation.x,y + m_translation.y,(int)round(w * m_scale.x),(int)round(h * m_scale.y)};


    //Draws the text
    SDL_Color fontColor = fillColor;
    SDL_Surface * textSurface = TTF_RenderText_Solid(font, text.c_str(), fontColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(m_renderer, textSurface);

    SDL_RenderCopy(m_renderer, textTexture, NULL, &textPosition);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

///Automatically calculates the width and height of the text to draw
void SDL::text(string text,int x, int y)
{
    int w;
    int h;
    const char* textChar = text.c_str();
    TTF_SizeText(font, textChar, &w, &h);

    SDL::sdl.text(text,x,y,w,h);
    //SDL_Render
}

///Draws a triangle with the vertices (x0,y0), (x1,y1), (x2,y2)
void SDL::triangle(int x0, int y0, int x1, int y1, int x2, int y2)
{

    //Calculate the coordinates including to the transformations (scales and translations)
    int centerX=0;
    int centerY=0;

    x0 = centerX + (x0-centerX) * m_scale.x;
    x1 = centerX + (x1-centerX) * m_scale.x;
    x2 = centerX + (x2-centerX) * m_scale.x;

    y0 = centerY + (y0-centerY) * m_scale.y;
    y1 = centerY + (y1-centerY) * m_scale.y;
    y2 = centerY + (y2-centerY) * m_scale.y;



    x0+=m_translation.x;
    x1+=m_translation.x;
    x2+=m_translation.x;

    y0+=m_translation.y;
    y1+=m_translation.y;
    y2+=m_translation.y;


    //Triangle fill
    filledTrigonRGBA(m_renderer, x1, y1, x2, y2, x0, y0,fillColor.r, fillColor.g, fillColor.b, fillColor.a);

    //Triangle outline
    trigonRGBA(m_renderer,x1, y1, x2, y2, x0, y0,strokeColor.r, strokeColor.g, strokeColor.b, strokeColor.a);

}


void SDL::render()
{
    SDL_RenderPresent( m_renderer );
}

void SDL::clear()
{
    SDL_SetRenderDrawColor( m_renderer, 0, 0, 0, 255 );
    SDL_RenderClear( m_renderer );
}



InitError::InitError() :
    exception(),
    msg( SDL_GetError() )
{
}

InitError::InitError( const std::string & m ) :
    exception(),
    msg( m )
{
}

InitError::~InitError() throw()
{
}

const char * InitError::what() const throw()
{
    return msg.c_str();
}



//Change drawing parameters


void textFont(TTF_Font * newFont)
{
    font = newFont;
}

void fill(SDL_Color color, uint8_t alpha)
{
    SDL_Color resultColor = SDL_Color{color.r, color.g, color.b, alpha};
    fillColor = resultColor;
}

void stroke(SDL_Color color, uint8_t alpha)
{
    SDL_Color resultColor = SDL_Color{color.r, color.g, color.b, alpha};
    strokeColor = resultColor;
}









//Functions to draw using the SDL

void text(string text, int x, int y, int w, int h)
{
    SDL::sdl.text(text,x,y,w,h);
}
void text(string text, int x, int y)
{
    SDL::sdl.text(text,x,y);
}
void triangle(int x0, int y0, int x1, int y1, int x2, int y2)
{
    SDL::sdl.triangle(x0,y0,x1,y1,x2,y2);
}

void line(int x0, int y0, int x1, int y1)
{
    SDL::sdl.line(x0,y0,x1,y1);
}

void pushMatrix()
{
    SDL::sdl.pushMatrix();
}

void popMatrix()
{
    SDL::sdl.popMatrix();
}
void translate(int x, int y)
{
    SDL::sdl.translate(x,y);
}
void scale(float sx, float sy)
{
    SDL::sdl.scale(sx, sy);
}
void clear()
{
    SDL::sdl.clear();
}

void render()
{
    SDL::sdl.render();
}
void rect(int x, int y, int w, int h)
{
    SDL::sdl.rect(x,y,w,h);
}


