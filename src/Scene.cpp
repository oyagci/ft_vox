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
    shader.setUniform4x4f("modelMatrix", glm::translate(glm::vec3(0, -2, 0)) * glm::scale(glm::vec3(50, 1, 50)));
    _basicCube.draw();

    for (int x = 0; x < 10; x++)
    {
        for (int z = 0; z < 10; z++)
        {
            shader.setUniform4x4f("modelMatrix", glm::translate(glm::vec3(x * 10, 0, z * 10)) * glm::scale(glm::vec3(1, 1, 1)));
            _basicCube.draw();
        }
    }
}
