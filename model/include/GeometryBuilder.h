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
#include "Sphere.h"
#include "Cube.h"
#include <vector>

namespace GeometryBuilder {

    float* buildCubes(std::vector<Cube>*   cubes,
                      GLuint&              vertexCount,
                      GeometryConstruction geometryType) {

        //Making lines for cubes!!!!!
        unsigned long long vertices = 0;
        if (geometryType == GeometryConstruction::LINE_WIREFRAME) {
            vertices = cubes->size() * 24;
        }
        else if (geometryType == GeometryConstruction::TRIANGLE_MESH) {
            vertices = cubes->size() * 36;
        }
        vertexCount         = static_cast<GLuint>(vertices);
        //Now flatten vertices and normals out for opengl
        size_t triBuffSize  = vertices * 3;
        float* flattenVerts = new float[triBuffSize]; //Only include the x y and z values not w

        int i = 0; //iterates through vertices indexes
        for (auto cube : *cubes) {
            
            auto center = cube.getCenter();

            auto a = center + Vector4(-cube.getLength() / 2.0f, -cube.getHeight() / 2.0f, -cube.getWidth() / 2.0f);
            auto b = center + Vector4( cube.getLength() / 2.0f, -cube.getHeight() / 2.0f, -cube.getWidth() / 2.0f);
            auto c = center + Vector4( cube.getLength() / 2.0f, -cube.getHeight() / 2.0f,  cube.getWidth() / 2.0f);
            auto d = center + Vector4(-cube.getLength() / 2.0f, -cube.getHeight() / 2.0f,  cube.getWidth() / 2.0f);
            auto e = center + Vector4(-cube.getLength() / 2.0f,  cube.getHeight() / 2.0f, -cube.getWidth() / 2.0f);
            auto f = center + Vector4( cube.getLength() / 2.0f,  cube.getHeight() / 2.0f, -cube.getWidth() / 2.0f);
            auto g = center + Vector4( cube.getLength() / 2.0f,  cube.getHeight() / 2.0f,  cube.getWidth() / 2.0f);
            auto h = center + Vector4(-cube.getLength() / 2.0f,  cube.getHeight() / 2.0f,  cube.getWidth() / 2.0f);


            if (geometryType == GeometryConstruction::LINE_WIREFRAME) {
                
                flattenVerts[i++] = a.getx();
                flattenVerts[i++] = a.gety();
                flattenVerts[i++] = a.getz();
                flattenVerts[i++] = b.getx();
                flattenVerts[i++] = b.gety();
                flattenVerts[i++] = b.getz();

                flattenVerts[i++] = b.getx();
                flattenVerts[i++] = b.gety();
                flattenVerts[i++] = b.getz();
                flattenVerts[i++] = c.getx();
                flattenVerts[i++] = c.gety();
                flattenVerts[i++] = c.getz();

                flattenVerts[i++] = c.getx();
                flattenVerts[i++] = c.gety();
                flattenVerts[i++] = c.getz();
                flattenVerts[i++] = d.getx();
                flattenVerts[i++] = d.gety();
                flattenVerts[i++] = d.getz();

                flattenVerts[i++] = d.getx();
                flattenVerts[i++] = d.gety();
                flattenVerts[i++] = d.getz();
                flattenVerts[i++] = a.getx();
                flattenVerts[i++] = a.gety();
                flattenVerts[i++] = a.getz();


                flattenVerts[i++] = e.getx();
                flattenVerts[i++] = e.gety();
                flattenVerts[i++] = e.getz();
                flattenVerts[i++] = f.getx();
                flattenVerts[i++] = f.gety();
                flattenVerts[i++] = f.getz();

                flattenVerts[i++] = f.getx();
                flattenVerts[i++] = f.gety();
                flattenVerts[i++] = f.getz();
                flattenVerts[i++] = g.getx();
                flattenVerts[i++] = g.gety();
                flattenVerts[i++] = g.getz();

                flattenVerts[i++] = g.getx();
                flattenVerts[i++] = g.gety();
                flattenVerts[i++] = g.getz();
                flattenVerts[i++] = h.getx();
                flattenVerts[i++] = h.gety();
                flattenVerts[i++] = h.getz();

                flattenVerts[i++] = h.getx();
                flattenVerts[i++] = h.gety();
                flattenVerts[i++] = h.getz();
                flattenVerts[i++] = e.getx();
                flattenVerts[i++] = e.gety();
                flattenVerts[i++] = e.getz();


                flattenVerts[i++] = a.getx();
                flattenVerts[i++] = a.gety();
                flattenVerts[i++] = a.getz();
                flattenVerts[i++] = e.getx();
                flattenVerts[i++] = e.gety();
                flattenVerts[i++] = e.getz();

                flattenVerts[i++] = b.getx();
                flattenVerts[i++] = b.gety();
                flattenVerts[i++] = b.getz();
                flattenVerts[i++] = f.getx();
                flattenVerts[i++] = f.gety();
                flattenVerts[i++] = f.getz();

                flattenVerts[i++] = c.getx();
                flattenVerts[i++] = c.gety();
                flattenVerts[i++] = c.getz();
                flattenVerts[i++] = g.getx();
                flattenVerts[i++] = g.gety();
                flattenVerts[i++] = g.getz();

                flattenVerts[i++] = d.getx();
                flattenVerts[i++] = d.gety();
                flattenVerts[i++] = d.getz();
                flattenVerts[i++] = h.getx();
                flattenVerts[i++] = h.gety();
                flattenVerts[i++] = h.getz();
            }
            else if (geometryType == GeometryConstruction::TRIANGLE_MESH){
                
                flattenVerts[i++] = a.getx();
                flattenVerts[i++] = a.gety();
                flattenVerts[i++] = a.getz();
                flattenVerts[i++] = b.getx();
                flattenVerts[i++] = b.gety();
                flattenVerts[i++] = b.getz();
                flattenVerts[i++] = c.getx();
                flattenVerts[i++] = c.gety();
                flattenVerts[i++] = c.getz();

                flattenVerts[i++] = a.getx();
                flattenVerts[i++] = a.gety();
                flattenVerts[i++] = a.getz();
                flattenVerts[i++] = c.getx();
                flattenVerts[i++] = c.gety();
                flattenVerts[i++] = c.getz();
                flattenVerts[i++] = d.getx();
                flattenVerts[i++] = d.gety();
                flattenVerts[i++] = d.getz();


                flattenVerts[i++] = e.getx();
                flattenVerts[i++] = e.gety();
                flattenVerts[i++] = e.getz();
                flattenVerts[i++] = f.getx();
                flattenVerts[i++] = f.gety();
                flattenVerts[i++] = f.getz();
                flattenVerts[i++] = g.getx();
                flattenVerts[i++] = g.gety();
                flattenVerts[i++] = g.getz();

                flattenVerts[i++] = e.getx();
                flattenVerts[i++] = e.gety();
                flattenVerts[i++] = e.getz();
                flattenVerts[i++] = g.getx();
                flattenVerts[i++] = g.gety();
                flattenVerts[i++] = g.getz();
                flattenVerts[i++] = h.getx();
                flattenVerts[i++] = h.gety();
                flattenVerts[i++] = h.getz();

                flattenVerts[i++] = b.getx();
                flattenVerts[i++] = b.gety();
                flattenVerts[i++] = b.getz();
                flattenVerts[i++] = c.getx();
                flattenVerts[i++] = c.gety();
                flattenVerts[i++] = c.getz();
                flattenVerts[i++] = f.getx();
                flattenVerts[i++] = f.gety();
                flattenVerts[i++] = f.getz();

                flattenVerts[i++] = f.getx();
                flattenVerts[i++] = f.gety();
                flattenVerts[i++] = f.getz();
                flattenVerts[i++] = g.getx();
                flattenVerts[i++] = g.gety();
                flattenVerts[i++] = g.getz();
                flattenVerts[i++] = c.getx();
                flattenVerts[i++] = c.gety();
                flattenVerts[i++] = c.getz();

                flattenVerts[i++] = a.getx();
                flattenVerts[i++] = a.gety();
                flattenVerts[i++] = a.getz();
                flattenVerts[i++] = d.getx();
                flattenVerts[i++] = d.gety();
                flattenVerts[i++] = d.getz();
                flattenVerts[i++] = e.getx();
                flattenVerts[i++] = e.gety();
                flattenVerts[i++] = e.getz();

                flattenVerts[i++] = e.getx();
                flattenVerts[i++] = e.gety();
                flattenVerts[i++] = e.getz();
                flattenVerts[i++] = h.getx();
                flattenVerts[i++] = h.gety();
                flattenVerts[i++] = h.getz();
                flattenVerts[i++] = d.getx();
                flattenVerts[i++] = d.gety();
                flattenVerts[i++] = d.getz();

                flattenVerts[i++] = a.getx();
                flattenVerts[i++] = a.gety();
                flattenVerts[i++] = a.getz();
                flattenVerts[i++] = b.getx();
                flattenVerts[i++] = b.gety();
                flattenVerts[i++] = b.getz();
                flattenVerts[i++] = e.getx();
                flattenVerts[i++] = e.gety();
                flattenVerts[i++] = e.getz();

                flattenVerts[i++] = e.getx();
                flattenVerts[i++] = e.gety();
                flattenVerts[i++] = e.getz();
                flattenVerts[i++] = f.getx();
                flattenVerts[i++] = f.gety();
                flattenVerts[i++] = f.getz();
                flattenVerts[i++] = b.getx();
                flattenVerts[i++] = b.gety();
                flattenVerts[i++] = b.getz();

                flattenVerts[i++] = c.getx();
                flattenVerts[i++] = c.gety();
                flattenVerts[i++] = c.getz();
                flattenVerts[i++] = d.getx();
                flattenVerts[i++] = d.gety();
                flattenVerts[i++] = d.getz();
                flattenVerts[i++] = g.getx();
                flattenVerts[i++] = g.gety();
                flattenVerts[i++] = g.getz();

                flattenVerts[i++] = g.getx();
                flattenVerts[i++] = g.gety();
                flattenVerts[i++] = g.getz();
                flattenVerts[i++] = h.getx();
                flattenVerts[i++] = h.gety();
                flattenVerts[i++] = h.getz();
                flattenVerts[i++] = d.getx();
                flattenVerts[i++] = d.gety();
                flattenVerts[i++] = d.getz();

            }
            else if (geometryType == GeometryConstruction::TRIANGLE_WIREFRAME) {
                std::cout << "Triangle wireframe not supported!" << std::endl;
            }
        }
        return flattenVerts;       
    }

