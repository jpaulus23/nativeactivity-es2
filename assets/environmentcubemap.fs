precision mediump float;

uniform samplerCube sCubeTexture;

//varying vec3 vInterpolatedNormal;
varying vec3 vInterpolatedViewVector;

void main() {

	//vec4 reflected = reflect(vec4(vInterpolatedViewVector,1), vec4(vInterpolatedNormal,1));
	
	gl_FragColor =  textureCube(sCubeTexture, vInterpolatedViewVector.xyz);
}