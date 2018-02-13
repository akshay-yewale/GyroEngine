#include <Shaders/shaders.inc>


DeclareConstantBuffer(g_constantBuffer_perFrame, 0 )
{
	float g_elapsedSecondCount_systemTime;
	float g_elapsedSecondCount_simulationTime;
	// For float4 alignment
	float2 g_padding;
};


DeclareConstantBuffer(g_constantBuffer_perMaterial, 1)
{
	float4 g_color;
};

DeclareTexture(g_color_texture, 0);

DeclareSamplerState(g_color_samplerState, 0);

#if defined (EAE6320_PLATFORM_D3D)

void main(

	//Input
	//=====
	
	in float4 o_position : SV_POSITION ,
	in float2 i_uv : TEXCOORD0 ,

	// Output
	//=======

	out float4 o_color : SV_TARGET

	)

#elif defined (EAE6320_PLATFORM_GL)

layout( location = 0) in vec2 i_uv;

// Output
//=======

// Whatever color value is output from the fragment shader
// will determine the color of the corresponding pixel on the screen
out vec4 o_color;

void main()
#endif
{
	float4 sampledColor = Texture2D(g_color_texture, i_uv, g_color_samplerState);
	float4 randomColor = vec4(1.0 * sin(g_elapsedSecondCount_simulationTime),
		1.0 * cos(g_elapsedSecondCount_simulationTime),  
		1.0,
		1.0 );
	o_color = sampledColor * randomColor ;

}

	