    float* buildTriangles(std::vector<Triangle>* triangles, GLuint& vertexCount) {
        //Making lines for triangles!!!!!
        auto vertices       = triangles->size() * 6;
        vertexCount         = static_cast<GLuint>(vertices);
        //Now flatten vertices and normals out for opengl
        size_t triBuffSize  = vertices * 3;
        float* flattenVerts = new float[triBuffSize]; //Only include the x y and z values not w

        int i = 0; //iterates through vertices indexes
        for (auto triangle : *triangles) {
            auto points = triangle.getTrianglePoints();
            flattenVerts[i++] = points[0].getx();
            flattenVerts[i++] = points[0].gety();
            flattenVerts[i++] = points[0].getz();
            flattenVerts[i++] = points[1].getx();
            flattenVerts[i++] = points[1].gety();
            flattenVerts[i++] = points[1].getz();

            flattenVerts[i++] = points[1].getx();
            flattenVerts[i++] = points[1].gety();
            flattenVerts[i++] = points[1].getz();
            flattenVerts[i++] = points[2].getx();
            flattenVerts[i++] = points[2].gety();
            flattenVerts[i++] = points[2].getz();

            flattenVerts[i++] = points[2].getx();
            flattenVerts[i++] = points[2].gety();
            flattenVerts[i++] = points[2].getz();
            flattenVerts[i++] = points[0].getx();
            flattenVerts[i++] = points[0].gety();
            flattenVerts[i++] = points[0].getz();
        }
        return flattenVerts;
    }

