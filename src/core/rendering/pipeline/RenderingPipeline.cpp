#include "./RenderingPipeline.hpp"

RenderingPipeline::RenderingPipeline()
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
    _fbo.setSize(width, height);
    _fbo.genColorTexture(GL_LINEAR, GL_REPEAT);
    // _fbo.genDepthTexture(GL_NEAREST, GL_REPEAT);
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

void RenderingPipeline::render()
{
    glBindTexture(GL_TEXTURE_2D, _fbo.getColorTexture());
    _quadShader.bind();
    _screenQuad.draw();
}
