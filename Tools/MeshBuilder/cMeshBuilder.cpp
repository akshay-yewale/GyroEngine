#include "cMeshBuilder.h"
#include <Tools/AssetBuildLibrary/Functions.h>
#include <Engine\Platform\Platform.h>
#include <stdio.h>
#include <fstream>

namespace{
	size_t m_VertexDataCount;
	eae6320_001::VertexFormat::sMeshVertexDataFormat * VertexData;
	uint16_t * IndexData ;
	size_t m_indexCount ;

}

eae6320::cResult eae6320::Assets::cMeshBuilder::Build(const std::vector<std::string>& i_arguments) {
	eae6320::cResult result = eae6320::Results::Success;

	if (i_arguments.size() >= 0) {

		LoadAsset(m_path_source);

#if defined( EAE6320_PLATFORM_D3D )
		{
			size_t indexCount = (size_t)m_indexCount;
			for (int i = 2; i < indexCount; i += 3)
			{
				uint16_t _t = IndexData[i];
				IndexData[i] = IndexData[i - 1];
				IndexData[i - 1] = _t;
			}	

			for (int i = 0; i < m_VertexDataCount; i++)
			{
				VertexData[i].v = 1 - VertexData[i].v;
			}

		}
#endif
		// write to file
		{
			FILE * filePointer;
			filePointer = fopen(m_path_target, "wb");

			uint16_t temp = (uint16_t)m_indexCount;
			fwrite(&temp, sizeof(uint16_t), sizeof(temp) / sizeof(uint16_t), filePointer);

			fwrite(IndexData, sizeof(uint16_t), m_indexCount, filePointer);

			uint16_t temp2 = (uint16_t)m_VertexDataCount;
			fwrite(&temp2, sizeof(uint16_t), sizeof(temp2) / sizeof(uint16_t), filePointer);

			fwrite(VertexData, sizeof(eae6320_001::VertexFormat::sMeshVertexDataFormat), m_VertexDataCount, filePointer);
		}
	}
	else
	{
		OutputErrorMessageWithFileInfo(m_path_source,
			"There is error in a Mesh file");
		return Results::Failure;
	}
	return result;
}

eae6320::cResult eae6320::Assets::cMeshBuilder::LoadAsset(const char * const i_path) {
	auto result = eae6320::Results::Success;

	// Create a new Lua state
	lua_State* luaState = nullptr;
	{
		luaState = luaL_newstate();
		if (!luaState)
		{
			result = eae6320::Results::OutOfMemory;
			//std::cerr << "Failed to create a new Lua state" << std::endl;
			goto OnExit;
		}
	}

	const auto stackTopBeforeLoad = lua_gettop(luaState);
	{
		const auto luaResult = luaL_loadfile(luaState, i_path);
		if (luaResult != LUA_OK)
		{
			result = eae6320::Results::Failure;
			const char * error = lua_tostring(luaState, -1);
			//std::cerr << lua_tostring(luaState, -1) << std::endl;
			// Pop the error message
			lua_pop(luaState, 1);
			goto OnExit;
		}
	}

	{
		constexpr int argumentCount = 0;
		constexpr int returnValueCount = LUA_MULTRET;	// Return _everything_ that the file returns
		constexpr int noMessageHandler = 0;
		const auto luaResult = lua_pcall(luaState, argumentCount, returnValueCount, noMessageHandler);
		if (luaResult == LUA_OK)
		{
			// A well-behaved asset file will only return a single value
			const auto returnedValueCount = lua_gettop(luaState) - stackTopBeforeLoad;
			if (returnedValueCount == 1)
			{
				// A correct asset file _must_ return a table
				if (!lua_istable(luaState, -1))
				{
					result = eae6320::Results::InvalidFile;
					//std::cerr << "Asset files must return a table (instead of a " <<
					//luaL_typename(luaState, -1) << ")" << std::endl;
					// Pop the returned non-table value
					lua_pop(luaState, 1);
					goto OnExit;
				}
			}
			else
			{
				result = eae6320::Results::InvalidFile;
				//std::cerr << "Asset files must return a single table (instead of " <<
				//returnedValueCount << " values)" << std::endl;
				// Pop every value that was returned
				lua_pop(luaState, returnedValueCount);
				goto OnExit;
			}
		}
		else
		{
			result = eae6320::Results::InvalidFile;
			//std::cerr << lua_tostring(luaState, -1) << std::endl;
			// Pop the error message
			lua_pop(luaState, 1);
			goto OnExit;
		}
	}

	// If this code is reached the asset file was loaded successfully,
	// and its table is now at index -1
	result = LoadTableValues(*luaState);


	//Initialize all the neccessary parameters into the mesh : vertexdata and index data
	//this->InitializeGeometry(VertexData, m_VertexDataCount, IndexData, m_indexCount);


	// Pop the table
	lua_pop(luaState, 1);

OnExit:

	if (luaState)
	{
		// If I haven't made any mistakes
		// there shouldn't be anything on the stack,
		// regardless of any errors encountered while loading the file:
		//EAE6320_ASSERT(lua_gettop(luaState) == 0);

		lua_close(luaState);
		luaState = nullptr;
	}

	return result;
}

