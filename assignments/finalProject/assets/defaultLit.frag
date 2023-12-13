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
};

struct Material {
	float ambientK;
	float diffuseK;
	float shininess;
	float specular;
};

#define MAX_LIGHTS 1
uniform Light _Lights[MAX_LIGHTS];
uniform sampler2D _Text_texture_diffuse;
uniform vec3 _CamPos;
uniform Material _Mat;
uniform float _Dist;

//Calculates light color values
float getLightEQ(vec3 newNormals, vec3 lightToFragVec, vec3 halfVecPart, vec3 camToFragVec, float dist){
	float lightVal = _Mat.ambientK; //Ambient calculations part

	lightVal += _Mat.diffuseK * max(dot(newNormals, lightToFragVec), 0) * dist; //Diffuse calculations part

	//Specular calculations
	lightVal += _Mat.specular * pow(max(dot(newNormals, normalize(halfVecPart / length(halfVecPart))), 0), _Mat.shininess) * dist;
	return lightVal;
}

void main(){
	//Calculate world normals and makes light variable
	vec3 newNormals = normalize(fs_in.WorldNormals);
	vec3 lightColor = vec3(0.0f, 0.0f, 0.0f);

	//Calculates v vector
	vec3 camToFragVec = normalize(_CamPos - fs_in.WorldPos);

	//Run throug all current lights to calculate w vector and half vector and add colors
	for (int i = 0; i < MAX_LIGHTS; i++){
		vec3 lightToFragVec = _Lights[i].position - fs_in.WorldPos;

		//Gets light distance from mesh
		float lightDist = length(lightToFragVec);

		//Calculates half vec
		lightToFragVec = normalize(lightToFragVec);
		vec3 halfVecPart = normalize(lightToFragVec + camToFragVec);

		//Adds to final light color
		lightColor += _Lights[i].color * getLightEQ(newNormals, lightToFragVec, halfVecPart, camToFragVec, _Lights[i].range / lightDist);
	}

	//Sets texture and adds light colors
	vec4 newColor = texture(_Text_texture_diffuse,fs_in.UV);
	FragColor = vec4(newColor.rgb * lightColor, 1.0);
}