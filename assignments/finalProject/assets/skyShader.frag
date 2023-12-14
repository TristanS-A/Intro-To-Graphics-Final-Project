#version 450
out vec4 FragColor;

uniform sampler2D _Texture;

in Surface{
    vec2 UV;
    vec3 WorldPos;
    vec3 WorldNormals;
}fs_in;

void main(){


	FragColor = texture(_Texture, fs_in.UV);
}