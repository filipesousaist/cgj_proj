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

uniform vec3 directionalLightPos;
uniform vec3 pointLightPos[NUM_POINT_LIGHTS];
uniform vec3 spotLightPos[NUM_SPOT_LIGHTS];
uniform vec3 spotLightEndPos[NUM_SPOT_LIGHTS];

in Data {
	vec3 pos;
	vec3 normal;
	vec3 eye;
	vec2 texCoord;
} DataIn;

out vec4 colorOut;

void main() {

	vec4 texel, texel1; 
	
	vec4 totalSpecular = vec4(0.0);
	vec4 totalDiffuse = vec4(0.0);

	vec3 n = normalize(DataIn.normal);
	vec3 e = normalize(DataIn.eye);

	// Directional light
	if (day){
		vec3 dl = normalize(directionalLightPos - DataIn.pos);
		float diffuse = max(dot(n,dl), 0.0);
	
		if (diffuse > 0.0) {
			totalDiffuse += diffuse * vec4(0.5, 0.5, 0.5, 1.0);
			vec3 h = normalize(dl + e);
			float intSpecular = max(dot(h,n), 0.0);
			totalSpecular += pow(intSpecular, mat.shininess);
		}
	}

	// Point lights
	if (candles) {
		for (int i = 0; i < NUM_POINT_LIGHTS; i ++) {
			vec3 l = normalize(pointLightPos[i] - DataIn.pos);
			
			float diffuse = max(dot(n,l), 0.0);
		
			if (diffuse > 0.0) {
				totalDiffuse += diffuse;
				vec3 h = normalize(l + e);
				float intSpecular = max(dot(h,n), 0.0);
				totalSpecular += pow(intSpecular, mat.shininess);
			}
		}
	}
	
	// Spotlights
	if (headlights) {
		float SPOT_INTENSITY = 10;
		float SPOT_LINEAR_ATT = 0;
		float SPOT_QUADRATIC_ATT = 0.01;

		float SPOT_COS_CUTOFF = cos(radians(5));

		for (int i = 0; i < NUM_SPOT_LIGHTS; i++) {
			vec3 posToSpotLight = spotLightPos[i] - DataIn.pos;
			float dist = length(posToSpotLight);
			vec3 sl = normalize(posToSpotLight);
			vec3 coneDir = normalize(spotLightPos[i] - spotLightEndPos[i]);

			float spotCos = dot(sl, coneDir);

			if (spotCos > SPOT_COS_CUTOFF) { //inside cone?
				float spot_intensity = SPOT_INTENSITY / (1 + SPOT_LINEAR_ATT * dist + SPOT_QUADRATIC_ATT * dist * dist);
				float diffuse = max(dot(n, sl), 0.0) * spot_intensity;

				if (diffuse > 0.0) {
					totalDiffuse += diffuse;
					vec3 h = normalize(sl + e);
					float intSpecular = max(dot(h,n), 0.0);
					totalSpecular += pow(intSpecular, mat.shininess) * spot_intensity;
				}
			}
		}
	}
	
	vec4 finalDiffuse = mat.diffuse * totalDiffuse;
	vec4 finalSpecular = mat.specular * totalSpecular;

	if (mat.texCount == 0) // plain color
		colorOut = max(finalDiffuse + finalSpecular, mat.ambient);
	else if (mat.texCount == 1) 
	{
		texel = texture(texmap, DataIn.texCoord);
		if (mergeTextureWithColor) // mix 
			colorOut = max(finalDiffuse * texel + finalSpecular, mat.ambient * texel);
		else // diffuse color is replaced by texel color, with specular area or ambient (0.07 * texel)
			colorOut = max(totalDiffuse * texel + finalSpecular, 0.07 * texel);
	}
	else // multitexturing
	{
		texel = texture(texmap, DataIn.texCoord);
		texel1 = texture(texmap1, DataIn.texCoord);
		colorOut = max(totalDiffuse * texel * texel1 + finalSpecular, 0.07 * texel * texel1);
	}

	if (fog) {
		float dist = length(DataIn.pos);
		float fogAmount = exp(-dist * dist * 0.005);
		vec3 fogColor = vec3(0.5, 0.6, 0.7);
		vec3 final_color = mix(fogColor, colorOut.rgb, fogAmount);
		colorOut = vec4(final_color, colorOut.a);
	}

}