#include "./DeferredRenderer.hpp"

DeferredRenderer::DeferredRenderer()
{
    _gbufferShader.addVertexShader("shaders/gbuffer.vs.glsl")
			.addFragmentShader("shaders/gbuffer.fs.glsl");
	_gbufferShader.link();
    _quadShader.addVertexShader("shaders/deferred.vs.glsl")
			.addFragmentShader("shaders/deferred.fs.glsl");
	_quadShader.link();

    _ssaoShader.addVertexShader("shaders/filters/filter.vs.glsl")
			.addFragmentShader("shaders/filters/ssao.glsl");
	_ssaoShader.link();
}

DeferredRenderer::~DeferredRenderer()
{

}

void DeferredRenderer::genSSAOKernel(int size)
{
    _ssaoKernel.clear();

    for (uint i = 0; i < size; ++i)
    {
        float random1 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        float random2 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        float random3 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        float random4 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

        glm::vec3 sample(random1 * 2.0 - 1.0, random2 * 2.0 - 1.0, random3 * 2.0 - 1.0);

        sample = glm::normalize(sample);
        sample *= random4;
        float scale = (float)i / float(size); 
        scale = 0.1f + (scale * scale) * (1.0f - 0.1f);
        sample *= scale;

        _ssaoKernel.push_back(sample.x);
        _ssaoKernel.push_back(sample.y);
        _ssaoKernel.push_back(sample.z);
    }
}

void DeferredRenderer::genSSAONoise(const int size)
{
    glm::vec3 ssaoNoise[size * size];

    for (unsigned int i = 0; i < size * size; i++)
    {
        float random1 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        float random2 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        float random3 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

        glm::vec3 noise(random1 * 2.0 - 1.0, random2 * 2.0 - 1.0, 0.0);

        ssaoNoise[i] = noise;
    }

    glGenTextures(1, &_ssaoNoiseTexture);
    glBindTexture(GL_TEXTURE_2D, _ssaoNoiseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, size, size, 0, GL_RGB, GL_FLOAT, ssaoNoise);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); 
}

void DeferredRenderer::genSSAOPass(int width, int height)
{
    _ssaoPass.clean();
    _ssaoPass.genFramebuffer();
    _ssaoPass.setSize(width, height);
    _ssaoPass.genColorTexture(GL_RGB, GL_RGBA, GL_FLOAT, GL_NEAREST, GL_CLAMP_TO_BORDER);
    _ssaoPass.genRenderbuffer(GL_DEPTH, GL_DEPTH_COMPONENT);
    _ssaoPass.drawBuffers({
        GL_COLOR_ATTACHMENT0
    });

    this->genSSAOKernel(32);
    this->genSSAONoise(4);
}

void DeferredRenderer::bindSSAO(Camera &camera)
{
    _ssaoPass.bind();
    glViewport(0, 0, _width, _height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    _ssaoShader.bind();
    this->bind(_ssaoShader);
    _ssaoShader.setUniform4x4f("modelViewMatrix", glm::mat4(1));

    _ssaoShader.setUniform1f("screen_w", _width);
    _ssaoShader.setUniform1f("screen_h", _height);

    _ssaoShader.setUniform3f("cameraPosition", camera.getPosition());
    _ssaoShader.setUniform3f("cameraDirection", camera.getTransform().forward());

    _ssaoShader.setUniform1i("kernelSize", 32);
    
    _ssaoShader.setUniform1i("ssaoNoise", 4);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, _ssaoNoiseTexture);

    glUniform3fv(_ssaoShader.getUniformLocation("ssaoKernel"), 32, &_ssaoKernel[0]);

    _ssaoShader.setUniform4x4f("viewMatrix", camera.getViewMatrix());
    _ssaoShader.setUniform4x4f("projectionMatrix", camera.getProjectionMatrix());
    _ssaoShader.setUniform4x4f("inverseProjectionMatrix", glm::inverse(camera.getProjectionMatrix()));

    _quad.draw();

    _ssaoShader.unbind();
    _ssaoPass.unbind();
}

void DeferredRenderer::resize(int width, int height)
{
    _width = width;
    _height = height;

    _gbuffer.clean();
    _gbuffer.genFramebuffer();
    _gbuffer.setSize(width, height);
    _gbuffer.genColorTexture(GL_RGBA16F, GL_RGBA, GL_FLOAT, GL_NEAREST, GL_CLAMP_TO_BORDER);
    _gbuffer.genColorTexture(GL_RGB16F, GL_RGB, GL_FLOAT, GL_NEAREST, GL_CLAMP_TO_BORDER);
    _gbuffer.genColorTexture(GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, GL_LINEAR, GL_CLAMP_TO_BORDER);
    _gbuffer.genDepthTexture(GL_NEAREST, GL_MIRRORED_REPEAT);
    _gbuffer.genRenderbuffer(GL_DEPTH, GL_DEPTH_COMPONENT);
    _gbuffer.drawBuffers({
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT2,
    });

    genSSAOPass(width, height);
}

void DeferredRenderer::update(Camera &camera)
{
    float aspect = 720.0f / 1280.0f;
    float fov = camera.getFov();

    float tanHalfHorFOV = std::tan(TO_RADIANS(fov / 2.0f));
    float tanHalfVerFOV = std::tan(TO_RADIANS((fov * aspect) / 2.0f));

    float shadowNear = camera.getNear();
    float shadowFar = 50.0f;

    float xn = shadowNear * tanHalfHorFOV;
    float xf = shadowFar * tanHalfHorFOV;
    float yn = shadowNear * tanHalfVerFOV;
    float yf = shadowFar * tanHalfVerFOV;

    _frustumCorners = {
        glm::vec4(xn, yn, shadowNear, 1.0f),
        glm::vec4(-xn, yn, shadowNear, 1.0f),
        glm::vec4(xn, -yn, shadowNear, 1.0f),
        glm::vec4(-xn, -yn, shadowNear, 1.0f),
        
        glm::vec4(xf, yf, shadowFar, 1.0f),
        glm::vec4(-xf, yf, shadowFar, 1.0f),
        glm::vec4(xf, -yf, shadowFar, 1.0f),
        glm::vec4(-xf, -yf, shadowFar, 1.0f)
    };
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
    _quadShader.unbind(); 
}

void DeferredRenderer::render(Light &light, Camera &camera)
{
    light.bind(camera);
    this->bind(light.getShader());
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
    shader.setUniform1i("depth", 3);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, _gbuffer.getDepthTexture());
}

void DeferredRenderer::unbind()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
