#include "GeometryMath.h"
#include "Sphere.h"
#include "Entity.h"

//Returns true if the sphere is not completely enclosed within a cube
bool GeometryMath::sphereProtrudesCube(Sphere* sphere,
                                       Cube*   cube) {

    Vector4 sphereCenter    = sphere->getPosition();
    float   sphereRadius    = sphere->getRadius();
    Vector4 cubeCenter      = cube->getCenter();
    float   cubeWidth       = cube->getWidth()  / 2.0f;
    float   cubeHeight      = cube->getHeight() / 2.0f;
    float   cubeLength      = cube->getLength() / 2.0f;
    Vector4 distanceCenters = sphereCenter - cubeCenter;
    float*  distance        = distanceCenters.getFlatBuffer();

    //Test x and z component first because most movement will be in the xz plane not falling down with gravity in the y direction
    //at least that is my theory to improve efficiency

    //Test x component
    if (distance[0] + sphereRadius > cubeWidth ||
        distance[0] - sphereRadius < -cubeWidth) {
        return true;
    }
    //Test z component
    if (distance[2] + sphereRadius > cubeLength ||
        distance[2] - sphereRadius < -cubeLength) {
        return true;
    }
    //Test y component
    if (distance[1] + sphereRadius > cubeHeight ||
        distance[1] - sphereRadius < -cubeHeight) {
        return true;
    }

    //Sphere is completely contained inside cube
    return false;
}

bool GeometryMath::spheresSpheresDetection(Geometry *spheresGeometryA,
                                           Geometry *spheresGeometryB) {

    // Get all of the spheres that model the geometry A
    auto sphereVecA = spheresGeometryA->getSpheres();
    //Get all of the triangles that model the geometry B
    auto sphereVecB = spheresGeometryB->getSpheres();

    for (auto sphereA : *sphereVecA) {

        for (auto sphereB : *sphereVecB) {

            if (sphereSphereDetection(sphereA, sphereB)) {
                return true;
            }
        }
    }
    return false;
}

bool GeometryMath::spheresTrianglesDetection(Geometry* spheresGeometry,
                                             Geometry* triangleGeometry) {

    //Get all of the spheres that model the geometry
    auto sphereVec   = spheresGeometry->getSpheres();
    //Get all of the triangles that model the geometry
    auto triangleVec = triangleGeometry->getTriangles();

    for (auto sphere : *sphereVec) {

        for (auto triangle : *triangleVec) {

            if (sphereTriangleDetection(sphere, triangle)) {
                return true;
            }
        }
    }
    return false;
}

Sphere GeometryMath::transform(Sphere* sphere, Matrix transform) {
    Vector4 center = transform * sphere->getObjectPosition();
    float* buffer  = transform.getFlatBuffer();
    Vector4 scale(buffer[0],
                  buffer[5],
                  buffer[10]);
    return Sphere(scale.getx() * sphere->getObjectRadius(),
                  center);
}

Triangle GeometryMath::transform(Triangle* triangle, Matrix transform) {
    Vector4x3 ABC = triangle->getTrianglePoints();
    Vector4     A = transform * ABC[0];
    Vector4     B = transform * ABC[1];
    Vector4     C = transform * ABC[2];

    return Triangle(A, B, C);
}

