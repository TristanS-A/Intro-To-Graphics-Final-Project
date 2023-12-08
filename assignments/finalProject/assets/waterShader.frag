#version 450
out vec4 FragColor;

in Surface{
    vec2 UV;
    vec3 WorldPos;
    vec3 WorldNormals;
    vec4 ClipSpace;
}fs_in;

uniform sampler2D _ReflectionTexture;
uniform sampler2D _NormalMap;
uniform float _Time;
in vec3 toCamVec;

void main(){
    vec2 distortion = (texture(_NormalMap, fs_in.UV + _Time * 0.05).rg * 2.0 - 1.0) * 0.02;
    vec2 ndc = (fs_in.ClipSpace.xy / fs_in.ClipSpace.w) * 0.5 + 0.5;
    vec2 reflectCords = vec2(ndc.x, -ndc.y) + distortion;
    reflectCords.x = clamp(reflectCords.x, 0.01, 0.99);
    reflectCords.y = clamp(reflectCords.y, -0.99, -0.01);

    float refractionVal = dot(normalize(toCamVec), vec3(0, 1, 0));
    refractionVal = pow(refractionVal, 4);

    FragColor = vec4(texture(_ReflectionTexture, reflectCords).rgb, refractionVal);
}