#version 450
out vec4 FragColor;

in Surface{
    vec2 UV;
    vec3 WorldPos;
    vec3 WorldNormals;
    vec4 ClipSpace;
}fs_in;

uniform sampler2D _Text_texture_diffuse;

void main(){
    vec2 ndc = (fs_in.ClipSpace.xy / fs_in.ClipSpace.w) * 0.5 + 0.5;
    vec2 reflectCords = vec2(ndc.x, -ndc.y);
    FragColor = texture(_Text_texture_diffuse, reflectCords);
}