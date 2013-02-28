attribute vec4 aPosition;

uniform mat4 MVPMatrixUniform;

invariant gl_Position;

void main() {
	
	gl_Position = aPosition * MVPMatrixUniform;
	
}