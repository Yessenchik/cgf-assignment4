#version 410 core

in vec3 vWorldPosition;
in vec3 vWorldNormal;
in vec2 vTexCoord;

uniform sampler2D uDiffuseTexture;
uniform vec3 uCameraPosition;
uniform vec3 uLightPosition;
uniform vec3 uLightColor;
uniform float uAmbient;
uniform vec3 uSpecularColor;
uniform float uShininess;

out vec4 fragColor;

vec3 phong(vec3 diffuseColor, vec3 P, vec3 N) {
    vec3 L = normalize(uLightPosition - P);
    vec3 V = normalize(uCameraPosition - P);
    vec3 R = reflect(-L, N);

    float NdotL = max(dot(N, L), 0.0);
    vec3 ambient = uAmbient * diffuseColor;
    vec3 diffuse = diffuseColor * NdotL;
    vec3 specular = NdotL > 0.0 ? uSpecularColor * pow(max(dot(R, V), 0.0), uShininess) : vec3(0.0);

    return ambient + uLightColor * (diffuse + specular);
}

void main() {
    vec3 N = normalize(vWorldNormal);
    if (!gl_FrontFacing) {
        N = -N;
    }

    vec3 diffuseColor = texture(uDiffuseTexture, vTexCoord).rgb;
    fragColor = vec4(clamp(phong(diffuseColor, vWorldPosition, N), 0.0, 1.0), 1.0);
}
