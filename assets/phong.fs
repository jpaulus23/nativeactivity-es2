precision mediump float;

uniform sampler2D sTexture;
uniform vec4 EyePosUniform;
uniform vec4 LightPosUniform;

varying vec2 vTexCoords;
varying vec3 vInterpolatedNormals;

void main() {
		
	float SpecularIntensity = 1.0;
	float DiffuseIntensity = 0.2;

	vec3 HVector = vec3(EyePosUniform + LightPosUniform);
	HVector = normalize(HVector);
	
	float Angle = max(dot(vInterpolatedNormals,vec3(HVector)),0.0);

	vec3 buffer =  vec3(texture2D(sTexture,vTexCoords))
				   *
				   DiffuseIntensity
				   *
				   dot(vInterpolatedNormals,vec3(normalize(LightPosUniform)))
				   +
				   SpecularIntensity * pow(Angle,4.0)
				   *vec3(texture2D(sTexture,vTexCoords));
				   
	gl_FragColor =   0.05* vec4(vec3(texture2D(sTexture,vTexCoords)),0) + vec4(buffer,1);
}