#version 330

out vec4 FragColor;

in vec3 facePos;

uniform sampler3D volume;
uniform sampler1D transferFunc;
uniform vec3 camPos;

uniform int maxRaySteps;
uniform float rayStepSize;
uniform float gradientStepSize;

uniform vec3 lightPosition;

vec3 CalculateNormal(vec3 texCoord)
{
	vec3 sample1, sample2;

	sample1.x = texture(volume, texCoord - vec3(gradientStepSize, 0.0f, 0.0f)).x;
	sample2.x = texture(volume, texCoord + vec3(gradientStepSize, 0.0f, 0.0f)).x;
	sample1.y = texture(volume, texCoord - vec3(0.0f, gradientStepSize, 0.0f)).x;
	sample2.y = texture(volume, texCoord + vec3(0.0f, gradientStepSize, 0.0f)).x;
	sample1.z = texture(volume, texCoord - vec3(0.0f, 0.0f, gradientStepSize)).x;
	sample2.z = texture(volume, texCoord + vec3(0.0f, 0.0f, gradientStepSize)).x;

	return normalize(sample2 - sample1);
}


vec4 CalculateLighting(vec4 color, vec3 N)
{
	vec3 lightDirection = normalize(facePos - lightPosition);
	vec4 diffuseLight = vec4(0.8f, 0.8f, 0.8f, 1.0f);
	vec4 specularLight = vec4(1.0f, 1.0f, 1.0f, 1.0f);

	vec3 L, H;

	L = normalize(lightDirection);
	H = normalize(L + normalize(-facePos));
	
	float diff = clamp(dot(N,L), 0.0f, 1.0f);
	float amb = 0.3f;
	vec4 spec = specularLight * pow (max(0.0f, dot(H, N)), 100.0f); 

	return ((color * diff) + spec + (color * amb));
}


void main()
{
	vec4 finalColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	vec3 position = facePos;
	vec3 texCoord;
	vec4 color;
	vec3 normal;

	vec3 direction = position - camPos;
	direction = normalize(direction);

	float absorption = 0.0f;
	float opacity;

	color = vec4(0.0f, 0.0f, 0.0f, 0.0f);
		
	texCoord = (position + 1.0f) / 2.0f; 

	FragColor = texture(volume, texCoord);

}