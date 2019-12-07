#include "./Scene.hpp"

Scene::Scene()
{

}

Scene::~Scene()
{

}

//Temporary
float rotation = 0;

void Scene::update()
{
    rotation++;
}

void Scene::render(Shader &shader)
{
    shader.setUniform4x4f("modelMatrix", glm::rotate(rotation * 0.01f, glm::vec3(1, 1, 0)));
    _basicCube.draw();
}