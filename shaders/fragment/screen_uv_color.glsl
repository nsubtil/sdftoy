#version 400

uniform vec2 screen_resolution;
out vec4 fragColor;

void main(void)
{
    fragColor = vec4(gl_FragCoord.xy / screen_resolution, 0.0, 1.0);
}