//3D Triangle plane test against a 3D Sphere
bool GeometryMath::sphereTriangleDetection(Sphere& sphere, Triangle& triangle) {


    /*A = A - P
    B = B - P
    C = C - P
    rr = r * r
    V = cross(B - A, C - A)
    d = dot(A, V)
    e = dot(V, V)
    sep1 = d * d > rr * e*/

    //Grab the location of the 3D sphere's center position
    Vector4 spherePosition  = sphere.getPosition();
    float sphereRadius      = sphere.getRadius();

    Vector4 *trianglePoints = triangle.getTrianglePoints();
    //Points A, B and C that describe a 3D Triangle
    Vector4 A(trianglePoints[0] - spherePosition);
    Vector4 B(trianglePoints[1] - spherePosition);
    Vector4 C(trianglePoints[2] - spherePosition);

    Vector4 temp(B - A);
    float rr  = sphereRadius * sphereRadius;
    Vector4 V = temp.crossProduct(C - A);
    float d   = A.dotProduct(V);
    float e   = V.dotProduct(V);
    if ((d*d) >= (rr*e)) {
        return false;
    }

    /*  aa = dot(A, A)
    ab = dot(A, B)
    ac = dot(A, C)
    bb = dot(B, B)
    bc = dot(B, C)
    cc = dot(C, C)
    sep2 = (aa > rr) & (ab > aa) & (ac > aa)
    sep3 = (bb > rr) & (ab > bb) & (bc > bb)
    sep4 = (cc > rr) & (ac > cc) & (bc > cc)*/

    float aa = A.dotProduct(A);
    float ab = A.dotProduct(B);
    float ac = A.dotProduct(C);
    float bb = B.dotProduct(B);
    float bc = B.dotProduct(C);
    float cc = C.dotProduct(C);

    if ((aa >= rr) & (ab >= aa) & (ac >= aa)) {
        return false;
    }

    if ((bb >= rr) & (ab >= bb) & (bc >= bb)) {
        return false;
    }

    if ((cc >= rr) & (ac >= cc) & (bc >= cc)) {
        return false;
    }

    /*AB = B - A
    BC = C - B
    CA = A - C
    d1 = ab - aa
    d2 = bc - bb
    d3 = ac - cc
    e1 = dot(AB, AB)
    e2 = dot(BC, BC)
    e3 = dot(CA, CA)
    Q1 = A * e1 - d1 * AB
    Q2 = B * e2 - d2 * BC
    Q3 = C * e3 - d3 * CA
    QC = C * e1 - Q1
    QA = A * e2 - Q2
    QB = B * e3 - Q3
    sep5 = [dot(Q1, Q1) > rr * e1 * e1] & [dot(Q1, QC) > 0]
    sep6 = [dot(Q2, Q2) > rr * e2 * e2] & [dot(Q2, QA) > 0]
    sep7 = [dot(Q3, Q3) > rr * e3 * e3] & [dot(Q3, QB) > 0]
    separated = sep1 | sep2 | sep3 | sep4 | sep5 | sep6 | sep7*/

    Vector4 AB(B - A);
    Vector4 BC(C - B);
    Vector4 CA(A - C);
    float d1 = ab - aa;
    float d2 = bc - bb;
    float d3 = ac - cc;
    float e1 = AB.dotProduct(AB);
    float e2 = BC.dotProduct(BC);
    float e3 = CA.dotProduct(CA);
    Vector4 Q1 = (A * e1) - (AB * d1);
    Vector4 Q2 = (B * e2) - (BC * d2);
    Vector4 Q3 = (C * e3) - (CA * d3);
    Vector4 QC = (C * e1) - Q1;
    Vector4 QA = (A * e2) - Q2;
    Vector4 QB = (B * e3) - Q3;

    if ((Q1.dotProduct(Q1) >= (rr * e1 * e1)) && (Q1.dotProduct(QC) >= 0.0f)) {
        return false;
    }
    if ((Q2.dotProduct(Q2) >= (rr * e2 * e2)) && (Q2.dotProduct(QA) >= 0.0f)) {
        return false;
    }
    if ((Q3.dotProduct(Q3) >= (rr * e3 * e3)) && (Q3.dotProduct(QB) >= 0.0f)) {
        return false;
    }

    //If passed all tests hit detected
    return true;
}

bool GeometryMath::sphereSphereDetection(Sphere& sphereA, Sphere& sphereB) {

    //Grab the location of the 3D sphere's center position A
    Vector4 spherePositionA = sphereA.getPosition();
    float   sphereRadiusA   = sphereA.getRadius();

    //Grab the location of the 3D sphere's center position B
    Vector4 spherePositionB = sphereB.getPosition();
    float   sphereRadiusB   = sphereB.getRadius();

    float distanceAB        = (spherePositionA - spherePositionB).getMagnitude();
    float radii             = sphereRadiusA + sphereRadiusB;

    //If the distance between sphere A and B is less than their radii combined then an overlap is detected
    if (radii - distanceAB >= 0) {
        return true;
    }
    return false;

}

