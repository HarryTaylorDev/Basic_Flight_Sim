#version 400

uniform mat4 ModelMatrix;
uniform mat4 ProjectionMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ViewMatrix;

in  vec3 in_Position;  // Position coming in
in  vec2 in_TexCoord;  // texture coordinate coming in
in  vec3 in_Normal;    // vertex normal used for lighting

uniform vec4 LightPos;  // light position

out vec2 ex_TexCoord;  // exiting texture coord
out vec3 ex_Normal;    // exiting normal transformed by the normal matrix
out vec3 ex_PositionEye; 
out vec3 ex_LightDir; 

out vec3 coneNormal;
	
out vec3 fragPos;

void main(void)
{
	fragPos = vec3(ModelMatrix * vec4(in_Position, 1.0));

	gl_Position = ProjectionMatrix * ViewMatrix * vec4(fragPos, 1.0);
	
	ex_TexCoord = in_TexCoord;
		
	ex_Normal = NormalMatrix * in_Normal; 

	coneNormal = mat3(transpose(inverse(ModelMatrix))) * in_Normal;

	ex_PositionEye = vec3((ViewMatrix * ModelMatrix * vec4(in_Position, 1.0)));

	ex_LightDir = vec3(ViewMatrix * LightPos);


}