#version 450
out vec4 FragColor;

in Surface{
    vec2 UV;
    vec3 WorldPos;
    vec3 WorldNormals;
    vec4 ClipSpace;
}fs_in;

struct Light {
    vec3 position;
    vec3 color;
    float range;
    float power;
};

uniform sampler2D _ReflectionTexture;
uniform sampler2D _NormalMap;
uniform float _Time;
uniform float _DistortionSpeed;
uniform float _Tileing;
uniform bool _EnableSpecHighlights;
in vec3 toCamVec;

#define MAX_LIGHTS 2
uniform Light _Lights[MAX_LIGHTS];

void main(){

    //Calculate distortion from normal texture
    vec2 distortionCords = texture(_NormalMap, fs_in.UV * _Tileing + _Time * 0.05 * _DistortionSpeed).rg * 0.1;
    distortionCords = fs_in.UV * _Tileing + distortionCords;

    //Gets normal map color and makes normal vector from it
    vec4 waterNormalColor = texture(_NormalMap, distortionCords);
    vec3 waterNormalVec = normalize(vec3(waterNormalColor.r * 2.0 - 1.0, waterNormalColor.b * 2.0 - 1.0, waterNormalColor.g * 2.0 - 1.0));

    //Specular lighting calculations
    vec3 specularHighlights = vec3(0, 0, 0);

    for (int i = 0; i < MAX_LIGHTS; i++){

        //Scales light color by light power
        vec3 newColor = _Lights[i].color * _Lights[i].power;

        //Base distence for light range
        float baseDistance = _Lights[i].range;

        //Gets vector from worldPos to light
        vec3 lightToFragVec = _Lights[i].position - fs_in.WorldPos;

        //Gets light distance from mesh
        float lightDist = length(lightToFragVec) + 0.00001;

        lightToFragVec = normalize(lightToFragVec);

        //Makes half vector for BLin-Phong specular calculations
        vec3 halfVecPart = normalize(lightToFragVec + toCamVec);

        //Calculates Blin-Phong specular highlights and scales intensity by distence using logorythmic falloff
        float lightVal = 1 * pow(max(dot(waterNormalVec, normalize(halfVecPart / length(halfVecPart))), 0), 50) * clamp(baseDistance - 1.2 * log(lightDist), 0, 500);

        //Adds lights to specular highlight color
        specularHighlights += newColor * lightVal;
    }

    //Calculates stepped water color
    float waterStep = step(texture(_NormalMap, fs_in.UV * _Tileing + _Time * 0.05 * _DistortionSpeed).r * 0.1, 0.07);
    vec3 waterCol = mix(vec3(1.0, 1.0, 1.0), vec3(0.0, 0.5, 1.0), waterStep);
    waterStep = step(texture(_NormalMap, (fs_in.UV) * _Tileing + _Time * 0.05).r * 0.1, 0.05);
    waterCol = mix(waterCol, vec3(0.0, 0.4, 0.9), waterStep);

    vec3 color;

    //Branch to add specular highlights to water
    if (_EnableSpecHighlights){
        color = waterCol + vec3(step(1.0, specularHighlights));
    }
    else{
        color = waterCol;
    }


    FragColor = vec4(color, 1.0);
}