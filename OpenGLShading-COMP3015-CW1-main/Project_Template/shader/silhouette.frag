#version 460

uniform vec4 LineColour;

in vec3 GPosition;
in vec3 GNormal;

flat in int GIsEdge;

layout(location = 0) out vec4 FragColour;

const int levels = 3;
const float scaleFactor = 1.0/levels;
//Light structure
uniform struct LightInfo 
{
vec4 Position;		
vec3 Intensity;

} Light;

//material information struct
uniform struct MaterialInfo 
{
vec3 Ka; //material ambient				
vec3 Kd; //material diffuse				
} Material;

vec3 toonShade()
{
vec3 s = normalize(Light.Position.xyz - GPosition.xyz);
vec3 ambient = Material.Ka;
float cosine = dot(s,GNormal);
vec3 diffuse = Material.Kd * ceil(cosine * levels) * scaleFactor;

return Light.Intensity * (ambient + diffuse);

}
void main()
{
if(GIsEdge == 1)
{FragColour = LineColour;}
else{FragColour = vec4(toonShade(), 1.0);
}
}