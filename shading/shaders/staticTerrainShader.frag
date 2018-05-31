#version 330

in vec3 normalOut;
in vec2 texOut;
in vec3 positionOut;

layout(location = 0) out vec4 color4;
layout(location = 1) out vec4 normal4;
layout(location = 2) out vec4 position4;

// https://gist.github.com/patriciogonzalezvivo/670c22f3966e662d2f83
// Simplex 2D noise
vec3 permute(vec3 x) { return mod(((x*34.0)+1.0)*x, 289.0); }

float snoise(vec2 v){
  const vec4 C = vec4(0.211324865405187, 0.366025403784439,
           -0.577350269189626, 0.024390243902439);
  vec2 i  = floor(v + dot(v, C.yy) );
  vec2 x0 = v -   i + dot(i, C.xx);
  vec2 i1;
  i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
  vec4 x12 = x0.xyxy + C.xxzz;
  x12.xy -= i1;
  i = mod(i, 289.0);
  vec3 p = permute( permute( i.y + vec3(0.0, i1.y, 1.0 ))
  + i.x + vec3(0.0, i1.x, 1.0 ));
  vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy),
    dot(x12.zw,x12.zw)), 0.0);
  m = m*m ;
  m = m*m ;
  vec3 x = 2.0 * fract(p * C.www) - 1.0;
  vec3 h = abs(x) - 0.5;
  vec3 ox = floor(x + 0.5);
  vec3 a0 = x - ox;
  m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );
  vec3 g;
  g.x  = a0.x  * x0.x  + h.x  * x0.y;
  g.yz = a0.yz * x12.xz + h.yz * x12.yw;
  return 130.0 * dot(m, g);
}

void main(){
    //  gradient = [
    //       Start   Color
    //      (-1.000, (0, 0, 128)),        # deep water
    //      (-0.150, (0, 0, 255)),        # shallow water
    //      (-0.050, (0, 128, 255)),      # shore
    //      (0.000,  (0xFF, 0xEC, 0x96)), # sand
    //      (0.060,  (32, 160, 0)),       # grass
    //      (0.400,  (128, 128, 128)),    # rock
    //      (0.500,  (96, 96, 96)),       # rock
    //      (0.600,  (255, 255, 255)),    # snow
    //  ]

    // These threshold represent the top of the range to use the color.
    // e.g. The value of t_sand is the highest height colored with sand.
    float t_sand   = 0.25;
    float t_grass1 = 1.00; // Coordinate these with ::GenerateTrees()
    float t_grass2 = 1.80; //
    float t_rock1  = 2.20;
    float t_rock2  = 2.50;
    float t_snow   = 2.75;

    vec3 c_sand   = vec3(255, 236, 150);
    vec3 c_grass1 = vec3(32, 160, 0);
    vec3 c_grass2 = vec3(45, 134, 23);
    vec3 c_rock1  = vec3(128, 128, 128);
    vec3 c_rock2  = vec3(96, 96, 96);
    vec3 c_snow   = vec3(230, 230, 230);

    // Coordinate this with `::ScaleNoiseToTerrainHeight()`
    float min = -2.0;
    float max =  1.7;
    float wiggle = 1.0 + snoise(texOut / 10) / 20;
    float height = texOut.x * wiggle;

    vec3 color3;
    if (height < t_sand) {
        color3 = c_sand;
    } else if (height < t_grass1) {
        color3 = c_grass1;
    } else if (height < t_grass2) {
        color3 = c_grass2;
    } else if (height < t_rock1) {
        color3 = c_rock1;
    } else if (height < t_rock2) {
        color3 = c_rock2;
    } else {
        color3 = c_snow;
    }

    color4    = vec4(color3/255.0, 1.0);
    normal4   = vec4(normalize(normalOut), 1.0);
    position4 = vec4(positionOut.xyz, 1.0);
}
