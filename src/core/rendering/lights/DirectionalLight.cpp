#include "DirectionalLight.hpp"
#include <limits>

DirectionalLight::DirectionalLight()
:   DirectionalLight(glm::vec4(1, 1, 1, 1), 1, glm::quat(glm::vec3(0, 0, 0)))
{}

DirectionalLight::DirectionalLight(const glm::vec4 &color, float intensity, const glm::quat &rotation)
:   Light("directionallight", color, intensity), 
    _rotation(rotation),
    _direction(glm::normalize(glm::vec3(glm::vec4(0.0f, 0.0f, -1.0f, 0.0f) * glm::toMat4(rotation))))
{}

DirectionalLight::~DirectionalLight()
{}

void DirectionalLight::initShadows(int resolution)
{
    _resolution = resolution;
    _shadowFbo.clean();
    _shadowFbo.genFramebuffer();
    _shadowFbo.setSize(resolution, resolution);
    _shadowFbo.genDepthTexture(GL_NEAREST, GL_CLAMP_TO_EDGE);
    _shadowFbo.genRenderbuffer(GL_DEPTH, GL_DEPTH_COMPONENT);

    _shadowShader.addVertexShader("shaders/lights/shadows/shadow.vs.glsl")
			     .addFragmentShader("shaders/lights/shadows/shadow.fs.glsl");
	_shadowShader.link();
    _emmitShadows = true;
}

void DirectionalLight::bind(Camera &camera)
{
    _shader.bind();
    _shader.setUniform4f("light.light.color", _color);
    _shader.setUniform1f("light.light.intensity", _intensity);
    _shader.setUniform3f("light.direction", _direction);

    if (_emmitShadows)
    {
        _shader.setUniform1i("light.light.hasShadow", 1);
        _shader.setUniform4x4f("light.light.shadowProjection", _shadowProjection);
        _shader.setUniform1i("light.light.shadowMap", 10);
        glActiveTexture(GL_TEXTURE10);
        glBindTexture(GL_TEXTURE_2D, _shadowFbo.getDepthTexture());
    }
    else
    {
        _shader.setUniform1i("light.light.hasShadow", 0);
    }
}

glm::mat4 DirectionalLight::getViewMatrix(float width, float height, Camera &camera)
{
    // std::cout << _direction.x << " " << _direction.y << " " << _direction.z << " " << "\n";
    // glm::mat4 lightProjectionMatrix = glm::lookAt(_direction, glm::vec3(0, 0, 0), glm::vec3(0, -1, 0));
    // glm::mat4 lightProjectionMatrix = glm::lookAt(glm::vec3(0, -1, 0), glm::vec3(0, 0, 0), glm::vec3(0, -1, 0));
    glm::mat4 cameraTranslation = glm::translate(glm::mat4(), -camera.getPosition());
    glm::mat4 cameraRotation = glm::lookAt(camera.getTransform().forward(), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    glm::mat4 cameraViewMatrix = cameraRotation;
    // glm::mat4 cameraViewMatrix = camera.getViewMatrix();

    glm::mat4 cameraViewMatrixInverted = glm::inverse(cameraViewMatrix);
    glm::mat4 lightProjectionMatrix = glm::toMat4(_rotation);
    
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

    glm::vec4 frustumCorners[] = {
        glm::vec4(xn, yn, shadowNear, 1.0f),
        glm::vec4(-xn, yn, shadowNear, 1.0f),
        glm::vec4(xn, -yn, shadowNear, 1.0f),
        glm::vec4(-xn, -yn, shadowNear, 1.0f),
        
        glm::vec4(xf, yf, shadowFar, 1.0f),
        glm::vec4(-xf, yf, shadowFar, 1.0f),
        glm::vec4(xf, -yf, shadowFar, 1.0f),
        glm::vec4(-xf, -yf, shadowFar, 1.0f)
    };

    float minX = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::lowest();
    float minY = std::numeric_limits<float>::max();
    float maxY = std::numeric_limits<float>::lowest();
    // float minZ = std::numeric_limits<float>::max();
    // float maxZ = std::numeric_limits<float>::lowest();

    glm::vec4 frustumCornersL[8];

    for (int j = 0; j < 8; j++) {
        glm::vec4 vW = cameraViewMatrixInverted * frustumCorners[j];

        frustumCornersL[j] = lightProjectionMatrix * vW;

        minX = MIN(minX, frustumCornersL[j].x);
        maxX = MAX(maxX, frustumCornersL[j].x);
        minY = MIN(minY, frustumCornersL[j].y);
        maxY = MAX(maxY, frustumCornersL[j].y);
        // minZ = MIN(minZ, frustumCornersL[j].z);
        // maxZ = MAX(maxZ, frustumCornersL[j].z);
    }

    float border = 2;
    glm::mat4 projection = glm::ortho(
        maxX + border,
        minX - border,
        minY - border,
        maxY + border,
        // minZ - border,
        // maxZ + border
        -shadowFar,
        shadowFar
    );

    // glm::mat4 result = projectionMatrix * lightProjectionMatrix;
    _shadowProjection = projection * lightProjectionMatrix * glm::translate(-camera.getPosition());
    return _shadowProjection;
}