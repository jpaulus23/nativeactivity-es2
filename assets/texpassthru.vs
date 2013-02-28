attribute vec4 aPosition;
attribute vec2 aTexCoords;

uniform mat4 MVPMatrixUniform;

varying vec2 vTexCoords;

invariant gl_Position;

void main() {	

	vTexCoords = aTexCoords;
	
	gl_Position = aPosition * MVPMatrixUniform;
	
}