//Test a single triangle against a single cube
bool GeometryMath::triangleCubeDetection(Triangle* triangle, Cube* cube) {

    float p0, p1, p2, r;
    // Compute box center and extents (if not already given in that format)
    Vector4 c = cube->getCenter();
    float e0  = cube->getHeight() / 2.0f;
    float e1  = cube->getWidth() / 2.0f;
    float e2  = cube->getLength() / 2.0f;

    // Translate triangle as conceptually moving AABB to origin
    Vector4* points = triangle->getTrianglePoints();
    Vector4 v0      = points[0] - c;
    Vector4 v1      = points[1] - c;
    Vector4 v2      = points[2] - c;

    // Compute edge vectors for triangle
    Vector4 f0      = v1 - v0;
    Vector4 f1      = v2 - v1;
    Vector4 f2      = v0 - v2;

    Vector4 u0(1, 0, 0, 1);
    Vector4 u1(0, 1, 0, 1);
    Vector4 u2(0, 0, 1, 1);

    Vector4 a00 = u0.crossProduct(f0);
    Vector4 a01 = u0.crossProduct(f1);
    Vector4 a02 = u0.crossProduct(f2);
    Vector4 a10 = u1.crossProduct(f0);
    Vector4 a11 = u1.crossProduct(f1);
    Vector4 a12 = u1.crossProduct(f2);
    Vector4 a20 = u2.crossProduct(f0);
    Vector4 a21 = u2.crossProduct(f1);
    Vector4 a22 = u2.crossProduct(f2);

    // Test axes a00..a22 (category 3)
    // Test axis a00
    p0 = v0.dotProduct(a00);
    p1 = v1.dotProduct(a00);
    p2 = v2.dotProduct(a00);
    r = e0 * abs(u0.dotProduct(a00)) + e1 * abs(u1.dotProduct(a00)) + e2 * abs(u2.dotProduct(a00));
    if ((_max(p2, _max(p0, p1)) < -r) || (_min(p2, _min(p0, p1)) > r)) {
        return false; // Axis is a separating axis
    }

    // Test axis a01
    p0 = v0.dotProduct(a01);
    p1 = v1.dotProduct(a01);
    p2 = v2.dotProduct(a01);
    r = e0 * abs(u0.dotProduct(a01)) + e1 * abs(u1.dotProduct(a01)) + e2 * abs(u2.dotProduct(a01));
    if ((_max(p2, _max(p0, p1)) < -r) || (_min(p2, _min(p0, p1)) > r)) {
        return false; // Axis is a separating axis
    }

    // Test axis a02
    p0 = v0.dotProduct(a02);
    p1 = v1.dotProduct(a02);
    p2 = v2.dotProduct(a02);
    r = e0 * abs(u0.dotProduct(a02)) + e1 * abs(u1.dotProduct(a02)) + e2 * abs(u2.dotProduct(a02));
    if ((_max(p2, _max(p0, p1)) < -r) || (_min(p2, _min(p0, p1)) > r)) {
        return false; // Axis is a separating axis
    }

    // Test axis a10
    p0 = v0.dotProduct(a10);
    p1 = v1.dotProduct(a10);
    p2 = v2.dotProduct(a10);
    r = e0 * abs(u0.dotProduct(a10)) + e1 * abs(u1.dotProduct(a10)) + e2 * abs(u2.dotProduct(a10));
    if ((_max(p2, _max(p0, p1)) < -r) || (_min(p2, _min(p0, p1)) > r)) {
        return false; // Axis is a separating axis
    }

    // Test axis a11
    p0 = v0.dotProduct(a11);
    p1 = v1.dotProduct(a11);
    p2 = v2.dotProduct(a11);
    r = e0 * abs(u0.dotProduct(a11)) + e1 * abs(u1.dotProduct(a11)) + e2 * abs(u2.dotProduct(a11));
    if ((_max(p2, _max(p0, p1)) < -r) || (_min(p2, _min(p0, p1)) > r)) {
        return false; // Axis is a separating axis
    }

    // Test axis a12
    p0 = v0.dotProduct(a12);
    p1 = v1.dotProduct(a12);
    p2 = v2.dotProduct(a12);
    r = e0 * abs(u0.dotProduct(a12)) + e1 * abs(u1.dotProduct(a12)) + e2 * abs(u2.dotProduct(a12));
    if ((_max(p2, _max(p0, p1)) < -r) || (_min(p2, _min(p0, p1)) > r)) {
        return false; // Axis is a separating axis
    }

    // Test axis a20
    p0 = v0.dotProduct(a20);
    p1 = v1.dotProduct(a20);
    p2 = v2.dotProduct(a20);
    r = e0 * abs(u0.dotProduct(a20)) + e1 * abs(u1.dotProduct(a20)) + e2 * abs(u2.dotProduct(a20));
    if ((_max(p2, _max(p0, p1)) < -r) || (_min(p2, _min(p0, p1)) > r)) {
        return false; // Axis is a separating axis
    }

    // Test axis a21
    p0 = v0.dotProduct(a21);
    p1 = v1.dotProduct(a21);
    p2 = v2.dotProduct(a21);
    r = e0 * abs(u0.dotProduct(a21)) + e1 * abs(u1.dotProduct(a21)) + e2 * abs(u2.dotProduct(a21));
    if ((_max(p2, _max(p0, p1)) < -r) || (_min(p2, _min(p0, p1)) > r)) {
        return false; // Axis is a separating axis
    }

    // Test axis a22
    p0 = v0.dotProduct(a22);
    p1 = v1.dotProduct(a22);
    p2 = v2.dotProduct(a22);
    r = e0 * abs(u0.dotProduct(a22)) + e1 * abs(u1.dotProduct(a22)) + e2 * abs(u2.dotProduct(a22));
    if ((_max(p2, _max(p0, p1)) < -r) || (_min(p2, _min(p0, p1)) > r)) {
        return false; // Axis is a separating axis
    }

    // Test the three axes corresponding to the face normals of AABB b (category 1).
    // Exit if...
    // ... [-e0, e0] and [min(v0.x,v1.x,v2.x), max(v0.x,v1.x,v2.x)] do not overlap
    if ((_max(v0.getx(), _max(v1.getx(), v2.getx())) < -e0) || (_min(v0.getx(), _min(v1.getx(), v2.getx())) > e0)) {
        return false;
    }
    // ... [-e1, e1] and [min(v0.y,v1.y,v2.y), max(v0.y,v1.y,v2.y)] do not overlap
    if ((_max(v0.gety(), _max(v1.gety(), v2.gety())) < -e1) || (_min(v0.gety(), _min(v1.gety(), v2.gety())) > e1)) {
        return false;
    }
    // ... [-e2, e2] and [min(v0.z,v1.z,v2.z), max(v0.z,v1.z,v2.z)] do not overlap
    if ((_max(v0.getz(), _max(v1.getz(), v2.getz())) < -e2) || (_min(v0.getz(), _min(v1.getz(), v2.getz())) > e2)) {
        return false;
    }

    // Test separating axis corresponding to triangle face normal (category 2)
    Vector4 normal = f0.crossProduct(f1);
    float d        = -normal.dotProduct(v0);  /* plane eq: normal.x+d=0 */
    Vector4 maxBox(e0, e1, e2, 1);

    float x_min, y_min, z_min;
    float x_max, y_max, z_max;

    if (normal.getx() > 0.0f) {
        x_min = -maxBox.getx();
        x_max = maxBox.getx();
    }
    else {
        x_min = maxBox.getx();
        x_max = -maxBox.getx();
    }

    if (normal.gety() > 0.0f) {
        y_min = -maxBox.gety();
        y_max = maxBox.gety();
    }
    else {
        y_min = maxBox.gety();
        y_max = -maxBox.gety();
    }

    if (normal.getz() > 0.0f) {
        z_min = -maxBox.getz();
        z_max = maxBox.getz();
    }
    else {
        z_min = maxBox.getz();
        z_max = -maxBox.getz();
    }

    Vector4 vmin(x_min, y_min, z_min, 1);
    Vector4 vmax(x_max, y_max, z_max, 1);

    if (normal.dotProduct(vmin) + d > 0.0f) {
        return false;
    }

    if (normal.dotProduct(vmax) + d >= 0.0f) {
        return true;
    }

    return false;
}

