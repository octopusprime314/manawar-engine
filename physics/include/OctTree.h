/*
* OctTree is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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
*  OctTree class. OctTree balances data on a tree structure.
*/
#pragma once
#include "OctNode.h"


template<typename T>
class OctTree {

    OctNode<T>* _root;

public:
    OctTree() {};
    ~OctTree() {};
    OctNode<T>* getRoot() { return _root; }
    OctNode<T>* insert(OctNode<T>* node, T data) {

        if (node != nullptr) {
            std::vector<OctNode<T>*>& children = node->getChildren();
            int childCount = 0;
            for (OctNode<T>* child : children) {
                if (child == nullptr) {
                    //When an open space in the oct node is found break out and return new node
                    return node->insert(data, childCount);
                }
                childCount++;
            }
        }
        else{
            _root = new OctNode<T>(data);
            return _root;
        }
        return nullptr;
    }
    void display(OctNode<T>* node = nullptr) {

        if (node == nullptr) {
            node = _root;
        }
        std::cout << node->getData() << std::endl;
        vector<OctNode<T>*>& children = node->getChildren();
        for (OctNode<T>* child : children) {
            display(child);
        }
    }
};