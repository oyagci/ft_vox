in vec3 viewPosition;
in vec3 viewDirection;

struct Light {
	vec4	color;
	float	intensity;
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
	return calcLight(light.light, direction, normal, fragPos);
}

vec4 calcDirectionalLight(DirectionalLight light, vec3 normal, vec3 fragPos)
{
    return calcLight(light.light, light.direction, normal, fragPos);
}
