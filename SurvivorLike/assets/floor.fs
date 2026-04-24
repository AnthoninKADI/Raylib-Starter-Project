#version 330

in vec2 fragTexCoord;
out vec4 finalColor;

uniform vec2 cameraPos;

float hash(vec2 p)
{
    return fract(sin(dot(p, vec2(127.1,311.7))) * 43758.5453123);
}

float noise(vec2 p)
{
    vec2 i = floor(p);
    vec2 f = fract(p);

    float a = hash(i);
    float b = hash(i + vec2(1.0, 0.0));
    float c = hash(i + vec2(0.0, 1.0));
    float d = hash(i + vec2(1.0, 1.0));

    vec2 u = f*f*(3.0-2.0*f);

    return mix(a,b,u.x) + (c-a)*u.y*(1.0-u.x) + (d-b)*u.x*u.y;
}

void main()
{
    // 🔥 IMPORTANT : world space stable
    vec2 world = fragTexCoord * 2000.0 + cameraPos;

    float n = noise(world * 0.01);

    vec3 grass = vec3(0.12, 0.55, 0.12);
    vec3 dirt  = vec3(0.45, 0.35, 0.25);

    vec3 col = mix(grass, dirt, n);

    // route verticale
    float road = abs(fract(world.x * 0.001) - 0.5);
    road = smoothstep(0.1, 0.15, road);

    vec3 roadColor = vec3(0.2, 0.2, 0.2);

    col = mix(roadColor, col, road);

    finalColor = vec4(col, 1.0);
}