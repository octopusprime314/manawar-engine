#version 430

out vec4 fragColor;
in       VsData { vec2 texCoordOut; }
vsData;

uniform sampler2D deferredTexture;
uniform sampler2D velocityTexture;

void main() {

    // Divide by 2 to prevent overblurring
    vec2 velocityVector = texture(velocityTexture, vsData.texCoordOut).xy / 8.0;
    vec4 result         = vec4(0.0);
    vec2 texCoords      = vsData.texCoordOut;

    result += texture(deferredTexture, texCoords) * 0.4;
    texCoords -= velocityVector;
    result += texture(deferredTexture, texCoords) * 0.3;
    texCoords -= velocityVector;
    result += texture(deferredTexture, texCoords) * 0.2;
    texCoords -= velocityVector;
    result += texture(deferredTexture, texCoords) * 0.1;
    fragColor = vec4(vec3(result.rgb), 1.0);
}