eae6320::cResult eae6320::Assets::cMeshBuilder::LoadTableValues(lua_State& io_luaState) {
	auto result = eae6320::Results::Success;

	if (!(result = LoadTableValues_IndexData(io_luaState)))
	{
		return result;
	}
	if (!(result = LoadTableValues_VertexData(io_luaState)))
	{
		return result;
	}

	return result;

}

eae6320::cResult eae6320::Assets::cMeshBuilder::LoadTableValues_VertexData(lua_State& io_luaState) {
	auto result = eae6320::Results::Success;
	constexpr auto* const key = "VertexData";
	lua_pushstring(&io_luaState, key);
	lua_gettable(&io_luaState, -2);

	if (lua_istable(&io_luaState, -1))
	{
		if (!(result = LoadTableValues_VertexData_Values(io_luaState)))
		{
			goto OnExit;
		}
	}
	else
	{
		result = eae6320::Results::InvalidFile;
		//std::cerr << "The value at \"" << key << "\" must be a table " + "(instead of a " << luaL_typename(&io_luaState, -1) << ")" << std::endl;
		goto OnExit;
	}

OnExit:

	// Pop the textures table
	lua_pop(&io_luaState, 1);

	return result;
}

eae6320::cResult eae6320::Assets::cMeshBuilder::LoadTableValues_VertexData_Values(lua_State& io_luaState) {
	auto result = eae6320::Results::Success;

	//std::cout << "Iterating through every texture path:" << std::endl;
	const auto VertexDataCount = luaL_len(&io_luaState, -1);
	m_VertexDataCount = static_cast<size_t>(VertexDataCount);
	VertexData = new eae6320_001::VertexFormat::sMeshVertexDataFormat[m_VertexDataCount]();

	for (int i = 1; i <= VertexDataCount; ++i)
	{
		eae6320_001::VertexFormat::sMeshVertexDataFormat *positionColor = new eae6320_001::VertexFormat::sMeshVertexDataFormat();
		lua_pushinteger(&io_luaState, i);
		lua_gettable(&io_luaState, -2);
		LoadTableValues_VertexData_Values_Position(io_luaState, *positionColor);
		lua_pop(&io_luaState, 1);

		lua_pushinteger(&io_luaState, i);
		lua_gettable(&io_luaState, -2);
		LoadTableValues_VertexData_Values_UV(io_luaState, *positionColor);
		lua_pop(&io_luaState, 1);


		lua_pushinteger(&io_luaState, i);
		lua_gettable(&io_luaState, -2);
		LoadTableValues_VertexData_Values_Color(io_luaState, *positionColor);
		lua_pop(&io_luaState, 1);


		*(VertexData + (i - 1)) = eae6320_001::VertexFormat::sMeshVertexDataFormat(*positionColor);

	}
	return result;

}

eae6320::cResult eae6320::Assets::cMeshBuilder::LoadTableValues_IndexData(lua_State& io_luaState) {
	auto result = eae6320::Results::Success;
	constexpr auto* const key = "IndexData";
	lua_pushstring(&io_luaState, key);
	lua_gettable(&io_luaState, -2);

	if (lua_istable(&io_luaState, -1))
	{
		if (!(result = LoadTableValues_IndexData_Values(io_luaState)))
		{
			goto OnExit;
		}
	}
	else
	{
		result = eae6320::Results::InvalidFile;
		//std::cerr << "The value at \"" << key << "\" must be a table " + "(instead of a " << luaL_typename(&io_luaState, -1) << ")" << std::endl;
		goto OnExit;
	}

OnExit:

	// Pop the textures table
	lua_pop(&io_luaState, 1);

	return result;
}

