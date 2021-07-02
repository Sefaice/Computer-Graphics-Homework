#version 330 core

void main()
{            
	// Since we have no color buffer the resulting fragments do not require any processing, so we can simply use an empty fragment shader
	// the code is just what the pipeline does behind the scene
//	gl_FragDepth = gl_FragCoord.z;
//	gl_FragDepth += gl_FrontFacing ? 0.05 : 0;
}