#version 430

uniform mat4 m_pvm;
uniform mat4 m_viewModel;
uniform mat4 m_model;
uniform mat3 m_normal;

in vec4 position;
in vec4 normal;    //por causa do gerador de geometria
in vec4 texCoord;

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

	gl_Position = m_pvm * position;
}
