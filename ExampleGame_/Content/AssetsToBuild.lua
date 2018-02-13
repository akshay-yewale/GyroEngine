--[[
	This file lists every asset that must be built by the AssetBuildSystem
]]

return
{
	meshes =
	{
		"Mesh/Cube.lua",
		"Mesh/Plane.lua",
		"Mesh/RedOrb.lua",
		"Mesh/BlueOrb.lua",
		"Mesh/StarShip.lua",
	},
	shaders =
	{
		{path = "Shaders/Vertex/MeshVertexShader_001.sl", arguments = {"vertex"}},
		{path = "Shaders/Vertex/MeshVertexShader_002.sl", arguments = {"vertex"}},
		{path = "Shaders/Vertex/VertexShader_001.sl", arguments = {"vertex"}},
		{path = "Shaders/Vertex/VertexShader_002.sl", arguments = {"vertex"}},
		{path = "Shaders/Fragment/MeshFragmentShader_001.sl", arguments = {"fragment"}},
		{path = "Shaders/Fragment/FragmentShader_001.sl", arguments = {"fragment"}},
		{path = "Shaders/Fragment/FragmentShader_002.sl", arguments = {"fragment"}},
		{path = "Shaders/Vertex/sMeshVertexInputLayout_001.sl", arguments = {"vertex"}},
		{path = "Shaders/Vertex/sSprite_vertexInputLayout_geometry.sl", arguments = {"vertex"}},
		{path = "Shaders/Fragment/MeshFragmentShader_002.sl", arguments = {"fragment"}}, --translucent shaders

	},
	textures =
	{
		"Textures/Plane.png",
		"Textures/wolfrun_001.png",
		"Textures/wolfrun_002.png",
		"Textures/Logo.png",
		"Textures/Logo2.png",
		"Textures/StarShip_Texture.png",
		"Textures/RedOrb_001.png",
		"Textures/RedOrb_002.png",
		"Textures/BlueOrb_001.png",
		"Textures/BlueOrb_002.png",
		"Textures/LostState.png",
	},
}
