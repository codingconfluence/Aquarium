//      aquarium.hpp
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


#ifndef AQUARIUM_HPP
#define AQUARIUM_HPP

#include <vector>
#include <cstdlib>
#include <ctime>
#include "fish.hpp"

class Aquarium
{
    public:
        Aquarium(GLuint width = 50.0, GLuint height = 30.0,
                   GLuint depth = 20.0);
        virtual ~Aquarium();

        void display() const;
        void update();

        bool addFish(const Model *model, GLfloat scale = 1.0);
        void addFish(Fish *f);

        void init();

    private:
        std::vector<Fish *> fish;
        GLfloat size[3];
        GLint timeLocation;
        GLuint program, texture[3];

        void textureFromImg(GLuint t, const char *filename);
        void displayAquarium() const;
        bool collides(const Fish *f) const;
        void initShader();
        void displaySquare(GLfloat width, GLfloat height) const;
};

#endif /* AQUARIUM_HPP */