bool GeometryMath::sphereCubeDetection(Sphere *sphere, Cube *cube) {

    Vector4 point, pos, tempNorm;

    Vector4 rectanglePosition = cube->getCenter();
    Vector4 spherePosition    = sphere->getPosition();
    float   rectHeight        = cube->getHeight() / 2.0f;
    float   rectWidth         = cube->getWidth()  / 2.0f;
    float   rectLength        = cube->getLength() / 2.0f;
    float   sphereRadius      = sphere->getRadius();

    Vector4 normalWidth (1, 0, 0, 1);
    Vector4 normalHeight(0, 1, 0, 1);
    Vector4 normalLength(0, 0, 1, 1);

    Vector4 d = spherePosition - rectanglePosition;
    // Start result at center of box; make steps from there
    Vector4 q = rectanglePosition;
    // For each OBB axis...

    Vector4 normalColliderVect;

    // ...project d onto that axis to get the distance
    // along the axis of d from the box center
    float dist = d.dotProduct(normalWidth);

    // If distance farther than the box extents, clamp to the box
    if (dist > rectLength) {
        dist = rectLength;
    }
    if (dist < -rectLength) {
        dist = -rectLength;
    }

    // Step that distance along the axis to get world coordinate
    Vector4 step = normalWidth;
    step         = step * dist;
    q            = q + step;
    dist         = d.dotProduct(normalHeight);

    // If distance farther than the box extents, clamp to the box
    if (dist > rectHeight) {
        dist = rectHeight;
    }
    if (dist < -rectHeight) {
        dist = -rectHeight;
    }

    // Step that distance along the axis to get world coordinate
    step = normalHeight;
    step = step * dist;
    q    = q + step;
    dist = d.dotProduct(normalLength);

    // If distance farther than the box extents, clamp to the box
    if (dist > rectWidth) {
        dist = rectWidth;
    }
    if (dist < -rectWidth) {
        dist = -rectWidth;
    }


    // Step that distance along the axis to get world coordinate
    step = normalLength;
    step = step * dist;
    q    = q + step;

    Vector4 v = q - spherePosition;

    if (v.dotProduct(v) <= sphereRadius * sphereRadius) {
        return true;
    }
    return false;
}

