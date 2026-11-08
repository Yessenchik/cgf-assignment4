#version 410 core

in vec3 vWorldPosition;
in vec3 vWorldNormal;

uniform vec3 uCameraPosition;
uniform vec3 uLightPosition;
uniform vec3 uLightColor;
uniform float uAmbient;
uniform vec3 uSpecularColor;
uniform float uShininess;
uniform float uTextureScale;

out vec4 fragColor;

vec3 latticeGradient(vec3 cell) {
    vec3 h = vec3(dot(cell, vec3(127.1, 311.7, 74.7)),
                  dot(cell, vec3(269.5, 183.3, 246.1)),
                  dot(cell, vec3(113.5, 271.9, 124.6)));
    return -1.0 + 2.0 * fract(sin(h) * 43758.5453);
}

float gradientNoise(vec3 p) {
    vec3 cell = floor(p);
    vec3 f = fract(p);
    vec3 w = f * f * f * (f * (f * 6.0 - 15.0) + 10.0);

    float n000 = dot(latticeGradient(cell + vec3(0, 0, 0)), f - vec3(0, 0, 0));
    float n100 = dot(latticeGradient(cell + vec3(1, 0, 0)), f - vec3(1, 0, 0));
    float n010 = dot(latticeGradient(cell + vec3(0, 1, 0)), f - vec3(0, 1, 0));
    float n110 = dot(latticeGradient(cell + vec3(1, 1, 0)), f - vec3(1, 1, 0));
    float n001 = dot(latticeGradient(cell + vec3(0, 0, 1)), f - vec3(0, 0, 1));
    float n101 = dot(latticeGradient(cell + vec3(1, 0, 1)), f - vec3(1, 0, 1));
    float n011 = dot(latticeGradient(cell + vec3(0, 1, 1)), f - vec3(0, 1, 1));
    float n111 = dot(latticeGradient(cell + vec3(1, 1, 1)), f - vec3(1, 1, 1));

    float x00 = mix(n000, n100, w.x);
    float x10 = mix(n010, n110, w.x);
    float x01 = mix(n001, n101, w.x);
    float x11 = mix(n011, n111, w.x);
    return mix(mix(x00, x10, w.y), mix(x01, x11, w.y), w.z);
}

float fbm(vec3 p) {
    float sum = 0.0;
    float amplitude = 0.5;
    for (int octave = 0; octave < 5; octave++) {
        sum += amplitude * gradientNoise(p);
        p = p * 2.03 + vec3(1.7, 9.2, 3.1);
        amplitude *= 0.5;
    }
    return sum;
}

vec3 proceduralTexture(vec3 worldPosition) {
    vec3 p = worldPosition * uTextureScale;

    vec3 q = vec3(fbm(p),
                  fbm(p + vec3(5.2, 1.3, 2.8)),
                  fbm(p + vec3(1.7, 9.2, 4.4)));
    vec3 r = vec3(fbm(p + 1.6 * q + vec3(8.3, 2.8, 1.1)),
                  fbm(p + 1.6 * q + vec3(3.1, 7.4, 5.9)),
                  fbm(p + 1.6 * q + vec3(6.6, 0.7, 9.3)));
    float warped = fbm(p + 1.4 * r);

    float phase = dot(p, normalize(vec3(1.0, 0.6, 0.35))) * 4.0 + 5.0 * warped;
    float vein = 1.0 - smoothstep(0.0, 0.10, abs(sin(phase)));
    float halo = 1.0 - smoothstep(0.0, 0.45, abs(sin(phase)));
    float fineVein = 1.0 - smoothstep(0.0, 0.05, abs(sin(phase * 2.3 + 3.0 * r.y)));

    vec3 cream = vec3(0.94, 0.90, 0.83);
    vec3 warmGrey = vec3(0.58, 0.52, 0.50);
    vec3 rose = vec3(0.82, 0.60, 0.55);
    vec3 slate = vec3(0.20, 0.19, 0.24);
    vec3 gold = vec3(0.80, 0.62, 0.30);

    vec3 color = mix(cream, warmGrey, smoothstep(-0.25, 0.35, warped + 0.3 * q.x));
    color = mix(color, rose, 0.55 * smoothstep(0.1, 0.5, r.z));
    color = mix(color, gold, 0.35 * halo);
    color = mix(color, slate, 0.45 * fineVein);
    color = mix(color, slate, 0.9 * vein);
    return color;
}

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
    vec3 proceduralColor = proceduralTexture(vWorldPosition);
    fragColor = vec4(clamp(phong(proceduralColor, vWorldPosition, N), 0.0, 1.0), 1.0);
}
