#version 450
out vec4 FragColor;

in vec2 UV;
in vec3 finalLightColor;
uniform sampler2D _Texture;

void main() {
    vec4 newColor = texture(_Texture, UV);
    FragColor = vec4(newColor.rgb * finalLightColor, 1.0);
}