void GeometryMath::sphereTriangleResolution(Entity*   modelA,
                                            Sphere&   sphere,
                                            Entity*   modelB,
                                            Triangle& triangle) {

    StateVector* modelStateA    = modelA->getStateVector();
    Vector4*     triPoints      = triangle.getTrianglePoints();
    Vector4      spherePosition = sphere.getPosition();
    float        sphereRadius   = sphere.getRadius();

    //Compute the normal of the triangle
    Vector4 normal = triPoints[2] - triPoints[0];
    normal         = normal.crossProduct(triPoints[1] - triPoints[0]);
    normal.normalize();

    Vector4 closestPointOnTriangle = _closestPoint(&sphere,
                                                   &triangle);
    Vector4 overlap                = spherePosition - closestPointOnTriangle;
    overlap.normalize();

    Vector4 delta = ((overlap)*(sphereRadius*1.0001f)) + closestPointOnTriangle - spherePosition;

    //Sliding velocity component
    //Compute the speed of the resultant velocity along the normal for sliding collision resolution
    float normalComponent     = modelStateA->getLinearVelocity().dotProduct(normal);
    //Resultant velocity vector
    Vector4 n                 = normal * normalComponent;
    //Subtract original velocity vectory with the new velocity vector along the normal
    Vector4 resultantVelocity = modelStateA->getLinearVelocity() - n;

    modelA->setPosition(modelStateA->getLinearPosition() + delta);
    modelStateA->setLinearVelocity(resultantVelocity);
    modelStateA->setContact(true);
}
void GeometryMath::sphereSphereResolution(Entity* modelA,
                                          Sphere& sphereA,
                                          Entity* modelB,
                                          Sphere& sphereB) {

    StateVector* modelStateA = modelA->getStateVector();
    StateVector* modelStateB = modelB->getStateVector();

    Vector4 collisionNormal  = sphereA.getPosition() - sphereB.getPosition();
    
    if (modelStateA->getActive()) {
        //0.01 is restitution
        float overlap = ((sphereA.getRadius() + sphereB.getRadius() - collisionNormal.getMagnitude()) / 2.0f) + 0.01f;
        collisionNormal.normalize();
        modelA->setVelocity(collisionNormal * modelStateA->getLinearVelocity().getMagnitude() * 0.1f);

        auto prevModelBuffer = modelA->getPrevMVP()->getModelBuffer();
        modelA->setPosition(Vector4(prevModelBuffer[3], prevModelBuffer[7], prevModelBuffer[11]));
    }

    if (modelStateB->getActive()) {
        //Opposite reaction
        collisionNormal = -collisionNormal;
        //0.01 is restitution
        float overlap   = ((sphereA.getRadius() + sphereB.getRadius() - collisionNormal.getMagnitude()) / 2.0f) + 0.01f;
        collisionNormal.normalize();
        modelB->setVelocity(collisionNormal * modelStateB->getLinearVelocity().getMagnitude() * 0.1f);

        auto prevModelBuffer = modelB->getPrevMVP()->getModelBuffer();
        modelB->setPosition(Vector4(prevModelBuffer[3], prevModelBuffer[7], prevModelBuffer[11]));
    }
}

