in vec3 viewPosition;
in vec3 viewDirection;

struct Light {
	vec4		color;
	float		intensity;
	
	int			hasShadow;
	sampler2D	shadowMap;
	mat4		shadowProjection;
};

struct PointLight {
	Light	light;
	vec3	position;
};

struct DirectionalLight {
	Light	light;
	vec3	direction;
};

vec4 calcLight(Light light, vec3 direction, vec3 normal, vec3 fragPos)
{
    float diffuse_factor = dot(direction, -normal);
	vec4 diffuse_color = vec4(0.0);
	vec4 specular_color = vec4(0.0);

    if (diffuse_factor > 0)
       diffuse_color = vec4(light.color.rgb * light.intensity * diffuse_factor, 1.0);

    vec3 direction_to_eye = normalize(viewPosition - fragPos);
    vec3 reflection_direction = normalize(reflect(-direction, normal));

    return  vec4(diffuse_color.rgb + specular_color.rgb, 1);
}

vec4 calcPointLight(PointLight light, vec3 normal, vec3 fragPos)
{
	vec3 direction = normalize(fragPos - light.position);
	float dist = distance(light.position, fragPos);
	float intensity = 1.0 / dist * light.light.intensity;
	direction = normalize(direction);
	vec4 color = calcLight(light.light, direction, normal, fragPos);
	return color * intensity;
}

vec4 calcDirectionalLight(DirectionalLight light, vec3 normal, vec3 fragPos)
{
    return calcLight(light.light, light.direction, normal, fragPos);
}

float calcBlurredShadowFactor(sampler2D shadowMap, mat4 lightPosition, vec4 fragPosition, int size, float blur)
{
    float result = 0.0;

	vec4 lightProjection = lightPosition * fragPosition;
    vec3 coords = lightProjection.xyz / lightProjection.w * 0.5 + 0.5;

	if (coords.x < 0 
		|| coords.y < 0 
		|| coords.z < 0 
		|| coords.x > 1 
		|| coords.y > 1 
		|| coords.z > 1)
		return 1.0;

    float currentDepth = coords.z;
	float closestDepth = texture(shadowMap, coords.xy).r;
	float shadowDistance = (currentDepth - closestDepth) * blur;
	
	float texelSize = 1.0 / 2048.0;
    for (int x = -size; x <= size; x++)
    {
        for (int y = -size; y <= size; y++)
        {
            vec2 offset = vec2(float(x), float(y)) * shadowDistance;
            float depth = texture(shadowMap, coords.xy + offset * texelSize * 0.5).r;
            result += currentDepth + 0.00002 < depth ? 1.0 : 0.0;
        }
    }

    return result / ((size * 2 + 1) * (size * 2 + 1));
}

float calcShadowFactor(sampler2D shadowMap, mat4 lightPosition, vec4 fragPosition)
{
	vec4 lightProjection = lightPosition * fragPosition;
    vec3 coords = lightProjection.xyz / lightProjection.w * 0.5 + 0.5;

	if (coords.x < 0 
		|| coords.y < 0 
		|| coords.z < 0 
		|| coords.x > 0.999 
		|| coords.y > 0.999 
		|| coords.z > 0.999)
		return 1.0;

    float currentDepth = coords.z;
	float closestDepth = texture(shadowMap, coords.xy).r;

    if (currentDepth + 0.0001 > closestDepth)
        return 0.0;
	return 1.0;
}
