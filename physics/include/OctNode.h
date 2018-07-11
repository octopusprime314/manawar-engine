/*
* OctNode is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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
*  OctNode class. Node contains 8 child nodes
*/
#pragma once
#include "Triangle.h"
#include "Sphere.h"
#include <unordered_map>
#include "Entity.h"
#include <set>
#include <vector>

template<typename T>
class OctNode {
    std::vector<OctNode*> _children; //8 children in an oct node
    T                     _data;

    //Maps models to a list of triangles in the oct node
    //if a model's triangles are involved in a collision then changes can be propogated back to the model
    std::unordered_map<Entity*, std::set<Triangle*>> _triangles;
    //Maps models to a list of spheres in the oct node
    //if a model's spheres are involved in a collision then changes can be propogated back to the model
    std::unordered_map<Entity*, std::set<Sphere*>> _spheres;

public:
    OctNode(T data) : _data(data) {
        _children.resize(8, nullptr);
    }
    OctNode* insert(T data, int index) {
        _children[index] = new OctNode(data);
        return _children[index];
    }
    void addGeometry(Entity* entity, Triangle* triangle) {
        _triangles[entity].insert(triangle);
    }
    void addGeometry(Entity* entity, Sphere* sphere) {
        _spheres[entity].insert(sphere);
    }
    void removeGeometry(Entity* entity, Triangle* triangle) {
        _triangles[entity].erase(triangle);
    }
    void removeGeometry(Entity* entity, Sphere* sphere) {
        _spheres[entity].erase(sphere);
    }
    std::unordered_map<Entity*, std::set<Triangle*>>* getTriangles() {
        return &_triangles;
    }
    std::unordered_map<Entity*, std::set<Sphere*>>* getSpheres() {
        return &_spheres;
    }
    OctNode<T>* getChild(int index) { return _children[index]; }
    T getData() { return _data; }

    std::vector<OctNode<T>*>& getChildren() { return _children; }

};