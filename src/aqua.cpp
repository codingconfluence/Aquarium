//      aqua.cpp
//
//      Copyright 2010 Tomasz Maciejewski <ponton@jabster.pl>
//
//      This program is free software; you can redistribute it and/or modify
//      it under the terms of the GNU General Public License as published by
//      the Free Software Foundation; either version 2 of the License, or
//      (at your option) any later version.
//
//      This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY; without even the implied warranty of
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//      GNU General Public License for more details.
//
//      You should have received a copy of the GNU General Public License
//      along with this program; if not, write to the Free Software
//      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
//      MA 02110-1301, USA.

#include <unistd.h>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <GL/glu.h>
#include <GL/gl.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "modellib.hpp"
#include "aquarium.hpp"

ModelLib modelLib;
Aquarium aquarium;
unsigned int width = 640, height = 480;
SDL_Renderer *render = NULL;

typedef enum
{
    xKey_START = 512,
    xKEY_LEFT,
    xKEY_RIGHT,
    xKEY_PAGEUP,
    xKEY_PAGEDOWN,
    xKEY_UP,
    xKEY_DOWN,
    xKEY_TAB,
}xKey_e;

inline static int xKey_remap( int old )
{
    int x = 0;
    int max = 512;
    
    switch (old)
        {
        case SDLK_LEFT:
            x = xKEY_LEFT;
            break;
        case SDLK_RIGHT:
            x = xKEY_RIGHT;
            break;
        case SDLK_PAGEUP:
            x = xKEY_PAGEUP;
            break;
        case SDLK_PAGEDOWN:
            x = xKEY_PAGEDOWN;
            break;
        case SDLK_UP:
            x = xKEY_UP;
            break;
        case SDLK_DOWN:
            x = xKEY_DOWN;
            break;
        case SDLK_TAB:
            x = xKEY_TAB;
            break;
        default :
            if ( (old<max) && (old>=0) )
                x = old;
            else
                x = 0;
            break;
        }
    
    return x;
}


std::vector<bool> keyPressed(1024, false);
SDL_Surface *surface;
bool gamePause, collisions = true, FPP;
unsigned activeFish;
std::string model = "clownfish";
GLfloat scale = 1.0;

class Camera
{
    public:
        GLfloat x, y, z, hAngle, vAngle;

        Camera() : x(0.0), y(0.0), z(0.0), hAngle(0.0), vAngle(0.0)
        {
        }

        void set(const Fish *f = NULL)
        {
            GLfloat tx = x, ty = y, tz = z, va = vAngle, ha = hAngle;

            if (f)
            {
                const GLfloat *coord = f->getXYZ(), *angle = f->getAngle();

                tx = coord[0];
                ty = coord[1];
                tz = coord[2];

                va = angle[1];
                ha = -angle[0] -M_PI_2;
            }

            glRotatef(-va * (180.0 / M_PI), 1.0, 0.0, 0.0);
            glRotatef(ha * (180.0 / M_PI), 0.0, 1.0, 0.0);
            glTranslatef(-tx, -ty, -tz);
        }

        void move(GLfloat len = 0.1)
        {
            y += len * sin(vAngle);
            x += len * cos(vAngle)*sin(hAngle);
            z -= len * cos(vAngle)*cos(hAngle);
        }

} camera;

SDL_Window  *g_window;

SDL_Surface * setVideoMode()
{
    
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    int flags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL;

    
    g_window = SDL_CreateWindow( "Aquarium", 100, 100, width, height, flags );
    if ( !g_window ) {
            std::cout << "Error creating window: " << SDL_GetError()  << std::endl;
            SDL_Quit();
            exit(-1);
    }

       render = SDL_CreateRenderer(g_window,-1,0);
    if (!render)
    {
        std::cout << "render failed, exit\n"  << std::endl;
        SDL_Quit();
        exit(-1);

    }

    if ((surface = SDL_GetWindowSurface( g_window )) == NULL)
    {
        std::cerr << "Unable to create OpenGL screen: " << SDL_GetError() << '\n';
        SDL_Quit();
        exit(-1);
    }

     SDL_GLContext glContext = SDL_GL_CreateContext(g_window);

            if( glContext == NULL )
             {
                 // Display error message
                std::cerr << "SDL_GL_CreateContext error :" << SDL_GetError() << '\n';
                exit(-1);

                return NULL;
             }
            else
             {
                 // Initialize glew
                // glewInit();
             }

    return surface;
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    if (FPP)
    {
        aquarium.setActive(activeFish);
        camera.set(aquarium.getFish(activeFish));
    }
    else
    {
        aquarium.setActive();
        camera.set();
    }

    aquarium.display();

    SDL_GL_SwapWindow(g_window);
}

void initGL()
{
    glClearColor(0.3, 0.3, 0.8, 0.0);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHT0);


    GLfloat light_ambient[] = {0.3, 0.3, 0.3, 1.0};
    GLfloat light_diffuse[] = {0.5, 0.5, 0.5, 1.0};

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);

    //glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.05);
    //glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.1);

    GLfloat mat_ambient[] = {0.5, 0.5, 0.5, 1.0};
    GLfloat mat_diffuse[] = {0.9, 0.9, 0.9, 1.0};

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glShadeModel(GL_SMOOTH);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}

