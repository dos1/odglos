#ifdef GL_ES
precision highp float;
#endif

varying vec2 varying_texcoord;
varying vec4 varying_color;

void main() {
  vec4 color = varying_color;
  color *= 1.0 - abs(distance(varying_texcoord, vec2(0.5, 0.5))) * 2.0;
  gl_FragColor = color;
}
