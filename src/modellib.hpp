//      modellib.hpp
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


#ifndef MODELLIB_HPP
#define MODELLIB_HPP

#include "model.hpp"
#include <map>

class ModelLib
{
    public:
        ModelLib();
        virtual ~ModelLib();

        void loadLib(const char *p);
        void display(const std::string &modelName, GLfloat scale = 1.0);

    private:
        std::map<std::string, Model *> model;
};

#endif /* MODELLIB_HPP */