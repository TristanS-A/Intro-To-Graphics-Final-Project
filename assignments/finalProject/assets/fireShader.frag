#version 450
out vec4 FragColor;

in Surface{
	vec2 UV;
	vec3 WorldPos;
	vec3 WorldNormals;
}fs_in;

uniform sampler2D _Texture;

float rand(vec2 cord){
    const float a = 17;
    const float b = 82;
    const float c = 80000;
    return fract(sin(dot(cord.xy, vec2(a, b))) * c);
}

vec2 hash( vec2 p ) {
	p = vec2( dot(p,vec2(127.1,311.7)),
			  dot(p,vec2(269.5,183.3)) );

	return -1.0 + 2.0*fract(sin(p) * 43758.5453123);
}

float noise( in vec2 p ) {
    const float K1 = 0.366025404; // (sqrt(3)-1)/2;
    const float K2 = 0.211324865; // (3-sqrt(3))/6;

	vec2 i = floor( p + (p.x+p.y) * K1 );

    vec2 a = p - i + (i.x+i.y) * K2;
    vec2 o = step(a.yx,a.xy);
    vec2 b = a - o + K2;
	vec2 c = a - 1.0 + 2.0*K2;

    vec3 h = max( 0.5-vec3(dot(a,a), dot(b,b), dot(c,c) ), 0.0 );

	vec3 n = h*h*h*h*vec3( dot(a,hash(i+0.0)), dot(b,hash(i+o)), dot(c,hash(i+1.0)));

    return dot( n, vec3(70.0) );
}

float fbm ( in vec2 p ) {
    float f = 0.0;
    mat2 m = mat2( 1.6,  1.2, -1.2,  1.6 );
    f  = 0.5000*noise(p); p = m*p;
    f += 0.2500*noise(p); p = m*p;
    f += 0.1250*noise(p); p = m*p;
    f += 0.0625*noise(p); p = m*p;
    f = 0.5 + 0.5 * f;
    return f;
}

vec3 bumpMap(vec2 uv) {
    vec2 s = vec2(1.0, 1.0);
    //vec2 s = 1. / resolution.xy;

    float p =  fbm(uv);
    float h1 = fbm(uv + s * vec2(1., 0));
    float v1 = fbm(uv + s * vec2(0, 1.));

   	vec2 xy = (p - vec2(h1, v1)) * 40.0;
    //vec2 xy = (p - vec2(h1, v1)) * normalStrength;

    return vec3(xy + .5, 1.);
}

void main(){
    vec3 normal = bumpMap(10 * fs_in.UV * vec2(1.0, 0.3) + 0 * 0);
    //float noiseCord = fbm(fs_in.UV * 40.0);

    //FragColor = texture(_Texture,fs_in.UV);

    FragColor = vec4(normal, 1.0);
}