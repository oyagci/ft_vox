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
    shader.setUniform4x4f("modelMatrix", glm::scale(glm::vec3(10, 1, 10)));
    _basicCube.draw();
}