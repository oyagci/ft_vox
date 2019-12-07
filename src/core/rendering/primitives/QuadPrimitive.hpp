#pragma once

#include <lazy.hpp>

using namespace lazy;
using namespace graphics;

class QuadPrimitive
{
private:
    Mesh _mesh;

public:
    QuadPrimitive();
    ~QuadPrimitive() {};

    void draw();

    Mesh getMesh() const { return _mesh; }
};