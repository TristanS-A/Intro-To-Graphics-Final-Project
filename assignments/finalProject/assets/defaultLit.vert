#version 450
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vUV;

out Surface{
	vec2 UV;
	vec3 WorldPos;
	vec3 WorldNormals;
}vs_out;

uniform mat4 _Model;
uniform vec3 _NWorldVec;
uniform mat4 _ViewProjection;
uniform vec4 _ClipPlane;


void main(){
	vs_out.UV = vUV;
	vs_out.WorldPos = vec3(_Model * vec4(vPos,1.0));
	vs_out.WorldNormals = transpose(inverse(mat3(_Model))) * vNormal;
	gl_ClipDistance[0] = dot(_Model * vec4(vPos,1.0), _ClipPlane);
	gl_Position = _ViewProjection * _Model * vec4(vPos,1.0);
}