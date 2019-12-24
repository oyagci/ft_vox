#include "./RenderingPipeline.hpp"

RenderingPipeline::RenderingPipeline()
:   _resized(false)
{
    _quadShader.addVertexShader("shaders/screen_quad.vs.glsl")
			    .addFragmentShader("shaders/screen_quad.fs.glsl");
	_quadShader.link();

    _fbo.genFramebuffer();
}

RenderingPipeline::~RenderingPipeline()
{
}

void RenderingPipeline::resize(int width, int height)
{
    _width = width;
    _height = height;
    _deferred.resize(width, height);
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

void RenderingPipeline::renderScene(Camera &camera, Scene &scene)
{
    _deferred.renderScene(camera, scene);
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
    {
        renderDeferred(*light, camera);
    }

    glDepthFunc(GL_LESS);
    glDepthMask(true);
    glDisable(GL_BLEND);
}

void RenderingPipeline::renderScreenQuad()
{
    glBindTexture(GL_TEXTURE_2D, _fbo.getColorTexture(0));
    _quadShader.bind();
    _screenQuad.draw();
}

void RenderingPipeline::render(std::vector<Light*> &lights, Camera &camera)
{
    bind();
    renderForward(lights, camera);
    unbind();
    renderScreenQuad();
}
