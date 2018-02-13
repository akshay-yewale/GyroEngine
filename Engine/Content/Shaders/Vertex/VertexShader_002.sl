#include <Shaders/shaders.inc>
/*
* Platform Independent Code
*/

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

DeclareConstantBuffer(g_constantBuffer_perDrawCall, 2)
{
	// This is a placeholder to prevent an empty constant buffer declaration
	float4 DUMMY;
};

/*
* Platform Specific Code
*/

#if defined (EAE6320_PLATFORM_D3D)

	void main( in const float2 i_position : POSITION, in float2 i_uv : TEXCOORD0, out float4 out_position : SV_POSITION , out float2 o_uv : TEXCOORD0 )
#elif defined (EAE6320_PLATFORM_GL)

	layout( location = 0 ) in float2 i_position;
	layout( location = 1 ) in float2 i_uv;
	layout( location = 0 ) out float2 o_uv;
	void main()
#endif

{
	float4 temp_position;
	temp_position = float4(i_position.x - 1.0f , i_position.y , 0.0, 1.0);
	o_uv = i_uv;
	out_position = temp_position;

}