    float* buildSpheres(std::vector<Sphere>* spheres, GLuint& vertexCount, GeometryConstruction geometryType) {

        //Making lines for spheres!!!!!
        int stacks = 20;
        int slices = 20;
        unsigned long long vertices = 0;
        if (geometryType == GeometryConstruction::LINE_WIREFRAME) {
            vertices = spheres->size() * (((stacks - 2) * (slices) * 36) + (2 * slices * 18));
        }
        else if (geometryType == GeometryConstruction::TRIANGLE_MESH) {
            vertices = (spheres->size() * (((stacks - 2) * (slices) * 36) + (2 * slices * 18))) / 2;
        }
        vertexCount         = static_cast<GLuint>(vertices / 3);
        size_t triBuffSize  = vertices;
        float* flattenVerts = new float[triBuffSize]; //Only include the x y and z values not w
        int i               = 0;

        for (Sphere sphere : *spheres) {

            for (int t = 0; t < stacks; t++) {// stacks are ELEVATION so they count theta

                float theta1 = (static_cast<float>(t)     / stacks)*PI;
                float theta2 = (static_cast<float>(t + 1) / stacks)*PI;

                // slices are ORANGE SLICES so the count azimuth
                for (int p = 0; p < slices; p++) {

                    // azimuth goes around 0 .. 2*PI
                    float phi1 = (static_cast<float>(p)     / slices) * 2 * PI;
                    float phi2 = (static_cast<float>(p + 1) / slices) * 2 * PI;

                    //phi2   phi1
                    // |      |
                    // 2------1 -- theta1
                    // |\ _   |
                    // |    \ |
                    // 3------4 -- theta2
                    //

                    float r     = sphere.getRadius();
                    auto center = sphere.getObjectPosition();
                    Vector4 vertex1(r*sin(phi1)*cos(theta1), r*sin(phi1)*sin(theta1), r*cos(phi1));
                    Vector4 vertex2(r*sin(phi2)*cos(theta1), r*sin(phi2)*sin(theta1), r*cos(phi2));
                    Vector4 vertex3(r*sin(phi2)*cos(theta2), r*sin(phi2)*sin(theta2), r*cos(phi2));
                    Vector4 vertex4(r*sin(phi1)*cos(theta2), r*sin(phi1)*sin(theta2), r*cos(phi1));
                    vertex1 += center;
                    vertex2 += center;
                    vertex3 += center;
                    vertex4 += center;
                    //vertex1 = vertex on a sphere of radius r at spherical coords theta1, phi1
                    //vertex2 = vertex on a sphere of radius r at spherical coords theta1, phi2
                    //vertex3 = vertex on a sphere of radius r at spherical coords theta2, phi2
                    //vertex4 = vertex on a sphere of radius r at spherical coords theta2, phi1

                    if (geometryType == GeometryConstruction::LINE_WIREFRAME) {
                        // facing out
                        // top cap
                        if (t == 0) {

                            flattenVerts[i++] = vertex1.getx();
                            flattenVerts[i++] = vertex1.gety();
                            flattenVerts[i++] = vertex1.getz();
                            flattenVerts[i++] = vertex3.getx();
                            flattenVerts[i++] = vertex3.gety();
                            flattenVerts[i++] = vertex3.getz();

                            flattenVerts[i++] = vertex3.getx();
                            flattenVerts[i++] = vertex3.gety();
                            flattenVerts[i++] = vertex3.getz();
                            flattenVerts[i++] = vertex4.getx();
                            flattenVerts[i++] = vertex4.gety();
                            flattenVerts[i++] = vertex4.getz();

                            flattenVerts[i++] = vertex4.getx();
                            flattenVerts[i++] = vertex4.gety();
                            flattenVerts[i++] = vertex4.getz();
                            flattenVerts[i++] = vertex1.getx();
                            flattenVerts[i++] = vertex1.gety();
                            flattenVerts[i++] = vertex1.getz();
                        }
                        //end cap
                        else if (t + 1 == stacks) {

                            flattenVerts[i++] = vertex3.getx();
                            flattenVerts[i++] = vertex3.gety();
                            flattenVerts[i++] = vertex3.getz();
                            flattenVerts[i++] = vertex1.getx();
                            flattenVerts[i++] = vertex1.gety();
                            flattenVerts[i++] = vertex1.getz();

                            flattenVerts[i++] = vertex1.getx();
                            flattenVerts[i++] = vertex1.gety();
                            flattenVerts[i++] = vertex1.getz();
                            flattenVerts[i++] = vertex2.getx();
                            flattenVerts[i++] = vertex2.gety();
                            flattenVerts[i++] = vertex2.getz();

                            flattenVerts[i++] = vertex2.getx();
                            flattenVerts[i++] = vertex2.gety();
                            flattenVerts[i++] = vertex2.getz();
                            flattenVerts[i++] = vertex3.getx();
                            flattenVerts[i++] = vertex3.gety();
                            flattenVerts[i++] = vertex3.getz();
                        }
                        // body, facing OUT:
                        else {

                            flattenVerts[i++] = vertex1.getx();
                            flattenVerts[i++] = vertex1.gety();
                            flattenVerts[i++] = vertex1.getz();
                            flattenVerts[i++] = vertex2.getx();
                            flattenVerts[i++] = vertex2.gety();
                            flattenVerts[i++] = vertex2.getz();

                            flattenVerts[i++] = vertex2.getx();
                            flattenVerts[i++] = vertex2.gety();
                            flattenVerts[i++] = vertex2.getz();
                            flattenVerts[i++] = vertex4.getx();
                            flattenVerts[i++] = vertex4.gety();
                            flattenVerts[i++] = vertex4.getz();

                            flattenVerts[i++] = vertex4.getx();
                            flattenVerts[i++] = vertex4.gety();
                            flattenVerts[i++] = vertex4.getz();
                            flattenVerts[i++] = vertex1.getx();
                            flattenVerts[i++] = vertex1.gety();
                            flattenVerts[i++] = vertex1.getz();

                            flattenVerts[i++] = vertex2.getx();
                            flattenVerts[i++] = vertex2.gety();
                            flattenVerts[i++] = vertex2.getz();
                            flattenVerts[i++] = vertex3.getx();
                            flattenVerts[i++] = vertex3.gety();
                            flattenVerts[i++] = vertex3.getz();

                            flattenVerts[i++] = vertex3.getx();
                            flattenVerts[i++] = vertex3.gety();
                            flattenVerts[i++] = vertex3.getz();
                            flattenVerts[i++] = vertex4.getx();
                            flattenVerts[i++] = vertex4.gety();
                            flattenVerts[i++] = vertex4.getz();

                            flattenVerts[i++] = vertex4.getx();
                            flattenVerts[i++] = vertex4.gety();
                            flattenVerts[i++] = vertex4.getz();
                            flattenVerts[i++] = vertex2.getx();
                            flattenVerts[i++] = vertex2.gety();
                            flattenVerts[i++] = vertex2.getz();
                        }
                    }
                    else if (geometryType == GeometryConstruction::TRIANGLE_MESH) {
                        // facing out
                        // top cap
                        if (t == 0) {

                            flattenVerts[i++] = vertex1.getx();
                            flattenVerts[i++] = vertex1.gety();
                            flattenVerts[i++] = vertex1.getz();
                            flattenVerts[i++] = vertex3.getx();
                            flattenVerts[i++] = vertex3.gety();
                            flattenVerts[i++] = vertex3.getz();
                            flattenVerts[i++] = vertex4.getx();
                            flattenVerts[i++] = vertex4.gety();
                            flattenVerts[i++] = vertex4.getz();
                        }
                        //end cap
                        else if (t + 1 == stacks) {

                            flattenVerts[i++] = vertex3.getx();
                            flattenVerts[i++] = vertex3.gety();
                            flattenVerts[i++] = vertex3.getz();
                            flattenVerts[i++] = vertex1.getx();
                            flattenVerts[i++] = vertex1.gety();
                            flattenVerts[i++] = vertex1.getz();
                            flattenVerts[i++] = vertex2.getx();
                            flattenVerts[i++] = vertex2.gety();
                            flattenVerts[i++] = vertex2.getz();
                        }
                        // body, facing OUT:
                        else {

                            flattenVerts[i++] = vertex1.getx();
                            flattenVerts[i++] = vertex1.gety();
                            flattenVerts[i++] = vertex1.getz();
                            flattenVerts[i++] = vertex2.getx();
                            flattenVerts[i++] = vertex2.gety();
                            flattenVerts[i++] = vertex2.getz();
                            flattenVerts[i++] = vertex4.getx();
                            flattenVerts[i++] = vertex4.gety();
                            flattenVerts[i++] = vertex4.getz();

                            flattenVerts[i++] = vertex2.getx();
                            flattenVerts[i++] = vertex2.gety();
                            flattenVerts[i++] = vertex2.getz();
                            flattenVerts[i++] = vertex3.getx();
                            flattenVerts[i++] = vertex3.gety();
                            flattenVerts[i++] = vertex3.getz();
                            flattenVerts[i++] = vertex4.getx();
                            flattenVerts[i++] = vertex4.gety();
                            flattenVerts[i++] = vertex4.getz();
                        }
                    }
                    else if (geometryType == GeometryConstruction::TRIANGLE_WIREFRAME) {
                        std::cout << "Triangle wireframe not supported" << std::endl;
                    }
                }
            }
        }
        return flattenVerts;
    }
}