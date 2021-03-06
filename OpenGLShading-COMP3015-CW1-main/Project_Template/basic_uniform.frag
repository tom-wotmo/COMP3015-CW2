#version 460

layout (location = 0) out vec4 FragColor;
layout (binding = 0) uniform sampler2D Tex1;
layout (binding =1) uniform sampler2D NormalMapTex;

in vec3 position;
in vec3 normal;
in vec2 TexCoord;
in vec3 Vec;

//Light structure
uniform struct LightInfo 
{
vec4 Position;		
vec3 La; //ambient light			
vec3 L;	//light diffuse and spec			
} Lights;

//material information struct
uniform struct MaterialInfo 
{
vec3 Ka; //material ambient				
vec3 Kd; //material diffuse		
vec3 Ks; //material specular			
float Shininess; //material shininess	
} Material;
//fog structure
uniform struct FogInfo
{
float MaxDist;
float MinDist;
vec3 Colour;
}Fog;
//spotlight structure
uniform struct SpotLightInfo
{
vec3 Position;
vec3 L;
vec3 La;
vec3 Direction;
float Exponent;
float Cutoff;
}Spot;

uniform int shaderInt;
uniform bool isFoggy;
//blinnphong model
 vec3 blinnPhongModel(vec3 pos, vec3 n)
{
     
	 //getting the colour of texture
	 vec3 texColour = texture(Tex1, TexCoord).rgb;

	 //ambient calculation
	 vec3 ambient = texColour * Lights.La; 

	 vec3 s = vec3 (0.0);
	 s = normalize(vec3(Lights.Position) - pos);
	 
	 //dot product
	 float sDotN = max( dot(s,n), 0.0 );

	 vec3 diffuse = texColour * sDotN;

	 vec3 spec = vec3(0.0);

	 vec3 v = normalize(-position.xyz);

	if( sDotN > 0.0 )
	{

	 vec3 h = normalize(v + s);
	 //calculate specular
	 spec = Material.Ks * pow( max( dot(h,normal), 0.0 ), Material.Shininess);

	}

	 return ambient + Lights.L * (diffuse + spec);
 }
 //spotlight blinn model
 vec3 blinnPhongSpotModel(vec3 pos, vec3 n)
 {
    vec3 texColour = texture(Tex1, TexCoord).rgb;


	vec3 ambient = Spot.La * Material.Ka * texColour;
	vec3 s = normalize(Spot.L - position);

	float cosAng = dot(-s, normalize(Spot.Direction));
	float angle = acos(cosAng);

	float spotScale = 0.0f;
	vec3 spec = vec3(0.0f);
	vec3 diffuse = vec3(0.0f);

	if(angle < Spot.Cutoff)
	{
	   spotScale = pow(cosAng, Spot.Exponent);
	   float sDotN = max(dot(s,n), 0.0);
	   diffuse = Spot.L * Material.Kd * sDotN;

	   if(sDotN > 0.0)
	   {
	      vec3 v = normalize(-position.xyz);
		  vec3 h = normalize(v + s);
		  spec = Material.Ks * pow(max(dot(h,n), 0.0), Material.Shininess);

	   }
	}
	return ambient + spotScale * Spot.L * (diffuse + spec);
 }
void main()
{
  
     if(isFoggy)
	 {

    //calculating the fog
    float dist = abs(position.z);

	float fogFactor = (Fog.MaxDist - dist) / (Fog.MaxDist - Fog.MinDist);

	fogFactor = clamp(fogFactor,0.0,1.0);
	
	if(shaderInt == 2)
	{
		vec3 phongColour = blinnPhongModel(position, normalize(normal));
		vec3 colour = mix(Fog.Colour, phongColour, fogFactor);
		FragColor = vec4(colour, 1.0);
	}
	if(shaderInt == 1)
	{
	    vec3 phongColour = blinnPhongSpotModel(position, normalize(normal));
		vec3 colour = mix(Fog.Colour, phongColour, fogFactor);
		FragColor = vec4(colour, 1.0);
	
	}
	}
	else if (!isFoggy)
	{

	if(shaderInt == 2)
	{
	  vec3 phongColour = blinnPhongModel(position, normalize(normal));
	  FragColor = vec4(phongColour, 1.0);
	}
	if(shaderInt == 1)
	{
	    vec3 phongColour = blinnPhongSpotModel(position, normalize(normal));
		FragColor = vec4(phongColour, 1.0);
	}

	}
	
}