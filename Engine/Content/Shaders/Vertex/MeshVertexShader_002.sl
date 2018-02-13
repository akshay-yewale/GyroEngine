#include <Shaders/shaders.inc>
/*
* Platform Independent Code
*/

DeclareConstantBuffer(g_constantBuffer_perFrame, 0 )
{
	float4x4 g_transform_worldToCamera;
	float4x4 g_transform_cameraToProjected;
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
	//float4 g_position;
	float4x4 g_transform_localToWorld;
};

/*
* Platform Specific Code
*/

#if defined (EAE6320_PLATFORM_D3D)

	void main( 
in const float3 i_position : POSITION , 
in float4 i_color : COLOR0, 
in float2 i_uv : TEXCOORD0, 
out float4 out_position : SV_POSITION , 
out float4 o_color : COLOR0 ,
out float2 o_uv : TEXCOORD0 
)
#elif defined (EAE6320_PLATFORM_GL)

	layout( location = 0 ) in float3 i_position;
	layout( location = 1 ) in float4 i_color;
	layout( location = 2 ) in float2 i_uv;
	layout (location = 1 ) out float4 o_color;
	layout( location = 2 ) out float2 o_uv;
	void main()

#endif
{
	//float4 temp_position = float4(i_position.x + cos(g_elapsedSecondCount_simulationTime), //i_position.y+ sin(g_elapsedSecondCount_simulationTime), i_position.z, 1.0);


	float4 temp_position = float4(i_position.x , i_position.y, i_position.z, 1.0);
	float4 vertexPosition_local = temp_position;
	float4 vertexPosition_world = MUL(g_transform_localToWorld , vertexPosition_local);
	float4 vertexPosition_camera = MUL(g_transform_worldToCamera ,vertexPosition_world);
	float4 vertexPosition_projected = MUL(g_transform_cameraToProjected , vertexPosition_camera);

	//temp_position = float4(i_position.x, i_position.y , i_position.z, 1.0) 
	//+ float4(g_position.x, g_position.y, g_position.z, 0.0);
	
	o_color = i_color;
	o_uv = i_uv;
	out_position = vertexPosition_projected;

}
