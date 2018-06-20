/*
* VectorUtil is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
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
*  Loose functions for converting arrays into vectors
*/
#pragma once
#include <algorithm>
#include <vector>
#include <iterator>

namespace VectorUtil {

#define DEDUCE_TYPE(ptr) std::remove_pointer<decltype(ptr)>::type

    template<typename FromType, typename ToType> 
    struct CastTo {
        ToType operator()(FromType value) const { return static_cast<ToType>(value);}
    };

    template<typename FromType, typename ToType>
    void toType(FromType* arr, int size, std::vector<ToType>& refVec) {
        std::transform(arr, arr + size, std::back_inserter(refVec), CastTo<FromType, ToType>());
    }
}