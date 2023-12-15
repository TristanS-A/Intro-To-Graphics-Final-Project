#version 450
out vec4 FragColor;

uniform sampler2D _Texture;
uniform sampler2D _Texture2;

in Surface{
    vec2 UV;
    vec3 WorldPos;
    vec3 WorldNormals;
}fs_in;

void main(){
    if (fs_in.WorldPos.y < -49.9f) {
        FragColor = texture(_Texture2, fs_in.UV);
    }
    else {
	    FragColor = texture(_Texture, vec2(1 - fs_in.UV.x, 1 - fs_in.UV.y));
    }
}