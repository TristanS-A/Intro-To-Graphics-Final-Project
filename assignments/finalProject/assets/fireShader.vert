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
uniform float _Time;

/*
//Makes pseudo random numbers between 0.0 and 1.0 based on input
float rand(vec2 cord){
    const float a = 17;
    const float b = 82;

    //The large c makes the number more difficult to predict when using sin which is periodic
    const float c = 80000;

    //Fract is the most important part of random function (Makees next value more unpredictable)
    return fract(sin(dot(cord.xy, vec2(a, b))) * c);
}

//Modified rand function to return two pseudo random values from -1.0 to 1.0 based on input
vec3 hash( vec3 p ) {
    p = vec3( dot(p,vec3(127.1,311.7, 213.4))),
    dot(p,vec3(269.5,183.3, 567.9));

    return -1.0 + 2.0*fract(sin(p) * 43758.5453123);
}

//Makes simplex noise
float noise( in vec3 p ) {
    const float K1 = 0.366025404; // (sqrt(3)-1)/2;
    const float K2 = 0.211324865; // (3-sqrt(3))/6;

    vec3 i = floor( p + (p.x+p.y+p.z) * K1 );

    vec3 a = p - i + (i.x+i.y+i.z) * K2;
    vec3 o = step(a.zyx,a.xyz);
    vec3 b = a - o + K2;
    vec3 c = a - 1.0 + 2.0*K2;

    vec3 h = max( 0.5-vec3(dot(a,a), dot(b,b), dot(c,c) ), 0.0 );

    vec3 n = h*h*h*h*vec3( dot(a,hash(i+0.0)), dot(b,hash(i+o)), dot(c,hash(i+1.0)));

    return dot( n, vec3(70.0) );
}

//Turns noise into fractal noise (Making it less blurry and mroe defined)
float fbm ( in vec3 p ) {
    float f = 0.0;

    //Matrix to scale up by two and rotate clockwise ~36.8 degrees
    mat3 m = mat3( 1.6,  1.2, 0.0, -1.2,  1.6 , 0.0, 0.0, 0.0, 1.0);

    f = 0.5000*noise(p);
    p = m*p;
    f += 0.2500*noise(p);
    p = m*p;
    f += 0.1250*noise(p);
    p = m*p;
    f += 0.0625*noise(p);
    p = m*p;
    f = 0.5 + 0.5 * f;
    return f;
}*/

//Modified rand function to return two pseudo random values from -1.0 to 1.0 based on input
vec2 hash( vec2 p ) {
    p = vec2( dot(p,vec2(127.1,311.7)),
    dot(p,vec2(269.5,183.3)) );

    return -1.0 + 2.0*fract(sin(p) * 43758.5453123);
}

//Makes simplex noise
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

//Turns noise into fractal noise (Making it less blurry and mroe defined)
float fbm ( in vec2 p ) {
    float f = 0.0;

    //Matrix to scale up by two and rotate clockwise ~36.8 degrees
    mat2 m = mat2( 1.6,  1.2, -1.2,  1.6 );

    f = 0.5000*noise(p);
    p = m*p;
    f += 0.2500*noise(p);
    p = m*p;
    f += 0.1250*noise(p);
    p = m*p;
    f += 0.0625*noise(p);
    p = m*p;
    f = 0.5 + 0.5 * f;

    return f;
}

void main(){
    vs_out.UV = vUV;

    //This was an attempt to animate vPos to a bumpMap but it would mess up the seems of the model and also it looked a bit wierd sometimes

    //vec3 normal = bumpMap(vec3(vNormal.x * 1.0, vNormal.y * 0.5 - _Time, vNormal.z) * 2.0);
    //vec3 displacement = clamp((normal.xyz - 0.5) * 0.5, -1., 1.);

    //Scales how much noise (black and white "spots") is in the noise
    float noiseFrequency = 1.0;

    //Uses noise as a height map using normals so that seems (with the same normals) don't seperate and animates with time
    float height = 0.5 + 0.5 * pow(noise(noiseFrequency * vec2(vNormal.x * vNormal.z, vNormal.y * vNormal.z - _Time)), 1.0);
    //float height = pow(fbm(0.2 * vec2(vNormal.x * vNormal.z, vNormal.y * vNormal.z - _Time)), 1.0);

    //Scales the overall size of the mesh for each vertex
    float overallScale = 0.5f;

    //Scales distortion power
    float distortionPower = 0.6;

    //Adds distortion to vertex in the direction of the normals
    vec3 newPos = (vPos + normalize(vNormal) * (height * distortionPower)) * overallScale;

    vs_out.WorldPos = vec3(_Model * vec4(newPos,1.0));
    vs_out.WorldNormals = transpose(inverse(mat3(_Model))) * vNormal;
    gl_Position = _ViewProjection * _Model * vec4(newPos,1.0);
}