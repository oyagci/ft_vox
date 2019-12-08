#include "./DeferredRenderer.hpp"

DeferredRenderer::DeferredRenderer()
{
    _shader.addVertexShader("shaders/deferred.vs.glsl")
			.addFragmentShader("shaders/deferred.fs.glsl");
	_shader.link();
}

DeferredRenderer::~DeferredRenderer()
{

}

void DeferredRenderer::resize(int width, int height)
{
    _width = width;
    _height = height;
    _gbuffer.setSize(width, height);

    _gbuffer.genColorTexture(GL_RGB16F, GL_RGB, GL_FLOAT, GL_NEAREST, Geom::POSITION);
    _gbuffer.genColorTexture(GL_RGB16F, GL_RGB, GL_FLOAT, GL_NEAREST, Geom::NORMAL);
    _gbuffer.genColorTexture(GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, GL_NEAREST, Geom::ALBEDO);

    _gbuffer.drawBuffers({
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT2,
    });
}

void DeferredRenderer::update()
{

}

void DeferredRenderer::bind()
{
    _gbuffer.bind();
    glViewport(0, 0, _width, _height);
}

void DeferredRenderer::unbind()
{
    _gbuffer.unbind();
}