void resize(int w, int h)
{
    width = w;
    height = h;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, width/height, 0.01, 1000.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void keyboard()
{
    if(keyPressed['1'])
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    if(keyPressed['2'])
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    if(keyPressed['l'])
    {
        GLboolean lighting;
        glGetBooleanv(GL_LIGHTING, &lighting);
        lighting = !lighting;
        if (lighting)
            glEnable(GL_LIGHTING);
        else
            glDisable(GL_LIGHTING);

        keyPressed['l'] = false;
    }

    if(keyPressed['t'])
    {
        GLboolean textures;
        glGetBooleanv(GL_TEXTURE_2D, &textures);
        textures = !textures;
        if (textures)
            glEnable(GL_TEXTURE_2D);
        else
            glDisable(GL_TEXTURE_2D);

        keyPressed['t'] = false;
    }

    if (keyPressed[xKEY_LEFT])
    {
        if (!FPP)
        {
            GLfloat tmpAngle = camera.vAngle;
            camera.vAngle = 0.0;
            camera.hAngle -= M_PI_2;
            camera.move();
            camera.hAngle += M_PI_2;
            camera.vAngle = tmpAngle;
        }
        else if (activeFish > 0)
        {
            --activeFish;
            keyPressed[xKEY_LEFT] = false;
        }
    }

    if (keyPressed[xKEY_RIGHT])
    {
        if (!FPP)
        {
            GLfloat tmpAngle = camera.vAngle;
            camera.vAngle = 0.0;
            camera.hAngle += M_PI_2;
            camera.move();
            camera.hAngle -= M_PI_2;
            camera.vAngle = tmpAngle;
        }
        else
        {
            ++activeFish;
            keyPressed[xKEY_RIGHT] = false;
        }
    }

    if (keyPressed[xKEY_PAGEUP])
    {
        GLfloat tmpAngle = camera.vAngle;
        camera.vAngle = M_PI_2;
        camera.move();
        camera.vAngle = tmpAngle;
    }

    if (keyPressed[xKEY_PAGEDOWN])
    {
        GLfloat tmpAngle = camera.vAngle;
        camera.vAngle = -M_PI_2;
        camera.move();
        camera.vAngle = tmpAngle;
    }

    if (keyPressed[xKEY_UP])
        camera.move();

    if (keyPressed[xKEY_DOWN])
    {
        camera.hAngle += M_PI;
        camera.vAngle = -camera.vAngle;
        camera.move();
        camera.hAngle -= M_PI;
        camera.vAngle = -camera.vAngle;
    }

    if (keyPressed['p'])
    {
        gamePause = !gamePause;
        keyPressed['p'] = false;
    }

    if (keyPressed['c'])
    {
        collisions = !collisions;
        keyPressed['c'] = false;
    }

    if (keyPressed['='])
    {
        aquarium.addFish(modelLib[model], scale);
        keyPressed['='] = false;
    }

    if (keyPressed['-'])
    {
        aquarium.removeFish(5);
        keyPressed['-'] = false;
    }

    if (keyPressed[xKEY_TAB])
    {
        FPP = !FPP;
        keyPressed[xKEY_TAB] = false;
    }
}

void update()
{
    keyboard();
    if (!gamePause)
        aquarium.update(collisions);
}

void run()
{
    SDL_Event event = {0};

    while (!keyPressed[SDLK_ESCAPE] && event.type != SDL_QUIT)
    {
        update();
        display();

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP)
            {

                int x = xKey_remap( event.key.keysym.sym );
                std::cerr << "keybord: " << event.key.keysym.sym <<" newmap=" << x << std::endl;
                keyPressed[x] = event.key.state;
            }
            else if (event.type == SDL_MOUSEMOTION)
            {
                GLfloat hAngle = -2 * M_PI + (event.motion.x * 4 * M_PI) / width,
                        vAngle = M_PI_2 + (event.motion.y * -M_PI) / height;
                camera.hAngle = hAngle;
                camera.vAngle = vAngle;
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                if (event.button.button == SDL_BUTTON_LEFT)
                    SDL_SetRelativeMouseMode(SDL_TRUE);
                else if (event.button.button == SDL_BUTTON_RIGHT)
                    SDL_SetRelativeMouseMode(SDL_FALSE);
            }
            else if (event.type == SDL_WINDOWEVENT)
            {
                if (event.window.event==SDL_WINDOWEVENT_RESIZED)
                    {
                        width = event.window.data1;
                        height = event.window.data2;
                     
                        if (surface)
                            SDL_FreeSurface(surface);

                        surface = setVideoMode();
                        resize(width, height);
                    }
                if (event.window.event==SDL_WINDOWEVENT_SIZE_CHANGED)
                    {
                        SDL_GetWindowSize(g_window,(int*)&width,(int*)&height);
                     
                        if (surface)
                            SDL_FreeSurface(surface);

                        surface = setVideoMode();
                        resize(width, height);
                    }

            }
        }

        usleep(20000);
    }
}

int main(int argc, char **argv)
{
    unsigned n = 30;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "Unable to initialize SDL: " << SDL_GetError() << '\n';
        return -1;
    }

    IMG_Init(IMG_INIT_JPG);

    surface = setVideoMode();

    initGL();
    resize(width, height);

	modelLib.loadLib("data");
    modelLib.loadLib(".");

    aquarium.init();

    if (argc > 1)
    {
        n = atoi(argv[1]);
    }

    if (argc > 2)
    {
        model = argv[2];
    }

    if (argc > 3)
    {
        scale = atof(argv[3]);
    }

    while (n > 0 && aquarium.addFish(modelLib[model], scale))
        --n;

    run();

    IMG_Quit();
    SDL_Quit();

    return EXIT_SUCCESS;
}
