#version 450
out vec4 FragColor;

in Surface{
	vec2 UV;
	vec3 WorldPos;
	vec3 WorldNormals;
}fs_in;

struct Light {
	vec3 position;
	vec3 color;
	float range;
	float power;
};

struct Material {
	float ambientK;
	float diffuseK;
	float shininess;
	float specular;
};

#define MAX_LIGHTS 2
uniform Light _Lights[MAX_LIGHTS];
uniform sampler2D _Text_texture_diffuse;
uniform vec3 _CamPos;
uniform Material _Mat;
uniform float _Dist;

void main(){
	//Calculate world normals and makes light variable
	vec3 newNormals = normalize(fs_in.WorldNormals);
	vec3 lightColor = vec3(0.0f, 0.0f, 0.0f);

	//Calculates v vector
	vec3 camToFragVec = normalize(_CamPos - fs_in.WorldPos);

	float rimLighting;

	//Run throug all current lights to calculate w vector and half vector and add colors
	for (int i = 0; i < MAX_LIGHTS; i++){
		vec3 lightToFragVec = _Lights[i].position - fs_in.WorldPos;

		//Scales light color by light power
		vec3 newColor = _Lights[i].color * _Lights[i].power;

		//Gets light distance from mesh
		float lightDist = length(lightToFragVec);

		//Calculates half vec
		lightToFragVec = normalize(lightToFragVec);
		vec3 halfVecPart = normalize(lightToFragVec + camToFragVec);

		float newRange = _Lights[i].range / lightDist;

		float tempLightColor = _Mat.ambientK; //Ambient calculations part

		//Calculates rim lighting and counts as diffuse
		rimLighting = _Mat.diffuseK * (1.0 - max(dot(-lightToFragVec, newNormals), 0.0)) * newRange;

		//Specular calculations
		tempLightColor += _Mat.specular * pow(max(dot(newNormals, normalize(halfVecPart / length(halfVecPart))), 0), _Mat.shininess) * newRange;

		//Smoothsteps the rim lighting
		tempLightColor += smoothstep(0.0, 1.0, rimLighting);

		lightColor += newColor * tempLightColor;
	}

	//Sets texture and adds light colors
	vec4 newColor = texture(_Text_texture_diffuse,fs_in.UV);
	FragColor = vec4(vec3(newColor.rgb * lightColor), 1.0);
}