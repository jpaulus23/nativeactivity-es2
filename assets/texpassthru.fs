precision mediump float;

uniform sampler2D sTexture;

varying vec2 vTexCoords;

void main() {	


	//gl_FragColor = vec4(1,1,1,1);   //+tex;
	
	gl_FragColor = texture2D(sTexture,vTexCoords);
	
}