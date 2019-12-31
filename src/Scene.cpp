#include "./Scene.hpp"

Scene::Scene()
{

}

Scene::~Scene()
{

}

void Scene::update()
{

}

void Scene::render(Shader &shader)
{
    shader.setUniform4x4f("modelMatrix", glm::scale(glm::vec3(10, 1, 10)) * glm::translate(glm::vec3(0, -2, 0)));
    _basicCube.draw();

    shader.setUniform4x4f("modelMatrix", glm::scale(glm::vec3(1, 1, 1)));
    _basicCube.draw();
}
