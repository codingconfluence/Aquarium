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
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include "modellib.hpp"
#include "aquarium.hpp"
#include "../config.h"

ModelLib modelLib;
Aquarium aquarium;
unsigned width = 640, height = 480;

std::vector<bool> keyPressed(SDLK_LAST);
SDL_Surface *surface;
bool lighting = false, gamePause = false;
GLfloat light_position[] = {0.0, 0.0, 15.0, 1.0};

class Camera
{
    public:
        GLfloat x, y, z, hAngle, vAngle;

        Camera() : x(0.0), y(0.0), z(0.0), hAngle(0.0), vAngle(0.0)
        {
        }

        void set()
        {
            glRotatef(-vAngle * (180.0 / M_PI), 1.0, 0.0, 0.0);
            glRotatef(hAngle * (180.0 / M_PI), 0.0, 1.0, 0.0);
            glTranslatef(-x, -y, -z);
        }

        void move(GLfloat len = 0.5)
        {
            y += len * sin(vAngle);
            x += len * cos(vAngle)*sin(hAngle);
            z -= len * cos(vAngle)*cos(hAngle);
        }

} camera;

SDL_Surface * setVideoMode()
{
    SDL_Surface *surface;
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    int flags = SDL_OPENGL | SDL_RESIZABLE | SDL_DOUBLEBUF;
    if ((surface = SDL_SetVideoMode(width, height, 0, flags)) == NULL)
    {
        std::cerr << "Unable to create OpenGL screen: " << SDL_GetError() << '\n';
        SDL_Quit();
        exit(-1);
    }

    return surface;
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    camera.set();
    aquarium.display();

    SDL_GL_SwapBuffers();
}

void initGL()
{
    glClearColor(0.3, 0.3, 0.8, 0.0);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);

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
    gluPerspective(60.0, width/height, 0.01, 100.0);
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
        lighting = !lighting;
        keyPressed['l'] = false;
    }

    if (keyPressed[SDLK_LEFT])
    {
        GLfloat tmpAngle = camera.vAngle;
        camera.vAngle = 0.0;
        camera.hAngle -= M_PI_2;
        camera.move();
        camera.hAngle += M_PI_2;
        camera.vAngle = tmpAngle;
    }

    if (keyPressed[SDLK_RIGHT])
    {
        GLfloat tmpAngle = camera.vAngle;
        camera.vAngle = 0.0;
        camera.hAngle += M_PI_2;
        camera.move();
        camera.hAngle -= M_PI_2;
        camera.vAngle = tmpAngle;
    }

    if (keyPressed[SDLK_PAGEUP])
    {
        GLfloat tmpAngle = camera.vAngle;
        camera.vAngle = M_PI_2;
        camera.move();
        camera.vAngle = tmpAngle;
    }

    if (keyPressed[SDLK_PAGEDOWN])
    {
        GLfloat tmpAngle = camera.vAngle;
        camera.vAngle = -M_PI_2;
        camera.move();
        camera.vAngle = tmpAngle;
    }

    if (keyPressed[SDLK_UP])
        camera.move(0.5);

    if (keyPressed[SDLK_DOWN])
        camera.move(-.5);

    if (keyPressed['p'])
    {
        gamePause = !gamePause;
        keyPressed['p'] = false;
    }
}

void update()
{
    keyboard();
    if (!gamePause)
        aquarium.update();
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
                keyPressed[event.key.keysym.sym] = event.key.state;
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
                    SDL_WM_GrabInput(SDL_GRAB_ON);
                else if (event.button.button == SDL_BUTTON_RIGHT)
                    SDL_WM_GrabInput(SDL_GRAB_OFF);
            }
            else if (event.type == SDL_VIDEORESIZE)
            {
                width = event.resize.w;
                height = event.resize.h;

                if (surface)
                    SDL_FreeSurface(surface);

                surface = setVideoMode();
                resize(width, height);
            }
        }

        usleep(20000);
    }
}

int main(int argc, char **argv)
{
    std::string model = "clownfish";
    GLfloat scale = 7.0;
    unsigned n = 15;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "Unable to initialize SDL: " << SDL_GetError() << '\n';
        return -1;
    }

    IMG_Init(IMG_INIT_JPG);

    surface = setVideoMode();

    SDL_WM_SetCaption(PACKAGE_STRING, NULL);

    initGL();
    resize(640, 480);

    modelLib.loadLib(DATADIR);
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

    for (int i = 0; i < n; ++i)
    {
        aquarium.addFish(modelLib[model], scale);
    }

    run();

    IMG_Quit();
    SDL_Quit();

    return EXIT_SUCCESS;
}
