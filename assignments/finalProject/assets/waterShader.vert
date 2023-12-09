#version 450
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vUV;

out Surface{
    vec2 UV;
    vec3 WorldPos;
    vec3 WorldNormals;
    vec4 ClipSpace;
}vs_out;

uniform mat4 _Model;
uniform vec3 _NWorldVec;
uniform mat4 _ViewProjection;
uniform vec3 _CamPos;
out vec3 toCamVec;
uniform sampler2D _NormalMap;
uniform float _Time;

void main(){
    float distortionScale = 0.01;
    vec2 distortionCords = texture(_NormalMap, vUV * distortionScale + _Time * 0.01).rg * 0.1;
    distortionCords = vUV * distortionScale + distortionCords;
    float height = (texture(_NormalMap, distortionCords).r * 2.0 - 1.0) * 0.02;

    //Scales distortion power
    float distortionPower = 50;

    //Adds distortion to vertex in the direction of the normals
    vec3 newPos = (vPos + normalize(vNormal) * (height * distortionPower));

    vs_out.UV = vUV;
    vs_out.WorldPos = vec3(_Model * vec4(newPos,1.0));
    vs_out.WorldNormals = transpose(inverse(mat3(_Model))) * vNormal;
    vs_out.ClipSpace = _ViewProjection * _Model * vec4(newPos,1.0);

    toCamVec = normalize(_CamPos - vs_out.WorldPos);
    gl_Position = vs_out.ClipSpace;
}