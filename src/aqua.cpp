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
#include <GL/glut.h>
#include "model.hpp"

#include "../config.h"

Model fish;
unsigned width = 640, height = 480;
GLfloat angle;

void cube()
{
    glBegin(GL_QUADS);
        // Front Face
        glNormal3f( 0.0f, 0.0f, 0.5f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
        // Back Face
        glNormal3f( 0.0f, 0.0f,-0.5f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
        // Top Face
        glNormal3f( 0.0f, 0.5f, 0.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
        // Bottom Face
        glNormal3f( 0.0f,-0.5f, 0.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
        // Right Face
        glNormal3f( 0.5f, 0.0f, 0.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
        // Left Face
        glNormal3f(-0.5f, 0.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
    glEnd();
}

void display()
{
    const GLfloat scale = 10.0;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glTranslatef(0.0, -1.0, -6.0);
    glRotatef(angle, 0.0, 1.0, 0.0);

    glScalef(scale, scale, scale);

    glColor3f(1.0, 1.0, 1.0);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(4, GL_FLOAT, sizeof(GLfloat) * fish.getStride(), fish.getBuffer());
    glDrawArrays(GL_TRIANGLES, 0, fish.getCount());
    glDisableClientState(GL_VERTEX_ARRAY);

    GLint mode[2];
    glGetIntegerv(GL_POLYGON_MODE, mode);

    if (mode[0] == GL_LINE)
    {
        static GLubyte indices[] = {0, 1, 2, 3,
                                      4, 5, 1, 0,
                                      5, 6, 2, 1,
                                      6, 7, 3, 2,
                                      7, 4, 0, 3,
                                      5, 4, 7, 6};

        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, fish.getBoundingBox());
        glDrawElements(GL_QUADS, 24, GL_UNSIGNED_BYTE, indices);
        glDisableClientState(GL_VERTEX_ARRAY);
    }

    glutSwapBuffers();
}

void idle()
{
    angle += 1.0;

    if (angle > 360)
        angle -= 360;

    display();

    usleep(20000);
}

void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 27:  // The escape key
        case 'Q':
        case 'q':
            exit(0);   // Simply exit
            break;

        case '1':
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            break;

        case '2':
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            break;

        case 'l':
            {
                GLboolean lighting;

                glGetBooleanv(GL_LIGHTING, &lighting);

                if (!lighting)
                    glEnable(GL_LIGHTING);
                else
                    glDisable(GL_LIGHTING);
            }
    }
}

void initGL()
{
    GLfloat mat_specular[] = {1.0, 1.0, 0.0, 1.0};
    GLfloat mat_shininess[] = {50.0};

    GLfloat light_ambient[] = {0.2, 0.2, 0.0, 1.0};
    GLfloat light_diffuse[] = {0.5, 0.5, 0.0, 1.0};
    GLfloat light_specular[] = {0.8, 0.8, 8.0, 1.0};
    GLfloat light_position[] = {-1.0, 0.0, -6.0, 1.0};

    glClearColor(0.3, 0.3, 0.8, 0.0);
    glShadeModel(GL_SMOOTH);

    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
    glEnable(GL_COLOR_MATERIAL);

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.4);
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.1);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glDisable(GL_CULL_FACE);
}

void resize(int w, int h)
{
    width = w;
    height = h;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, width/height, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(width, height);
    glutCreateWindow(PACKAGE_STRING);

    glutDisplayFunc(display);
    glutReshapeFunc(resize);
    glutIdleFunc(idle);
    glutKeyboardFunc(keyboard);

    initGL();

    fish.loadObj(DATADIR "/clownfish/clownfish.obj");
    //fish.loadObj("goldenfish.obj");

    glutMainLoop();

    return EXIT_SUCCESS;
}
