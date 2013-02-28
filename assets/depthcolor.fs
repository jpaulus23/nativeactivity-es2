precision mediump float;

void main() {
	gl_FragColor = vec4(gl_FragCoord.z *2.0, gl_FragCoord.z*2.0, gl_FragCoord.z*2.0, 1);
}