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

    //Calculates displacement
    vec2 distortionCords = texture(_NormalMap, fs_in.UV * _Tileing + _Time * 0.05 * _DistortionSpeed).rg * 0.1;
    distortionCords = fs_in.UV * _Tileing + distortionCords;
    vec2 totalDistortion = (texture(_NormalMap, distortionCords).rg * 2.0 - 1.0) * 0.1;

    //Calculates textcords in clip space
    vec2 ndc = (fs_in.ClipSpace.xy / fs_in.ClipSpace.w) * 0.5 + 0.5;

    //Gets reflect text cords with distortion
    vec2 reflectCords = vec2(ndc.x, -ndc.y) + totalDistortion;

    //Clamp reflect distortion cords due to pixel wrapping bug
    reflectCords.x = clamp(reflectCords.x, 0.01, 0.99);
    reflectCords.y = clamp(reflectCords.y, -0.99, -0.01);

    //Calculates the fresnel effect value
    float fresnelVal = dot(toCamVec, vec3(0, 1, 0));

    //Scales fresnel value
    fresnelVal = pow(fresnelVal, 4);

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

    //Samples from reflection texture with reflection cords
    vec4 reflectTextColor = texture(_ReflectionTexture, reflectCords);

    vec4 waterColor = vec4(0, 0.5, 0.9, 1.0);

    vec4 color;

    //Branch to turn on/off specular highlights
    if (_EnableSpecHighlights) {

        //Mixes between reflection color and waterColor using specular highlights
        color = mix(vec4(reflectTextColor) + vec4(specularHighlights, 0.0), waterColor, 0.3);
    }
    else{
        //Mixes betweem reflection color and water color wihtout specular highlights
        color = mix(vec4(reflectTextColor), waterColor, 0.3);
    }

    //Mixes between new color and 0 opacity (To see under/through the water
    color = mix(color, vec4(0, 0, 0, 0), fresnelVal);

    FragColor = vec4(color);
}