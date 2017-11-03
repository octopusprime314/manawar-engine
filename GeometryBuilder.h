/*
* GeometryBuilder is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
* Copyright (c) 2017 Peter Morley.
*
* ReBoot is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, version 3.
*
* ReBoot is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

/**
*  GeometryBuilder namespace. Creates generic sample geometry and pushes
*  the data to the passed model
*/

#pragma once
#include "Model.h"

namespace GeometryBuilder {

    void buildCube(Model* model) {

        //Cube vertices modeled around origin (0,0,0) with length,width,height of 2

        //xy plane triangle top + z
        model->addVertex(Vector4(1.0, 1.0, 1.0, 1.0));
        model->addVertex(Vector4(-1.0, -1.0, 1.0, 1.0));
        model->addVertex(Vector4(-1.0, 1.0, 1.0, 1.0));

        //xy plane triangle bottom + z
        model->addVertex(Vector4(1.0, 1.0, 1.0, 1.0));
        model->addVertex(Vector4(-1.0, -1.0, 1.0, 1.0));
        model->addVertex(Vector4(1.0, -1.0, 1.0, 1.0));

        //xy plane triangle top - z
        model->addVertex(Vector4(1.0, 1.0, -1.0, 1.0));
        model->addVertex(Vector4(-1.0, -1.0, -1.0, 1.0));
        model->addVertex(Vector4(-1.0, 1.0, -1.0, 1.0));

        //xy plane triangle bottom - z
        model->addVertex(Vector4(1.0, 1.0, -1.0, 1.0));
        model->addVertex(Vector4(-1.0, -1.0, -1.0, 1.0));
        model->addVertex(Vector4(1.0, -1.0, -1.0, 1.0));

        //zy plane triangle top + x
        model->addVertex(Vector4(1.0, 1.0, 1.0, 1.0));
        model->addVertex(Vector4(1.0, -1.0, 1.0, 1.0));
        model->addVertex(Vector4(1.0, -1.0, -1.0, 1.0));

        //zy plane triangle bottom + x
        model->addVertex(Vector4(1.0, 1.0, 1.0, 1.0));
        model->addVertex(Vector4(1.0, 1.0, -1.0, 1.0));
        model->addVertex(Vector4(1.0, -1.0, -1.0, 1.0));

        //zy plane triangle top - x
        model->addVertex(Vector4(-1.0, 1.0, 1.0, 1.0));
        model->addVertex(Vector4(-1.0, 1.0, -1.0, 1.0));
        model->addVertex(Vector4(-1.0, -1.0, -1.0, 1.0));

        //zy plane triangle bottom - x
        model->addVertex(Vector4(-1.0, 1.0, 1.0, 1.0));
        model->addVertex(Vector4(-1.0, -1.0, 1.0, 1.0));
        model->addVertex(Vector4(-1.0, -1.0, -1.0, 1.0));

        //zx plane triangle top + y
        model->addVertex(Vector4(1.0, 1.0, 1.0, 1.0));
        model->addVertex(Vector4(-1.0, 1.0, 1.0, 1.0));
        model->addVertex(Vector4(1.0, 1.0, -1.0, 1.0));

        //zx plane triangle top + y
        model->addVertex(Vector4(1.0, 1.0, -1.0, 1.0));
        model->addVertex(Vector4(-1.0, 1.0, -1.0, 1.0));
        model->addVertex(Vector4(-1.0, 1.0, 1.0, 1.0));

        //zx plane triangle bottom - y
        model->addVertex(Vector4(1.0, -1.0, 1.0, 1.0));
        model->addVertex(Vector4(-1.0, -1.0, 1.0, 1.0));
        model->addVertex(Vector4(1.0, -1.0, -1.0, 1.0));

        //zx plane triangle bottom - y
        model->addVertex(Vector4(1.0, -1.0, -1.0, 1.0));
        model->addVertex(Vector4(-1.0, -1.0, -1.0, 1.0));
        model->addVertex(Vector4(-1.0, -1.0, 1.0, 1.0));

        //Normal per vertex ratio

        //xy plane triangle top + z
        model->addNormal(Vector4(0.0, 0.0, 1.0, 1.0));
        model->addNormal(Vector4(0.0, 0.0, 1.0, 1.0));
        model->addNormal(Vector4(0.0, 0.0, 1.0, 1.0));

        //xy plane triangle bottom + z
        model->addNormal(Vector4(0.0, 0.0, 1.0, 1.0));
        model->addNormal(Vector4(0.0, 0.0, 1.0, 1.0));
        model->addNormal(Vector4(0.0, 0.0, 1.0, 1.0));

        //xy plane triangle top - z
        model->addNormal(Vector4(0.0, 0.0, -1.0, 1.0));
        model->addNormal(Vector4(0.0, 0.0, -1.0, 1.0));
        model->addNormal(Vector4(0.0, 0.0, -1.0, 1.0));

        //xy plane triangle bottom - z
        model->addNormal(Vector4(0.0, 0.0, -1.0, 1.0));
        model->addNormal(Vector4(0.0, 0.0, -1.0, 1.0));
        model->addNormal(Vector4(0.0, 0.0, -1.0, 1.0));

        //zy plane triangle top + x
        model->addNormal(Vector4(1.0, 0.0, 0.0, 1.0));
        model->addNormal(Vector4(1.0, 0.0, 0.0, 1.0));
        model->addNormal(Vector4(1.0, 0.0, 0.0, 1.0));

        //zy plane triangle bottom + x
        model->addNormal(Vector4(1.0, 0.0, 0.0, 1.0));
        model->addNormal(Vector4(1.0, 0.0, 0.0, 1.0));
        model->addNormal(Vector4(1.0, 0.0, 0.0, 1.0));

        //zy plane triangle top - x
        model->addNormal(Vector4(-1.0, 0.0, 0.0, 1.0));
        model->addNormal(Vector4(-1.0, 0.0, 0.0, 1.0));
        model->addNormal(Vector4(-1.0, 0.0, 0.0, 1.0));

        //zy plane triangle bottom - x
        model->addNormal(Vector4(-1.0, 0.0, 0.0, 1.0));
        model->addNormal(Vector4(-1.0, 0.0, 0.0, 1.0));
        model->addNormal(Vector4(-1.0, 0.0, 0.0, 1.0));

        //zx plane triangle top + y
        model->addVertex(Vector4(0.0, 1.0, 0.0, 1.0));
        model->addVertex(Vector4(0.0, 1.0, 0.0, 1.0));
        model->addVertex(Vector4(0.0, 1.0, 0.0, 1.0));

        //zx plane triangle top + y
        model->addVertex(Vector4(0.0, 1.0, 0.0, 1.0));
        model->addVertex(Vector4(0.0, 1.0, 0.0, 1.0));
        model->addVertex(Vector4(0.0, 1.0, 0.0, 1.0));

        //zx plane triangle bottom - y
        model->addVertex(Vector4(0.0, -1.0, 0.0, 1.0));
        model->addVertex(Vector4(0.0, -1.0, 0.0, 1.0));
        model->addVertex(Vector4(0.0, -1.0, 0.0, 1.0));

        //zx plane triangle bottom - y
        model->addVertex(Vector4(0.0, -1.0, 0.0, 1.0));
        model->addVertex(Vector4(0.0, -1.0, 0.0, 1.0));
        model->addVertex(Vector4(0.0, -1.0, 0.0, 1.0));


        //Lines for normal visualization
        std::vector<Vector4> vertices = model->getVertices();
        std::vector<Vector4> normals = model->getNormals();
        size_t length = vertices.size();
        for (int i = 0; i < length; ++i) {
            model->addDebugNormal(vertices[i]);
            model->addDebugNormal(vertices[i] + normals[i]);
        }

    }
}