#ifndef DRAWINGUTILITY_H
#define DRAWINGUTILITY_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector>

using std::string;
using std::vector;

//SDL:

class InitError : public std::exception
{
    string a;
    std::string msg;
public:
    InitError();
    InitError( const std::string & );
    virtual ~InitError() throw();
    virtual const char * what() const throw();
};

struct Vector2
{
    float x;
    float y;
};
struct Transformation
{
    SDL_Rect translation;
    Vector2 scale;
};
class SDL
{
    SDL_Window * m_window;
    SDL_Renderer * m_renderer;

    SDL_Rect m_translation;
    Vector2 m_scale;

    vector<Transformation> m_transformations;
public:
    SDL( Uint32 flags = 0 );
    static SDL sdl;//instance
    virtual ~SDL();

    SDL_Rect GetCurrentTranslation();
    Vector2 GetCurrentScale();

    void draw();

    void clear();
    void render();
    void rect(int x, int y, int w, int h);
    void text(string text, int x, int y, int w, int h);
    void text(string text, int x, int y);
    void triangle(int x0, int y0, int x1, int y1, int x2, int y2);
    void line(int x0, int y0, int x1, int y1);

    void pushMatrix();
    void popMatrix();

    void translate(int x, int y);
    void scale(float sx, float sy);
};


void clear();
void render();

void fill(SDL_Color color, int alpha = 255);

void stroke(SDL_Color color, int alpha = 255);
void rect(int x, int y, int w, int h);

void textFont(TTF_Font*);
void text(string text, int x, int y, int w, int h);
void text(string text, int x, int y);

void triangle(int x0, int y0, int x1, int y1, int x2, int y2);

void line(int x0, int y0, int x1, int y1);


void pushMatrix();
void popMatrix();

void translate(int x, int y);
void scale(float sx, float sy);

#endif // DRAWINGUTILITY_H
