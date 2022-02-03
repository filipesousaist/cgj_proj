#version 430

uniform mat4 m_pvm;
uniform mat4 m_viewModel;
uniform mat4 m_Model;   //por causa do cubo para a skybox
uniform mat3 m_normal;

uniform bool reflect;

in vec4 position;
in vec4 normal;    //por causa do gerador de geometria
in vec4 texCoord;
in vec4 tangent;
out vec4 fragTan;
out vec3 skyboxTexCoord;

out Data {
	vec3 pos;
	vec3 normal;
	vec3 eye;
	vec2 texCoord;
} DataOut;

void main () {
	DataOut.pos = vec3(m_viewModel * position);

	DataOut.normal = normalize(m_normal * normal.xyz);
	DataOut.eye = -DataOut.pos;
	DataOut.texCoord = texCoord.st;
	skyboxTexCoord = vec3(m_Model * position);	//Transformação de modelação do cubo unitário 
	skyboxTexCoord.x = - skyboxTexCoord.x; //Texturas mapeadas no interior logo negar a coordenada x

	fragTan = tangent;
	gl_Position = m_pvm * position;
	if (reflect) 
		gl_Position = vec4(-gl_Position.x, gl_Position.y, gl_Position.z, gl_Position.w);
}
