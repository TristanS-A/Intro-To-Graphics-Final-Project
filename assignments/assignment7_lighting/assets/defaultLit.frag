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
};

struct Material {
	float ambientK;
	float diffuseK;
	float shininess;
	float specular;
};

#define MAX_LIGHTS 5
uniform int _CurrMaxLights;
uniform Light _Lights[MAX_LIGHTS];
uniform sampler2D _Texture;
uniform vec3 _CamPos;
uniform Material _Mat;
uniform int _BlinnPhong;

//Calculates light color values
float getLightEQ(vec3 newNormals, vec3 lightToFragVec, vec3 halfVecPart, vec3 camToFragVec, float dist){
	float lightVal = _Mat.ambientK; //Ambient calculations part

	lightVal += _Mat.diffuseK * max(dot(newNormals, lightToFragVec), 0) * dist; //Diffuse calculations part

	//My attempt at branchless programming but idk if the cost of the extra math is worth it
	//lightVal += _Mat.specular * pow(max(dot(newNormals, normalize(halfVecPart / length(halfVecPart))) * _BlinnPhong + dot(reflect(-lightToFragVec, newNormals), camToFragVec) * abs(_BlinnPhong - 1), 0), _Mat.shininess) * dist; //Specular calculations part

	//The branch in case its less cost than the extra math (for specular calculations part)
	if (_BlinnPhong == 1){
		lightVal += _Mat.specular * pow(max(dot(newNormals, normalize(halfVecPart / length(halfVecPart))), 0), _Mat.shininess) * dist;
	}
	else {
		lightVal += _Mat.specular * pow(max(dot(reflect(-lightToFragVec, newNormals), camToFragVec), 0), _Mat.shininess) * dist;
	}
	return lightVal;
}

void main(){
	//Calculate world normals and makes light variable
	vec3 newNormals = normalize(fs_in.WorldNormals);
	vec3 lightColor = vec3(0.0f, 0.0f, 0.0f);

	//Base distence for light range
	float baseDistance = 2.0;

	//Calculates v vector
	vec3 camToFragVec = normalize(_CamPos - fs_in.WorldPos);

	//Run throug all current lights to calculate w vector and half vector and add colors
	for (int i = 0; i < _CurrMaxLights; i++){
		vec3 lightToFragVec = _Lights[i].position - fs_in.WorldPos;

		//Gets light distance from mesh
		float lightDist = length(lightToFragVec);

		lightToFragVec = normalize(lightToFragVec);
		vec3 halfVecPart = normalize(lightToFragVec + camToFragVec);

		lightColor += _Lights[i].color * getLightEQ(newNormals, lightToFragVec, halfVecPart, camToFragVec, baseDistance / lightDist);
	}

	//Sets texture and adds light colors
	vec4 newColor = texture(_Texture,fs_in.UV);
	FragColor = vec4(newColor.rgb * lightColor, 1.0);
}