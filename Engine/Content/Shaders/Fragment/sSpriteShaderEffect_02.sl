#include <Shaders/shaders.inc>

#if defined (EAE6320_PLATFORM_D3D)


cbuffer g_constantBuffer_perFrame : register( b0 )
{
	float g_elapsedSecondCount_systemTime;
	float g_elapsedSecondCount_simulationTime;
	// For float4 alignment
	float2 g_padding;
}

cbuffer g_constantBuffer_perMaterial : register( b1 )
{
	float4 g_color;
}

// Entry Point
//============

void main(

	// Output
	//=======

	out float4 o_color : SV_TARGET

	)
{
	// Output solid white
	o_color = float4(
		// RGB
		1.0 * sin(g_elapsedSecondCount_simulationTime),
		1.0 * sin(g_elapsedSecondCount_simulationTime),
		1.0,
		// Alpha
		1.0 );
}


#elif defined (EAE6320_PLATFORM_GL)
layout( std140, binding = 0 ) uniform g_constantBuffer_perFrame
{
	float g_elapsedSecondCount_systemTime;
	float g_elapsedSecondCount_simulationTime;
	// For vec4 alignment
	vec2 g_padding;
};

layout( std140, binding = 1 ) uniform g_constantBuffer_perMaterial
{
	vec4 g_color;
};

// Output
//=======

// Whatever color value is output from the fragment shader
// will determine the color of the corresponding pixel on the screen
out vec4 o_color;

// Entry Point
//============

void main()
{
	// Output solid white
	o_color = vec4(
		// RGB
		1.0 * sin(g_elapsedSecondCount_simulationTime),
		1.0 * cos(g_elapsedSecondCount_simulationTime),  
		1.0,
		// Alpha
		1.0 );
}
#endif
