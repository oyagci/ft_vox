#include "./RenderingPipeline.hpp"

RenderingPipeline::RenderingPipeline()
{
    _quadShader.addVertexShader("shaders/screen_quad.vs.glsl")
			    .addFragmentShader("shaders/screen_quad.fs.glsl");
	_quadShader.link();

    // _fbo.genFramebuffer();
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

void RenderingPipeline::renderDeferred()
{
     glBindTexture(GL_TEXTURE_2D, _deferred.getGBuffer().getColorTexture(0));
    _quadShader.bind();
    _screenQuad.draw();
}

void RenderingPipeline::renderScreenQuad()
{
    glBindTexture(GL_TEXTURE_2D, _deferred.getGBuffer().getColorTexture(0));
    _quadShader.bind();
    _screenQuad.draw();
}

void RenderingPipeline::render()
{
    _deferred.renderDebug();
    // renderScreenQuad();
}
