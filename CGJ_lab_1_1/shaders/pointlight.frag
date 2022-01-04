#version 430

#define NUM_POINT_LIGHTS 6

uniform sampler2D texmap;
uniform sampler2D texmap1;

uniform bool mergeTextureWithColor;

struct Materials {
	vec4 diffuse;
	vec4 ambient;
	vec4 specular;
	vec4 emissive;
	float shininess;
	int texCount;
};

uniform Materials mat;

in Data {
	vec3 normal;
	vec3 eye;
	vec3 lightDir[NUM_POINT_LIGHTS];
	vec2 tex_coord;
} DataIn;

in vec4 pos;

out vec4 colorOut;

void main() {

	vec4 texel, texel1; 
	
	vec4 totalSpecular = vec4(0.0);
	vec4 totalDiffuse = vec4(0.0);

	vec3 n = normalize(DataIn.normal);
	vec3 e = normalize(DataIn.eye);
	vec3 l[NUM_POINT_LIGHTS];

	for (int i = 0; i < NUM_POINT_LIGHTS; i ++) {
		l[i] = normalize(DataIn.lightDir[i]);

		float diffuse = max(dot(n,l[i]), 0.0);
	
		if (diffuse > 0.0) {
			totalDiffuse += diffuse;
			vec3 h = normalize(l[i] + e);
			float intSpecular = max(dot(h,n), 0.0);
			totalSpecular += pow(intSpecular, mat.shininess);
		}
	}

	vec4 finalDiffuse = mat.diffuse * totalDiffuse;
	vec4 finalSpecular = mat.specular * totalSpecular;
	
	if (mat.texCount == 0)
	{
		colorOut = max(finalDiffuse + finalSpecular, mat.ambient);
	}
	else if (mat.texCount == 1) // diffuse color is replaced by texel color, with specular area or ambient (0.1*texel)
	{
		texel = texture(texmap, DataIn.tex_coord);
		if (mergeTextureWithColor) // mix 
			colorOut = max(finalDiffuse * texel + finalSpecular, mat.ambient * texel);
		else
			colorOut = max(totalDiffuse * texel + finalSpecular, 0.07 * texel);
	}
	else // multitexturing
	{
		texel = texture(texmap, DataIn.tex_coord);
		texel1 = texture(texmap1, DataIn.tex_coord);
		colorOut = max(totalDiffuse * texel * texel1 + finalSpecular, 0.07 * texel * texel1);
	}

	float dist = length(pos.xyz);
	float fogAmount = exp(-dist * 0.0/*2*/ );
	vec3 fogColor = vec3(0.5, 0.6, 0.7);
	vec3 final_color = mix(fogColor, colorOut.rgb, fogAmount);
	colorOut = vec4(final_color, colorOut.a);
}