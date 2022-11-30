#version 400

in  vec2 ex_TexCoord; //texture coord arriving from the vertex
in  vec3 ex_Normal;  //normal arriving from the vertex

out vec4 out_Color;   //colour for the pixel
in vec3 ex_LightDir;  //light direction arriving from the vertex

in vec3 ex_PositionEye;

uniform vec4 light_ambient;
uniform vec4 light_diffuse;
uniform vec4 light_specular;

uniform vec4 Cone_Light_Specular;
uniform vec4 Cone_Light_Ambient_And_Diffuse;

uniform vec4 material_ambient;
uniform vec4 material_diffuse;
uniform vec4 material_specular;
uniform float material_shininess;

uniform vec3 lightPosition;
uniform vec3 lightDirection;
uniform float lightCutOff;
uniform float lightCutOffOuter;

in vec3 coneNormal;

uniform sampler2D DiffuseMap;

in vec3 fragPos;

uniform vec3 planePos;

void main(void)
{

	//Calculate lighting
	vec3 n, L;
	vec4 color;
	float NdotL;
	
	n = normalize(ex_Normal);
	L = normalize(ex_LightDir);

	vec3 v = normalize(-ex_PositionEye);
	vec3 r = normalize(-reflect(L, n));
	
	float RdotV = max(0.0, dot(r, v));

	NdotL = max(dot(n, L),0.0);

	color = light_ambient * material_ambient;
	
	color += (light_ambient * material_diffuse * NdotL);

	color += material_specular * light_specular * pow(RdotV, material_shininess);

	//out_Color = color;  //show just lighting

	//out_Color = texture(DiffuseMap, ex_TexCoord); //show texture only

    /////////////////////////////////////////////////////////////////////////////////

	// ambient
    vec4 ambient = light_ambient * material_ambient;
    
    vec3 norm = normalize(coneNormal);

    // diffuse
    vec3 lightDir = normalize(lightPosition - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec4 diffuse = Cone_Light_Ambient_And_Diffuse * diff * material_diffuse; 
    
    // specular
    vec3 viewDir = normalize(ex_PositionEye - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material_shininess);
    vec4 specular = Cone_Light_Specular * spec * material_specular;  
    
    // spotlight (soft edges)
    float theta = dot(lightDir, normalize(-lightDirection)); 
    float epsilon = (lightCutOff - lightCutOffOuter);
    float intensity = smoothstep(0.0, 1.0, (theta - lightCutOffOuter) / epsilon);
    ambient  *= intensity;
    diffuse  *= intensity;
    specular *= intensity;
    
    // attenuation
    float constant = 1.0f;
    float linear = 0.0009f;
    float quadratic = 0.000001;

    float distance    = length(lightPosition - fragPos);
    float attenuation = 1.0 / (constant + (linear * distance) + (quadratic * (distance * distance)));    
    ambient  *= attenuation*2; 
    diffuse  *= attenuation*2;
    specular *= attenuation*2;   
       
    vec4 result = ambient + diffuse + specular;

    color+=result;
    vec2 a = vec2(planePos.x,planePos.z);
    vec2 b = vec2(fragPos.x,fragPos.z);
    float l = length(a-b);
    if (l<15){
        color *= (l/15);
    }

    out_Color = color * texture(DiffuseMap, ex_TexCoord); //show texture and lighting
}