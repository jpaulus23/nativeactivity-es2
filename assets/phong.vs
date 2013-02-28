attribute vec4 aPosition;
attribute vec4 aNormal;
attribute vec2 aTexCoords;

uniform mat4 MVPMatrixUniform;

uniform vec4 EyePosUniform;
uniform vec4 LightPosUniform;

varying vec3 vInterpolatedNormals;
varying vec2 vTexCoords;

invariant gl_Position;

void main() {
	
	//mat4 NormalMatrix = inverse of transpose of upper 3x3 of MVPMatrix
	
	
	vec3 fvTangent      = -1.0 * vec3(abs(aNormal.y) + abs(aNormal.z), abs(aNormal.x), 0); 
	vec3 fvBinormal     = cross(fvTangent,vec3(aNormal)); 
	
	mat3 NormalMatrix = mat3(fvTangent,fvBinormal,vec3(aNormal));
	//							fvTangent.y,fvBinormal.y,vNormal.y,
	//							fvTangent.z,fvBinormal.z,vNormal.z);
								
	vec3 fixedNormal = vec3(aNormal) * NormalMatrix;
	
	vInterpolatedNormals =   normalize(vec3(aNormal));
	
	vTexCoords = aTexCoords;
	
	gl_Position = aPosition * MVPMatrixUniform;
	
}