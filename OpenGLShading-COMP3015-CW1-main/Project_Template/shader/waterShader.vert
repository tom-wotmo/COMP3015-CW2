#version 460

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;

out vec3 Normal;
out vec3 Position;

uniform float Time;
uniform bool isAnimated;

uniform float Freq = 2;
uniform float Velocity = 3;
uniform float Amp = 0.5;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 MVP;

void main()
{
   if(isAnimated)
   {
	vec4 pos = vec4(VertexPosition, 1.0);

	float u = Freq * pos.x - Velocity * Time;
	float i = Freq * pos.z - Velocity * Time;

	pos.y = Amp * sin(u) + Amp * sin(i);
	pos.y *= 0.25f;

	vec3 n = vec3(0.0);
	n.xyz = normalize(vec3(cos(u), 1.0f, cos(i)));


	Normal = normalize(NormalMatrix * n);
	Position = vec3(ModelViewMatrix * pos);

	gl_Position = MVP * pos;
	}
	else
	{
	    Position = (ModelViewMatrix * vec4(VertexPosition, 1.0)).xyz;
        Normal = normalize(NormalMatrix * VertexNormal);
        gl_Position = MVP * vec4(VertexPosition, 1.0);
	}
}