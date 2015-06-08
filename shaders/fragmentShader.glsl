#version 150

// struct describing a light source
struct Light
{
	vec3 color;
	vec3 direction;	// zero vector = point light
	vec3 position;

	// intensity = power / ((distance)^(2 * attenuation))
	float power;		// usually 1 if there's no attenuation
	float attenuation;	// 0 means no attenuation

	// only used for spot lights:
	float angle;	// angle between axis and edge of spot light cone, 0 = directional light
	float blur;		// 0 = sharp cutoff, 1 = radial gradient
};

// maximum number of lights this shader can handle
const uint MAX_LIGHTS = uint(10);

in vec3 position;
in vec3 normal;
in vec2 textureUV;

out vec4 fragmentColor;

uniform vec3 lightAmbient = vec3(0, 0, 0);

uniform Light lights[MAX_LIGHTS];
uniform uint lightCount = uint(0);

uniform vec4 diffuseColor;
uniform vec4 specularColor;
uniform uint hasTexture = uint(0);
uniform sampler2D texture;

uniform vec3 cameraPosition;

// return a vector containing the normalized light direction as the first three
// elements and the light intensity as the fourth, given a point light source
vec4 pointLight(in Light light)
{
	vec3 displacement = position - light.position;
	float intensity = light.power;

	// attenuation is based on distance from point light location
	float squareDistance = dot(displacement, displacement);
	if (light.attenuation > 0 && squareDistance > 0)
	{
		intensity /= pow(squareDistance, light.attenuation);
	}

	return vec4(normalize(displacement), intensity);
}

// return a vector containing the normalized light direction as the first three
// elements and the light intensity as the fourth, given a directional light
// source
vec4 directionalLight(in Light light)
{
	float intensity = light.power;
	vec3 direction = normalize(light.direction);

	if (light.attenuation > 0)
	{
		// attenuation is based on distance from plane containing light location
		// and perpendicular to light direction
		vec3 displacement = position - light.position;
		float distance = dot(direction, displacement);
		if (distance <= 0)
		{
			// no light arrives from behind the directional light if attenuated
			intensity = 0;
		}
		else
		{
			intensity /= pow(distance*distance, light.attenuation);
		}
	}

	return vec4(direction, intensity);
}

// return a vector containing the normalized light direction as the first three
// elements and the light intensity as the fourth, given a spot light source
vec4 spotLight(in Light light)
{
	float intensity = light.power;
	vec3 displacement = position - light.position;
	vec3 direction = normalize(displacement);

	// no light arrives if outside the spot light cone
	float angle = degrees(acos(dot(direction, light.direction)));
	if (angle > light.angle)
	{
		intensity = 0;
	}
	else
	{
		// attenuation is based on distance from light location
		if (light.attenuation > 0)
		{
			float squareDistance = dot(displacement, displacement);
			if (squareDistance > 0)
			{
				intensity /= pow(squareDistance, light.attenuation);
			}
		}

		// blurring is based on angular distance from cone edge, proportional
		// to angle from edge to axis
		if (light.blur > 0)
		{
			float inFromEdge = (light.angle - angle) / light.angle;
			if (inFromEdge < light.blur)
			{
				intensity *= inFromEdge / light.blur;
			}
		}
	}

	return vec4(direction, intensity);
}

// calculates contributions of a given light source to diffuse and specular
// lighting of an object
void calculateLightContributions(in vec3 N, in vec3 E, in Light light,
								 inout vec4 diffuse, inout vec4 specular)
{
	// calculate direction and intensity of light from the given source at this
	// fragment
	vec4 directionAndIntensity =
		(vec3(0, 0, 0) == light.direction) ? pointLight(light) :
		(0 >= light.angle) ? directionalLight(light) : spotLight(light);

	if (0 >= directionAndIntensity.w)
	{
		// no light arriving from this source
		diffuse = vec4(0, 0, 0, diffuse.a);
		specular = vec4(light.color, 0);
	}
	else
	{
		// adjust light intensity
		vec3 color = light.color * directionAndIntensity.w;

		// diffuse
		vec3 L = -directionAndIntensity.xyz;
		float d = max( 0, dot( N, L ) );
		diffuse = vec4(diffuse.rgb * color * d, diffuse.a);

		// specular
		vec3 R = reflect( -L, N );
		float s = 0;
		if (0 < specular.a)
		{
			s = pow( max( 0, dot( E, R ) ), 128 * specular.a );
		}
		specular = vec4(specular.rgb * color, s);
	}
}

void main()
{
	// diffuse color
	vec4 diffuse = diffuseColor;
	if (bool(hasTexture))
	{
		diffuse = diffuse * texture2D( texture, textureUV );
	}

	// ambient light
	vec3 matte = diffuse.rgb * diffuse.a * lightAmbient;

	// calculations
	vec3 N = normalize( normal );
	vec3 E = normalize( cameraPosition - position );
	
	// sum individual diffuse and specular light sources
	vec4 currentDiffuse;
	vec4 currentSpecular;
	vec3 gloss = vec3(0, 0, 0);
	for(uint i = uint(0); i < MAX_LIGHTS && i < lightCount; ++i)
	{
		currentDiffuse = diffuse;
		currentSpecular = specularColor;
		calculateLightContributions(N, E, lights[i], currentDiffuse, currentSpecular);
		matte += currentDiffuse.rgb;
		gloss += currentSpecular.rgb * currentSpecular.a;
	}

	// combine ambient/diffuse with specular
	float glossAlpha = max(max(max(gloss.r, gloss.g), gloss.b), 0);
	float matteAlpha = max(min(diffuse.a, 1), 0);
	float finalAlpha = min(1, matteAlpha + glossAlpha);
	if (finalAlpha <= 0)
	{
		fragmentColor = vec4(gloss + matte, 0);
	}
	else
	{
		fragmentColor = vec4((gloss + (matte * matteAlpha)) / finalAlpha, finalAlpha);
	}
}
