#version 450
out vec4 FragColor;

in Surface{
	vec2 UV;
	vec3 WorldPos;
	vec3 WorldNormals;
}fs_in;

uniform sampler2D _Text_texture_diffuse;

void main(){
    FragColor = texture(_Text_texture_diffuse,fs_in.UV);
}