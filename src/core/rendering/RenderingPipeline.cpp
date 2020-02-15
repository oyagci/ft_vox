#include "./RenderingPipeline.hpp"

RenderingPipeline::RenderingPipeline()
:   _resized(false)
{
    _quadShader.addVertexShader("shaders/quad.vs.glsl")
			    .addFragmentShader("shaders/quad.fs.glsl");
	_quadShader.link();
}

RenderingPipeline::~RenderingPipeline()
{}

void RenderingPipeline::resize(int width, int height)
{
    _width = width;
    _height = height;
    _deferred.resize(width, height);
    _fbo.clean();
    _fbo.genFramebuffer();
    _fbo.setSize(width, height);
    _fbo.genColorTexture(GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, GL_LINEAR, GL_REPEAT);
    _fbo.genRenderbuffer(GL_DEPTH, GL_DEPTH_COMPONENT);
    _resized = true;
}

void RenderingPipeline::bind()
{
    _fbo.bind();
    glViewport(0, 0, _width, _height);
}

void RenderingPipeline::unbind()
{
    _fbo.unbind();
}

void RenderingPipeline::update(Camera &camera)
{
    _deferred.update(camera);
}

void RenderingPipeline::renderScene(Camera &camera, Scene &scene)
{
    _deferred.renderScene(camera, scene);
}

void RenderingPipeline::renderShadows(Light &light, Camera &camera, Scene &scene)
{
    light.getShadowFBO().bind();
    glViewport(0, 0, light.getResolution(), light.getResolution());

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    light.getShadowShader().bind();
    // light.getShadowShader().setUniform4x4f("projectionMatrix", camera.getProjectionMatrix());
    // light.getShadowShader().setUniform4x4f("viewMatrix", camera.getViewMatrix());
    // light.getShadowShader().setUniform4x4f("viewProjectionMatrix", camera.getViewProjectionMatrix());
    light.getShadowShader().setUniform4x4f("viewProjectionMatrix", light.getViewMatrix(_width, _height, camera));
    scene.render(light.getShadowShader());

    light.getShadowFBO().unbind();
}

void RenderingPipeline::renderDeferred(Light &light, Camera &camera)
{
    _deferred.render(light, camera);
}

void RenderingPipeline::renderForward(std::vector<Light*> &lights, Camera &camera)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glDepthMask(false);
    glDepthFunc(GL_EQUAL);

    for (Light *light : lights)
        renderDeferred(*light, camera);

    glDepthFunc(GL_LESS);
    glDepthMask(true);
    glDisable(GL_BLEND);
}

void RenderingPipeline::renderScreenQuad()
{
    _quadShader.bind();
    glBindTexture(GL_TEXTURE_2D, _fbo.getColorTexture(0));
    _quadShader.setUniform4x4f("modelViewMatrix", glm::mat4(1));
    _quad.draw();
    _quadShader.unbind();
}

void RenderingPipeline::render(std::vector<Light*> &lights, Camera &camera)
{
    bind();
    renderForward(lights, camera);
    unbind();
    renderScreenQuad();

    // _quadShader.bind();
    // glBindTexture(GL_TEXTURE_2D, _deferred.getGBuffer().getColorTexture(0)); //(*lights[0]).getShadowFBO().getDepthTexture());
    // _quadShader.setUniform4x4f("modelViewMatrix", glm::translate(glm::vec3(-1 + 0.3f, -1 + 0.6f, -1)) * glm::scale(glm::vec3(0.3f, 0.6, 1)));
    // _quad.draw();
    // _quadShader.unbind();
}