eae6320::cResult eae6320::Assets::cMeshBuilder::LoadTableValues_IndexData_Values(lua_State& io_luaState) {
	auto result = eae6320::Results::Success;
	//std::cout << "Iterating through every texture path:" << std::endl;
	const auto IndexCount = luaL_len(&io_luaState, -1);

	m_indexCount = static_cast<size_t>(IndexCount);
	IndexData = new uint16_t[m_indexCount]();
	
	for (int i = 1; i <= IndexCount; ++i)
	{
		lua_pushinteger(&io_luaState, i);
		lua_gettable(&io_luaState, -2);
		//std::cout << "\t" << lua_tostring(&io_luaState, -1) << std::endl;
		// this is where you got position 
		*(IndexData + (i - 1)) = static_cast<uint16_t>(lua_tointeger(&io_luaState, -1));
		lua_pop(&io_luaState, 1);
	}

	return result;

}

eae6320::cResult eae6320::Assets::cMeshBuilder::LoadTableValues_VertexData_Values_Position(lua_State& io_luaState,
	eae6320_001::VertexFormat::sMeshVertexDataFormat &o_positionColor) {
	auto result = eae6320::Results::Success;

	float position[3] = { 0.0f };
	int index = 0;
	constexpr auto* const key = "position";
	lua_pushstring(&io_luaState, key);
	lua_gettable(&io_luaState, -2);


	auto val = luaL_len(&io_luaState, -1);

	for (int i = 1; i <= val; ++i)
	{

		lua_pushinteger(&io_luaState, i);
		lua_gettable(&io_luaState, -2);
		float retrivedValue = (float)lua_tonumber(&io_luaState, -1);
		position[i - 1] = retrivedValue;
		lua_pop(&io_luaState, 1);
	}

	lua_pop(&io_luaState, 1);
	o_positionColor.x = position[0];
	o_positionColor.y = position[1];
	o_positionColor.z = position[2];
	//OnExit:
	return result;
}

eae6320::cResult eae6320::Assets::cMeshBuilder::LoadTableValues_VertexData_Values_Color(lua_State& io_luaState,
	eae6320_001::VertexFormat::sMeshVertexDataFormat &o_positionColor) {
	auto result = eae6320::Results::Success;

	uint8_t color[4];
	constexpr auto* const key = "color";
	lua_pushstring(&io_luaState, key);
	lua_gettable(&io_luaState, -2);


	auto val = luaL_len(&io_luaState, -1);

	for (int i = 1; i <= val; ++i)
	{

		lua_pushinteger(&io_luaState, i);
		lua_gettable(&io_luaState, -2);
		float retrivedValue = (float)lua_tonumber(&io_luaState, -1);
		const auto convertedValue = static_cast<uint8_t>(retrivedValue * 255.0f);
		color[i - 1] = convertedValue;
		lua_pop(&io_luaState, 1);
	}

	o_positionColor.r = color[0];
	o_positionColor.g = color[1];
	o_positionColor.b = color[2];
	o_positionColor.a = color[3];
	lua_pop(&io_luaState, 1);
	return result;
}

eae6320::cResult eae6320::Assets::cMeshBuilder::LoadTableValues_VertexData_Values_UV(lua_State& io_luaState,
	eae6320_001::VertexFormat::sMeshVertexDataFormat &o_positionColor) {
	auto result = eae6320::Results::Success;

	float color[2];
	constexpr auto* const key = "uv";
	lua_pushstring(&io_luaState, key);
	lua_gettable(&io_luaState, -2);


	auto val = luaL_len(&io_luaState, -1);

	for (int i = 1; i <= val; ++i)
	{

		lua_pushinteger(&io_luaState, i);
		lua_gettable(&io_luaState, -2);
		float retrivedValue = (float)lua_tonumber(&io_luaState, -1);
		//const auto convertedValue = static_cast<uint8_t>(retrivedValue * 255.0f);
		color[i - 1] = retrivedValue;
		lua_pop(&io_luaState, 1);
	}

	o_positionColor.u = color[0];
	o_positionColor.v = color[1];
	lua_pop(&io_luaState, 1);
	return result;
}
