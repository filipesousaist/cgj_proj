#version 430

#define NUM_POINT_LIGHTS 6

#define NUM_SPOT_LIGHTS 2

uniform sampler2D texmap;
uniform sampler2D texmap1;

uniform bool mergeTextureWithColor;

uniform bool day;

uniform bool candles;

uniform bool headlights;

uniform bool fog;

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
	vec3 directLightDir;
	vec3 pointLightDir[NUM_POINT_LIGHTS];
	vec3 spotLightDir[NUM_SPOT_LIGHTS];
	vec4 coneDir[NUM_SPOT_LIGHTS];
	vec2 tex_coord;

} DataIn;

in vec4 pos;

out vec4 colorOut;

void main() {

	vec4 texel, texel1; 
	
	vec4 totalSpecular = vec4(0.0);
	vec4 totalDiffuse = vec4(0.0);

	float spotCosCutOff = cos(radians(20));

	vec3 n = normalize(DataIn.normal);
	vec3 e = normalize(DataIn.eye);
	vec3 dl = normalize(DataIn.directLightDir);
	vec3 pl[NUM_POINT_LIGHTS];
	vec3 sl[NUM_SPOT_LIGHTS];

	if (day){
		float diffuse = max(dot(n,dl), 0.0);
	
		if (diffuse > 0.0) {
			totalDiffuse += diffuse * vec4(0.5, 0.5, 0.5, 1.0);
			vec3 h = normalize(dl + e);
			float intSpecular = max(dot(h,n), 0.0);
			totalSpecular += pow(intSpecular, mat.shininess);
		}
	}
	if (candles) {
		for (int i = 0; i < NUM_POINT_LIGHTS; i ++) {
			pl[i] = normalize(DataIn.pointLightDir[i]);

			float diffuse = max(dot(n,pl[i]), 0.0);
	
			if (diffuse > 0.0) {
				totalDiffuse += diffuse * vec4(0.5, 0.5, 0.5, 1.0);
				vec3 h = normalize(pl[i] + e);
				float intSpecular = max(dot(h,n), 0.0);
				totalSpecular += pow(intSpecular, mat.shininess);
			}
		}
	}

	if (headlights){
		float att = 0.0;
		float spotExp = 5.0;

		for (int i = 0; i < NUM_SPOT_LIGHTS; i++) {
			sl[i] = normalize(DataIn.spotLightDir[i]);

			vec3 sd = normalize(vec3(-DataIn.coneDir[i]));
			float spotCos = dot(sl[i], sd);

			if(spotCos > spotCosCutOff)  {	//inside cone?
				att = pow(spotCos, spotExp);
				float diffuse = max(dot(n,sl[i]), 0.0) * att;

				if (diffuse > 0.0) {
					totalDiffuse += diffuse;
					vec3 h = normalize(sl[i] + e);
					float intSpecular = max(dot(h,n), 0.0);
					totalSpecular += pow(intSpecular, mat.shininess) * att;
				}
			}
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

	if (fog) {
		float dist = length(pos.xyz);
		float fogAmount = exp(-dist * 0.2f);
		vec3 fogColor = vec3(0.5, 0.6, 0.7);
		vec3 final_color = mix(fogColor, colorOut.rgb, fogAmount);
		colorOut = vec4(final_color, colorOut.a);
	}

}