float GeometryMath::_max(float a,
                         float b) {
    return (a > b ? a : b);
}

float GeometryMath::_min(float a,
                         float b) {
    return (a < b ? a : b);
}

Vector4 GeometryMath::_closestPoint(Sphere* sphere,
                                    Triangle* triangle)
{
    /** The code for Triangle-float3 test is from Christer Ericson's Real-Time Collision Detection, pp. 141-142. */

    // Check if P is in vertex region outside A.
    Vector4* triPoints = triangle->getTrianglePoints();
    Vector4 A          = triPoints[0];
    Vector4 B          = triPoints[1];
    Vector4 C          = triPoints[2];
    Vector4 P          = sphere->getPosition();

    Vector4 ab = B - A;
    Vector4 ac = C - A;
    Vector4 ap = P - A;
    float d1   = ab.dotProduct(ap);
    float d2   = ac.dotProduct(ap);
    if (d1 <= 0.f && d2 <= 0.f)
        return A; // Barycentric coordinates are (1,0,0).

    // Check if P is in vertex region outside B.
    Vector4 bp = P - B;
    float d3   = ab.dotProduct(bp);
    float d4   = ac.dotProduct(bp);
    if (d3 >= 0.f && d4 <= d3)
        return B; // Barycentric coordinates are (0,1,0).

    // Check if P is in edge region of AB, and if so, return the projection of P onto AB.
    float vc    = d1 * d4 - d3 * d2;
    if (vc <= 0.f && d1 >= 0.f && d3 <= 0.f)
    {
        float v = d1 / (d1 - d3);
        return A + (ab * v); // The barycentric coordinates are (1-v, v, 0).
    }
    // Check if P is in vertex region outside C.
    Vector4 cp = P - C;
    float d5   = ab.dotProduct(cp);
    float d6   = ac.dotProduct(cp);
    if (d6 >= 0.f && d5 <= d6)
        return C; // The barycentric coordinates are (0,0,1).

    // Check if P is in edge region of AC, and if so, return the projection of P onto AC.
    float vb    = d5 * d2 - d1 * d6;
    if (vb <= 0.f && d2 >= 0.f && d6 <= 0.f)
    {
        float w = d2 / (d2 - d6);
        return A + (ac * w); // The barycentric coordinates are (1-w, 0, w).
    }

    // Check if P is in edge region of BC, and if so, return the projection of P onto BC.
    float va    = d3 * d6 - d5 * d4;
    if (va <= 0.f && (d4 - d3) >= 0.f && (d5 - d6) >= 0.f)
    {
        float w = (d4 - d3) / (d4 - d3 + d5 - d6);
        return B + ((C - B) * w); // The barycentric coordinates are (0, 1-w, w).
    }

    // P must be inside the face region. Compute the closest point through its barycentric coordinates (u,v,w).
    float denom = 1.f / (va + vb + vc);
    float v     = vb * denom;
    float w     = vc * denom;
    return A + ab * v + ac * w;
}

