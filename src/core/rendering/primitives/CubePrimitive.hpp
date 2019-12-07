#pragma once

#include <lazy.hpp>

using namespace lazy;
using namespace graphics;

class CubePrimitive
{
private:
    Mesh _mesh;

public:
    CubePrimitive();
    ~CubePrimitive() {};

    void draw();

    Mesh getMesh() const { return _mesh; }
};