#version 300 es

precision mediump float;

uniform sampler2D u_shadowMapTexture;
uniform vec2 u_shadowMapSize;
uniform vec4 u_lightColor;
uniform float u_lightIndex;

in vec2 v_texCoord;
out vec4 o_fragColor;

const float PI = 3.1415;

float sample(vec2 coord, float r)
{
    return step(r, texture(u_shadowMapTexture, coord).r);
}

void main()
{
    //rectangular to polar
    vec2 norm = v_texCoord.st * 2.0 - 1.0;
    float theta = atan(norm.y, norm.x);
    
//    /* directional light */
//    if (theta < -PI / 2.0 || theta > PI / 2.0) {
//        o_fragColor = vec4(0.0, 0.0, 0.0, 0.0);
//        return;
//    }
    
    float r = length(norm);
    float coord = (-theta + PI) / (2.0 * PI);
    
    //the tex coord to sample out shadow map
    vec2 tc = vec2(coord, u_lightIndex / (u_shadowMapSize.t - 1.0));
    
    //the center tex coord, which gives hard shadows
    float center = sample(tc, r);
    
    //we multiply the blur amount by our distance from center
    //this leads to more blurriness as the shadow "fades away"
    float blur = (1.0 / u_shadowMapSize.s) * smoothstep(0.0, 1.0, r);
    
    // gaussian blur
    float sum = 0.0;
    
    sum += sample(vec2(tc.x - 4.0 * blur, tc.y), r) * 0.05;
    sum += sample(vec2(tc.x - 3.0 * blur, tc.y), r) * 0.09;
    sum += sample(vec2(tc.x - 2.0 * blur, tc.y), r) * 0.12;
    sum += sample(vec2(tc.x - 1.0 * blur, tc.y), r) * 0.15;
    
    sum += center * 0.16;
    
    sum += sample(vec2(tc.x + 1.0 * blur, tc.y), r) * 0.15;
    sum += sample(vec2(tc.x + 2.0 * blur, tc.y), r) * 0.12;
    sum += sample(vec2(tc.x + 3.0 * blur, tc.y), r) * 0.09;
    sum += sample(vec2(tc.x + 4.0 * blur, tc.y), r) * 0.05;
    
    float lit = mix(center, sum, 1.0);
    
    //multiply the summed amount by our distance, which gives us a radial falloff
    //then multiply by vertex (light) color
    o_fragColor = u_lightColor * vec4(vec3(1.0), lit * smoothstep(1.0, 0.0, r));
}
