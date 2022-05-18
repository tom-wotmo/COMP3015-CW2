#version 460

in vec3 Position;
in vec3 Normal;

layout(location=0) out vec4 FragColor;
layout(binding=0) uniform sampler2D RenderTex;

uniform float EdgeThreshold;
uniform int Pass;

uniform struct MaterialInfo
{
vec3 Ka;
vec3 Kd;
vec3 Ks;
float Shininess;

}Material;

uniform struct LightInfo
{
vec4 Position;
vec3 La;
vec3 L;

}Light;

const vec3 lum = vec3(1,1,1);

vec3 blinnPhong(vec3 position, vec3 n)
{
vec3 ambient = Light.La;
vec3 s = normalize(Light.L -position);

float sDotN = max(dot(s,n), 0.0);
vec3 diffuse = Material.Kd * sDotN;

vec3 spec = vec3(0.0);
if(sDotN > 0.0)
   {
   vec3 v = normalize(-position.xyz);
   vec3 h = normalize(v + s);
   spec = Material.Ks * pow(max(dot(h,n), 0.0),Material.Shininess);

   }

return ambient + Light.L * (diffuse * spec);
}
float luminance( vec3 color)
{
return dot(lum, color);

}



void main() 
{

}