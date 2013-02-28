attribute vec4 aPosition;
attribute vec4 aNormal;

uniform mat4 MVPMatrixUniform;

//varying vec3 vInterpolatedNormal;
varying vec3 vInterpolatedViewVector;

invariant gl_Position;

mat3 GetLinearPart( mat4 m )
{
	mat3 result;
	
	result[0][0] = m[0][0]; 
	result[0][1] = m[0][1]; 
	result[0][2] = m[0][2]; 

	result[1][0] = m[1][0]; 
	result[1][1] = m[1][1]; 
	result[1][2] = m[1][2]; 
	
	result[2][0] = m[2][0]; 
	result[2][1] = m[2][1]; 
	result[2][2] = m[2][2]; 
	
	return result;
}


void main() {
		
	//vec4 result = eye *MVPMatrixUniform;
	//vInterpolatedViewVector = normalize(vec3(aPosition) -vec3(result));
	
	vec4 eye = vec4(0,0,1,1);
	
	mat3 ModelWorld3x3 = GetLinearPart( MVPMatrixUniform );
	
	vec4 WorldPos = aPosition * MVPMatrixUniform;
	
	vec3 N = normalize(aNormal.xyz * ModelWorld3x3 ); 
	
	vec3 E = normalize( WorldPos.xyz - eye.xyz );	
	
	vInterpolatedViewVector = reflect(E,N);		
	
	//vInterpolatedNormal =   normalize(vec3(aNormal));
	
	gl_Position = aPosition * MVPMatrixUniform;
}