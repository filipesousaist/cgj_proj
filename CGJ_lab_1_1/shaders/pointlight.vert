#version 430

uniform mat4 m_pvm;
uniform mat4 m_viewModel;
uniform mat4 m_model;
uniform mat3 m_normal;

uniform vec4 l_pos;

uniform vec4 pl_pos[NUM_POINT_LIGHTS];

uniform vec4 sl_pos[NUM_SPOT_LIGHTS];

uniform vec4 coneDir[NUM_SPOT_LIGHTS];

in vec4 position;
in vec4 normal;    //por causa do gerador de geometria
in vec4 texCoord;

out Data {
	vec3 pos;
	vec3 normal;
	vec3 eye;
	vec3 directLightDir;
	vec3 pointLightDir[NUM_POINT_LIGHTS];
	vec3 spotLightDir[NUM_SPOT_LIGHTS];
	vec4 coneDir[NUM_SPOT_LIGHTS];
	vec2 tex_coord;

} DataOut;

void main () {

	DataOut.pos = vec3(m_viewModel * position);

	DataOut.normal = normalize(m_normal * normal.xyz);
	DataOut.directLightDir = vec3(l_pos - pos);
	for (int i = 0; i < NUM_POINT_LIGHTS; i ++)
		DataOut.pointLightDir[i] = vec3(pl_pos[i] - pos);
	for (int i = 0; i < NUM_SPOT_LIGHTS; i ++){
		DataOut.spotLightDir[i] = vec3(sl_pos[i] - pos);
		DataOut.coneDir[i] = coneDir[i];
	}
	DataOut.eye = vec3(-pos);
	DataOut.tex_coord = texCoord.st;

	gl_Position = m_pvm * position;
}
