#version 400

layout (location = 0) in vec3 vPosition;

uniform vec3 CameraPos;
uniform float Size;
uniform float Rot;
uniform vec3 Offset;
uniform vec4 Color;
uniform mat4 MVP;
uniform int Frame;
uniform int Rows;
uniform int Cols;

out vec4 color;
out vec2 uv;

void main()
{
  color = Color;
  uv = vPosition.xy;
  uv.x = (uv.x + Cols)/ 8;
  uv.y = (uv.y + Rows)/ 8;
  
  vec3 z = normalize(CameraPos - Offset);
  vec3 x = normalize(cross(vec3(0,1,0), z));
  vec3 y = normalize(cross(z, x));
  mat3 R = mat3(x, y, z);

  x = vec3(cos(Rot), -sin(Rot), 0);
  y = vec3(sin(Rot), cos(Rot), 0);
  z = vec3(0,0,1);
  mat3 M = mat3(x, y, z);

  vec3 eyePos = M * R * Size * (vPosition - vec3(0.5, 0.5, 0.0)) + Offset;
  gl_Position = MVP * vec4(eyePos, 1.0); 
}
