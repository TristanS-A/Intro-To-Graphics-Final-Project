#version 450
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vUV;

uniform mat4 _Model;
uniform vec3 _NWorldVec;
uniform mat4 _ViewProjection;

out vec2 UV;
out vec3 finalLightColor;

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
uniform vec3 _CamPos;
uniform Material _Mat;
uniform int _BlinnPhong;

//Calculates light color values
float getLightEQ(vec3 newNormals, vec3 lightToFragVec, vec3 halfVecPart, vec3 camToFragVec, float dist){
    float lightVal = _Mat.ambientK; //Ambient calculations part

    lightVal += _Mat.diffuseK * max(dot(newNormals, lightToFragVec), 0) * dist; //Diffuse calculations part

    //My attempt at branchless programming but idk if the cost of the extra math is worth it
    //lightVal += _Mat.specular * pow(max(dot(newNormals, normalize(halfVecPart / length(halfVecPart))), 0) * _BlinnPhong + dot(reflect(-lightToFragVec, newNormals), camToFragVec) * abs(_BlinnPhong - 1), 0), _Mat.shininess) * dist; //Specular calculations part

    //The branch in case its less cost than the extra math (for specular calculations)
    if (_BlinnPhong == 1){
        //Blinn-Phong calculations
        lightVal += _Mat.specular * pow(max(dot(newNormals, normalize(halfVecPart / length(halfVecPart))), 0), _Mat.shininess) * dist;
    }
    else {
        //Phong calculations
        lightVal += _Mat.specular * pow(max(dot(reflect(-lightToFragVec, newNormals), camToFragVec), 0), _Mat.shininess) * dist;
    }

    return lightVal;
}

void main(){
    //Sets UV to send to fragment shader
    UV = vUV;

    //Calculate world normals and pos and makes light variable
    vec3 WorldPos = vec3(_Model * vec4(vPos,1.0));
    vec3 WorldNormals = normalize(transpose(inverse(mat3(_Model))) * vNormal);
    vec3 lightColor = vec3(0.0f, 0.0f, 0.0f);

    //Base distence for light range
    float baseDistance = 2.0;

    //Calculates v vector
    vec3 camToFragVec = normalize(_CamPos - WorldPos);

    //Run throug all current lights to calculate w vector and add colors
    for (int i = 0; i < _CurrMaxLights; i++){
        vec3 lightToFragVec = _Lights[i].position - WorldPos;

        //Gets light distance from mesh
        float lightDist = length(lightToFragVec);

        lightToFragVec = normalize(lightToFragVec);
        vec3 halfVecPart = normalize(lightToFragVec + camToFragVec);
        lightColor += _Lights[i].color * getLightEQ(WorldNormals, lightToFragVec, halfVecPart, camToFragVec, baseDistance / lightDist);
    }

    //Sets texture and adds light colors
    finalLightColor = lightColor;
    gl_Position = _ViewProjection * _Model * vec4(vPos,1.0);
}