void GeometryMath::getFrustumPlanes(Matrix inverseViewProjection, std::vector<Vector4>& frustumPlanes) {
    
    std::vector<Vector4> frustumPoints;

    frustumPoints.push_back(Vector4(-1.0, -1.0,  1.0)); //left  bottom far
    frustumPoints.push_back(Vector4(-1.0,  1.0,  1.0)); //left  top    far
    frustumPoints.push_back(Vector4(-1.0, -1.0, -1.0)); //left  bottom near 
    frustumPoints.push_back(Vector4(-1.0,  1.0, -1.0)); //left  top    near
    frustumPoints.push_back(Vector4( 1.0, -1.0,  1.0)); //right bottom far
    frustumPoints.push_back(Vector4( 1.0,  1.0,  1.0)); //right top    far
    frustumPoints.push_back(Vector4( 1.0, -1.0, -1.0)); //right bottom near
    frustumPoints.push_back(Vector4( 1.0,  1.0, -1.0)); //right top    near

    for (auto& point : frustumPoints) {
        point = inverseViewProjection * point;
        point = point / point.getw();
    }

    //left plane
    {
        Vector4 A(frustumPoints[2] - frustumPoints[0]);
        Vector4 B(frustumPoints[1] - frustumPoints[0]);
        Vector4 C(B.crossProduct(A));
        float d = -(C.getx() * -frustumPoints[0].getx() +
                    C.gety() * -frustumPoints[0].gety() +
                    C.getz() * -frustumPoints[0].getz());
        Vector4 D(C.getx(), C.gety(), C.getz(), d);
        D.normalize();
        frustumPlanes.push_back(D);
    }

    //right plane
    {
        Vector4 A(frustumPoints[5] - frustumPoints[4]);
        Vector4 B(frustumPoints[6] - frustumPoints[4]);
        Vector4 C(B.crossProduct(A));
        float d = -(C.getx() * -frustumPoints[4].getx() +
                    C.gety() * -frustumPoints[4].gety() +
                    C.getz() * -frustumPoints[4].getz());
        Vector4 D(C.getx(), C.gety(), C.getz(), d);
        D.normalize();
        frustumPlanes.push_back(D);
    }

    //far plane
    {
        Vector4 A(frustumPoints[3] - frustumPoints[6]);
        Vector4 B(frustumPoints[2] - frustumPoints[6]);
        Vector4 C(B.crossProduct(A));
        float d = -(C.getx() * -frustumPoints[6].getx() +
                    C.gety() * -frustumPoints[6].gety() +
                    C.getz() * -frustumPoints[6].getz());
        Vector4 D(C.getx(), C.gety(), C.getz(), d);
        D.normalize();
        frustumPlanes.push_back(D);
    }

    //near plane
    {
        Vector4 A(frustumPoints[0] - frustumPoints[4]);
        Vector4 B(frustumPoints[1] - frustumPoints[4]);
        Vector4 C(B.crossProduct(A));
        float d = -(C.getx() * -frustumPoints[4].getx() +
                    C.gety() * -frustumPoints[4].gety() +
                    C.getz() * -frustumPoints[4].getz());
        Vector4 D(C.getx(), C.gety(), C.getz(), d);
        D.normalize();
        frustumPlanes.push_back(D);
    }

    //bottom plane
    {
        Vector4 A(frustumPoints[4] - frustumPoints[0]);
        Vector4 B(frustumPoints[2] - frustumPoints[0]);
        Vector4 C(B.crossProduct(A));
        float d = -(C.getx() * -frustumPoints[0].getx() +
                    C.gety() * -frustumPoints[0].gety() +
                    C.getz() * -frustumPoints[0].getz());
        Vector4 D(C.getx(), C.gety(), C.getz(), d);
        D.normalize();
        frustumPlanes.push_back(D);
    }

    //top plane
    {
        Vector4 A(frustumPoints[1] - frustumPoints[5]);
        Vector4 B(frustumPoints[3] - frustumPoints[5]);
        Vector4 C(B.crossProduct(A));
        float d = -(C.getx() * -frustumPoints[5].getx() +
                    C.gety() * -frustumPoints[5].gety() +
                    C.getz() * -frustumPoints[5].getz());
        Vector4 D(C.getx(), C.gety(), C.getz(), d);
        D.normalize();
        frustumPlanes.push_back(D);
    }
    
}

bool GeometryMath::frustumAABBDetection(std::vector<Vector4> planes, Vector4 &mins, Vector4 &maxs) {
    
    std::vector<Vector4> aabbPoints;
    aabbPoints.push_back(Vector4(mins.getx(), mins.gety(), mins.getz()));
    aabbPoints.push_back(Vector4(mins.getx(), mins.gety(), maxs.getz()));
    aabbPoints.push_back(Vector4(mins.getx(), maxs.gety(), maxs.getz()));
    aabbPoints.push_back(Vector4(maxs.getx(), maxs.gety(), maxs.getz()));
    aabbPoints.push_back(Vector4(maxs.getx(), maxs.gety(), mins.getz()));
    aabbPoints.push_back(Vector4(maxs.getx(), mins.gety(), mins.getz()));
    aabbPoints.push_back(Vector4(maxs.getx(), mins.gety(), maxs.getz()));
    aabbPoints.push_back(Vector4(mins.getx(), maxs.gety(), mins.getz()));

    bool aabbOutside = false;
    for (auto plane : planes) {
        int pointsOutside = 0;
        for (auto point : aabbPoints) {
            float distance = plane.getw();
            auto pointOnPlane = plane * distance;
            auto pointVector = point - pointOnPlane;
            if (plane.dotProduct(pointVector) > 0) {
                pointsOutside++;
            }
        }
        if (pointsOutside == 8) {
            aabbOutside = true;
        }
    }

    return !aabbOutside;
}
