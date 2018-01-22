#version 330

void main(){
	
	//Only need to record the depth, this is inefficient but leave for now
	//maybe add a stencil buffer indicator in the alpha channel
	gl_FragData[0] = vec4(gl_FragCoord.z, gl_FragCoord.z, gl_FragCoord.z, gl_FragCoord.z);
}