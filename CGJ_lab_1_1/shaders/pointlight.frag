#version 430

#define NUM_POINT_LIGHTS 6

#define NUM_SPOT_LIGHTS 2

uniform sampler2D texmap;
uniform sampler2D texmap1;

uniform bool mergeTextureWithColor;
uniform bool isHUD;

uniform bool day;
uniform bool candles;
uniform bool headlights;
uniform bool fog;

uniform bool shadowMode;

// bumpmap
uniform mat3 m_normal;
uniform int texMode;
in vec4 fragTan;
uniform sampler2D normalMap;

uniform samplerCube skyBoxMap;
in vec3 skyboxTexCoord;
uniform int reflect_perFrag; //reflect vector calculated in the frag shader
uniform mat4 m_View;


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

const float reflect_factor = 0.9;

void main() {
	if (isHUD) {
		if (mat.texCount == 0)
			colorOut = mat.diffuse;
		else
			colorOut = texture(texmap, DataIn.texCoord);
		if (colorOut.a == 0)
			discard;
		return;
	}

	vec4 texel, texel1, cube_texel; 

	vec3 t, b, aux, eyeDir, new_n;
	
	vec4 totalSpecular = vec4(0.0);
	vec4 totalDiffuse = vec4(0.0);

	vec3 n = normalize(DataIn.normal);
	vec3 e = normalize(DataIn.eye);
	eyeDir = e;
	new_n = n;

	
	// Directional light
	if (day){
		vec3 dl = normalize(directionalLightPos - DataIn.pos);

		if(texMode == 2)  {
			//Calculate components of TBN basis in eye space
			t = normalize(m_normal * fragTan.xyz);  
			b = fragTan.w * cross(n,t);

			aux.x = dot(dl, t);
			aux.y = dot(dl, b);
			aux.z = dot(dl, n);
			dl = normalize(aux);

			aux.x = dot(eyeDir, t);
			aux.y = dot(eyeDir, b);
			aux.z = dot(eyeDir, n);
			eyeDir = normalize(aux);

			new_n = normalize(2.0 * texture(normalMap, DataIn.texCoord).rgb - 1.0);
		}

		float diffuse = max(dot(new_n, dl), 0.0);
	
		if (diffuse > 0.0) {
			totalDiffuse += diffuse;
			vec3 h = normalize(dl + eyeDir);
			float intSpecular = max(dot(h,new_n), 0.0);
			totalSpecular += pow(intSpecular, mat.shininess);
		}
	}

	// Point lights
	if (candles) {
		for (int i = 0; i < NUM_POINT_LIGHTS; i ++) {
			float POINT_INTENSITY = 5;
			float POINT_LINEAR_ATT = 0;
			float POINT_QUADRATIC_ATT = 0.01;

			vec3 posToPointLight = pointLightPos[i] - DataIn.pos;
			float dist = length(posToPointLight);
			vec3 l = normalize(posToPointLight);

			if(texMode == 2)  {
				//Calculate components of TBN basis in eye space
				t = normalize(m_normal * fragTan.xyz);  
				b = fragTan.w * cross(n,t);

				aux.x = dot(l, t);
				aux.y = dot(l, b);
				aux.z = dot(l, n);
				l = normalize(aux);

				aux.x = dot(eyeDir, t);
				aux.y = dot(eyeDir, b);
				aux.z = dot(eyeDir, n);
				eyeDir = normalize(aux);

				new_n = normalize(2.0 * texture(normalMap, DataIn.texCoord).rgb - 1.0);
			}
			
			float point_intensity = POINT_INTENSITY / (1 + POINT_LINEAR_ATT * dist + POINT_QUADRATIC_ATT * dist * dist);
			float diffuse = max(dot(new_n,l), 0.0) * point_intensity;
		
			if (diffuse > 0.0) {
				totalDiffuse += diffuse; //* vec4(0.5, 0.5, 0.5, 1.0);
				vec3 h = normalize(l + eyeDir);
				float intSpecular = max(dot(h,new_n), 0.0);
				totalSpecular += pow(intSpecular, mat.shininess) * point_intensity;
			}
		}
	}
	
	float totalSpotIntensity = 0;
	// Spotlights
	if (headlights) {
		float SPOT_INTENSITY = 10;
		float SPOT_LINEAR_ATT = 0;
		float SPOT_QUADRATIC_ATT = 0.01;

		float SPOT_COS_CUTOFF = cos(radians(20));

		for (int i = 0; i < NUM_SPOT_LIGHTS; i++) {
			vec3 posToSpotLight = spotLightPos[i] - DataIn.pos;
			float dist = length(posToSpotLight);
			vec3 sl = normalize(posToSpotLight);
			vec3 coneDir = normalize(spotLightPos[i] - spotLightEndPos[i]);
			if(texMode == 2)  {
				//Calculate components of TBN basis in eye space
				t = normalize(m_normal * fragTan.xyz);  
				b = fragTan.w * cross(n,t);

				aux.x = dot(sl, t);
				aux.y = dot(sl, b);
				aux.z = dot(sl, n);
				sl = normalize(aux);
				
				aux.x = dot(coneDir, t);
				aux.y = dot(coneDir, b);
				aux.z = dot(coneDir, n);
				coneDir = normalize(aux);

				aux.x = dot(eyeDir, t);
				aux.y = dot(eyeDir, b);
				aux.z = dot(eyeDir, n);
				eyeDir = normalize(aux);

				new_n = normalize(2.0 * texture(normalMap, DataIn.texCoord).rgb - 1.0);
			}
			
			float spotCos = dot(sl, coneDir);

			if (spotCos > SPOT_COS_CUTOFF) { //inside cone?
				float spot_intensity = SPOT_INTENSITY / (1 + SPOT_LINEAR_ATT * dist + SPOT_QUADRATIC_ATT * dist * dist);
				float diffuse = max(dot(new_n, sl), 0.0) * spot_intensity;

				if (diffuse > 0.0) {
					totalDiffuse += diffuse;
					totalSpotIntensity += diffuse;
					vec3 h = normalize(sl + eyeDir);
					float intSpecular = max(dot(h,new_n), 0.0);
					totalSpecular += pow(intSpecular, mat.shininess) * spot_intensity;
				}
			}
		}
	}
	
	vec4 finalDiffuse = mat.diffuse * totalDiffuse;
	vec4 finalSpecular = mat.specular * totalSpecular;

	if (mat.texCount == 0) // plain color
		if (texMode == 3) //SkyBox
		{
			colorOut = texture(skyBoxMap, skyboxTexCoord);
		}else{
			colorOut = max(finalDiffuse + finalSpecular, mat.ambient);
		}
	else if (mat.texCount == 1) 
	{
		if(reflect_perFrag == 1) {  //reflected vector calculated here
			vec3 reflected1 = vec3 (transpose(m_View) * vec4 (vec3(reflect(-e, n)), 0.0)); //reflection vector in world coord
			reflected1.x= -reflected1.x;   
			cube_texel = texture(skyBoxMap, reflected1);

			texel = texture(texmap, DataIn.texCoord); 
			vec4 aux_color = mix(texel, cube_texel, reflect_factor);
			aux_color = max(finalDiffuse*aux_color + finalSpecular, 0.1*aux_color);
			colorOut = vec4(aux_color.rgb, 1.0); 
		}else{
			texel = texture(texmap, DataIn.texCoord);
			if (texel.a <= 0.1) discard;
			else if (mergeTextureWithColor) // mix 
				colorOut = max(finalDiffuse * texel + finalSpecular, mat.ambient * texel);
			else // diffuse color is replaced by texel color, with specular area or ambient (0.07 * texel)
				colorOut = max(totalDiffuse * texel + finalSpecular, 0.07 * texel);
		}
		
	}
	else if (mat.texCount == -1) // modulated texture for particle
	{
		texel = texture(texmap, DataIn.texCoord);  //texel from element flare texture
		if((texel.a == 0.0)  || (mat.diffuse.a == 0.0) ) discard;
		else{
			colorOut = mat.diffuse * texel;
			return;
		}
	}
	else // multitexturing
	{
		texel = texture(texmap, DataIn.texCoord);
		texel1 = texture(texmap1, DataIn.texCoord);
		colorOut = max(totalDiffuse * texel * texel1 + finalSpecular, 0.07 * texel * texel1);
	}

	if (fog) {
		float dist = length(DataIn.pos);
		float fogAmount = exp(-dist * dist * 0.005 / (1 + 5 * totalSpotIntensity));
		vec3 fogColor = vec3(0.5, 0.6, 0.7);
		vec3 final_color = mix(fogColor, colorOut.rgb, fogAmount);
		colorOut = vec4(final_color, colorOut.a);
	}
	if(shadowMode) //constant color
		colorOut *= vec4(0.1, 0.1, 0.1, 1.0);

	colorOut = vec4(colorOut.rgb, mat.diffuse.a);

}