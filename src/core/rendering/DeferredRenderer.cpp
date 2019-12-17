#include "./DeferredRenderer.hpp"

DeferredRenderer::DeferredRenderer()
{
    _gbufferShader.addVertexShader("shaders/gbuffer.vs.glsl")
			.addFragmentShader("shaders/gbuffer.fs.glsl");
	_gbufferShader.link();
    _quadShader.addVertexShader("shaders/deferred.vs.glsl")
			.addFragmentShader("shaders/deferred.fs.glsl");
	_quadShader.link();

    _gbuffer.genFramebuffer();
}

DeferredRenderer::~DeferredRenderer()
{

}

void DeferredRenderer::resize(int width, int height)
{
    _width = width;
    _height = height;

    _gbuffer.setSize(width, height);
    _gbuffer.genColorTexture(GL_RGB16F, GL_RGB, GL_FLOAT, GL_NEAREST, GL_CLAMP_TO_BORDER);
    _gbuffer.genColorTexture(GL_RGB16F, GL_RGB, GL_FLOAT, GL_NEAREST, GL_CLAMP_TO_BORDER);
    _gbuffer.genColorTexture(GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, GL_LINEAR, GL_CLAMP_TO_BORDER);
    _gbuffer.genRenderbuffer(GL_DEPTH, GL_DEPTH_COMPONENT);
    _gbuffer.drawBuffers({
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT2,
    });
}

void DeferredRenderer::update()
{

}

void DeferredRenderer::renderScene(Camera &camera, Scene &scene)
{
    _gbuffer.bind();
    glViewport(0, 0, _width, _height);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    _gbufferShader.bind();
    _gbufferShader.setUniform4x4f("projectionMatrix", camera.getProjectionMatrix());
    _gbufferShader.setUniform4x4f("viewMatrix", camera.getViewMatrix());
    _gbufferShader.setUniform4x4f("viewProjectionMatrix", camera.getViewProjectionMatrix());
    scene.render(_gbufferShader);

    _gbuffer.unbind();
}

void DeferredRenderer::renderDebug()
{
    _quadShader.bind(); 
    this->bind(_quadShader);
    _quad.draw();
}

void DeferredRenderer::render(const Light &light)
{
    _quadShader.bind(); 
    this->bind(_quadShader);
    _quad.draw();
    this->unbind();
}

void DeferredRenderer::bind(Shader &shader)
{
    shader.setUniform1i("position", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _gbuffer.getColorTexture(0));
    shader.setUniform1i("normal", 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, _gbuffer.getColorTexture(1));
    shader.setUniform1i("albedo", 2);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, _gbuffer.getColorTexture(2));
}

void DeferredRenderer::unbind()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
