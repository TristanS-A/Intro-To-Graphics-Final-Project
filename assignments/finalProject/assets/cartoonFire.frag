#version 450
out vec4 FragColor;

in Surface{
    vec2 UV;
    vec3 WorldPos;
    vec3 WorldNormals;
}fs_in;

uniform sampler2D _Texture;
uniform float _Time;

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

vec3 bumpMap(vec2 uv) {
    //Scales normal cords to each pixel on x and y axis
    vec2 s = 1.0 / vec2(500,500);

    float p =  fbm(uv);
    float h1 = fbm(uv + s * vec2(1.0, 0));
    float v1 = fbm(uv + s * vec2(0, 1.0));

    //Makes normal vector by finding the difference between x and y and multiplies magnitude of vector
    vec2 xy = (p - vec2(h1, v1)) * 100;

    //0.5 is added as a median to keep the value from going positive or negative
    return vec3(xy + 0.5, 1.0);
}

void main(){

    //Makes normals using a bump map with nosie cords and makes distortion taller with multiplying UV.y by 0.5 and
    //also moves distortion with time on y axis
    vec3 normal = bumpMap(vec2(fs_in.UV.x * 1.0, fs_in.UV.y * 0.5 - _Time * 0.5) * 2);

    //Calculates displacement from normals
    vec2 displacement = clamp((normal.xy - .5) * 0.1, -1., 1.);

    //makes new UV from displacement
    vec2 newUV = fs_in.UV + displacement;

    //Streches out UV.y to make fire taller and moves it with time on the y axis
    vec2 uvT = vec2(newUV.x * 1.0, newUV.y * 0.5 - _Time * 0.5);

    //Makes noise with altered noise cords (which is UV)
    float n = pow(fbm(8.0 * uvT), 1.0);

    //Makes gradiant for the fire to fall off
    float gradiant = pow(1.0 - newUV.y, 2.0) * 5.0;

    //Applies gradient to noise
    float newNoise = n * gradiant;

    //Gives color to the noise to look like fire
    vec3 color = newNoise * vec3(2.0 * n, 2.0 * n * n, n * n * n);

    //Makes the fire fade out more (To stop the black outline from the dark/low colors)
    float fadeOutAddition = 0.2f;

    //Changes opacity so that the black areas dissapear
    float opacity = step(0.5, length((color)));

    //Sets frag color to noise color and opacity
    //FragColor = vec4(vec3(newNoise), 1.0);
    FragColor = vec4(step(0.4, color), opacity);
}