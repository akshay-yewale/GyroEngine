#include "Mesh.h"
#include <Engine\Asserts\Asserts.h>
#include <Engine\Platform\Platform.h>


eae6320::Assets::cManager<eae6320_001::Mesh> eae6320_001::Mesh::s_manager;

eae6320::cResult eae6320_001::Mesh::Initialize(eae6320_001::Mesh *& o_mesh) {
	auto result = eae6320::Results::Success;

	{
		auto result = eae6320::Results::Success;
		Mesh * newMesh = nullptr;

		{
			newMesh = new (std::nothrow) Mesh();
			if (!newMesh) {
				result = eae6320::Results::OutOfMemory;
				goto OnExit;
			}
		}
	OnExit:
		if (result)
		{
			EAE6320_ASSERT(newMesh);
			o_mesh = newMesh;
			return result;

		}
		else
		{
			if (newMesh) {
				newMesh->DecrementReferenceCount();
				newMesh = nullptr;

			}
			o_mesh = nullptr;
		}

		return result;
	}
}

eae6320::cResult eae6320_001::Mesh::Load(const char* const i_path, eae6320_001::Mesh *& o_Mesh) {

	auto result = eae6320::Results::Success;
	Mesh * newMesh = nullptr;
	
	newMesh = new (std::nothrow) Mesh();
	if (!newMesh)
	{
		result = eae6320::Results::OutOfMemory;
		EAE6320_ASSERTF(false, "Couldn't allocate memory for the texture %s", i_path);
		eae6320::Logging::OutputError("Failed to allocate memory for the texture %s", i_path);
		goto OnExit;
	}


OnExit:
	if (result)
	{
		EAE6320_ASSERT(newMesh);
		o_Mesh = newMesh;
		o_Mesh->LoadDataFromFile(i_path);
		//o_Mesh->LoadAsset(i_path);
		o_Mesh->InitializeGeometry(o_Mesh->VertexData, o_Mesh->m_VertexDataCount, o_Mesh->IndexData, o_Mesh->m_indexCount);

	}
	else
	{
		if (newMesh)
		{
			newMesh->DecrementReferenceCount();
			newMesh = nullptr;
		}
		o_Mesh = nullptr;
	}
	return result;
}

eae6320::cResult eae6320_001::Mesh::LoadDataFromFile(const char* filename) {
	
	auto result = eae6320::Results::Success;
	eae6320::Platform::sDataFromFile dataFromFile;
	std::string errorMessage;
	unsigned int* o_dataSize = new unsigned int();
	//const void ** o_dataFromFile = nullptr ;
	const auto localResult = eae6320::Platform::LoadBinaryFile(filename, dataFromFile, &errorMessage);
	if (localResult)
	{
		//const void **o_dataFromFile = const_cast<const void**>(dataFromFile.data);
		EAE6320_ASSERT(dataFromFile.size < (uint64_t(1) << (sizeof(*o_dataSize) * 8)));
		*o_dataSize = static_cast<unsigned int>(dataFromFile.size);

		//Get indexCOunt
		//const auto vertexCount = *reinterpret_cast<uint16_t*>( currentOffset );
		
		
		auto currentOffset = reinterpret_cast<uintptr_t>(dataFromFile.data);
		const auto i_IndexCount = *reinterpret_cast<uint16_t*>(currentOffset);
		currentOffset += sizeof(i_IndexCount);
		this->m_indexCount = (size_t)i_IndexCount;
		//const auto* const vertexArray = reinterpret_cast<VertexFormats::sMesh*>( currentOffset );
		const auto * const i_IndexData = reinterpret_cast<uint16_t*>(currentOffset);
		currentOffset += (sizeof(uint16_t) * i_IndexCount);
		this->IndexData = const_cast<uint16_t*>(i_IndexData);
		const auto i_VertexCount = *reinterpret_cast<uint16_t*>(currentOffset);
		currentOffset += sizeof(uint16_t);
		this->m_VertexDataCount = (size_t)i_VertexCount;
		const auto * const i_VertexData = reinterpret_cast<eae6320_001::VertexFormat::sMeshVertexDataFormat*>(currentOffset);
		this->VertexData = const_cast<eae6320_001::VertexFormat::sMeshVertexDataFormat*>( i_VertexData);
		
		// changing the color 
		/*for (int i = 0; i < this->m_VertexDataCount; i++)
		{
			this->VertexData[i].a = (uint8_t)1.0f;
		}*/

	}
	else
	{
		/*	eae6320::Assets::OutputErrorMessageWithFileInfo(__FILE__, __LINE__,
				"Failed to open requested #include path: %s",
				errorMessage.c_str());*/
		//goto OnExit;
	}

//OnExit:
	return result;
}

/*

eae6320::cResult eae6320_001::Mesh::Initialize(eae6320_001::Mesh *& o_mesh, const char* filename) {
	auto result = eae6320::Results::Success;

	{
		auto result = eae6320::Results::Success;
		Mesh * newMesh = nullptr;

		{
			newMesh = new (std::nothrow) Mesh();
			if (!newMesh) {
				result = eae6320::Results::OutOfMemory;
				goto OnExit;
			}
		}
	OnExit:
		if (result)
		{
			EAE6320_ASSERT(newMesh);
			o_mesh = newMesh;
			char * fMeshFile = new char[100];
			strcpy(fMeshFile, "data/Mesh/");
			strcat(fMeshFile, filename);

			if (!(result = o_mesh->LoadAsset(fMeshFile)))
			{
				return result;
			}


			return result;

		}
		else
		{
			if (newMesh) {
				newMesh->DecrementReferenceCount();
				newMesh = nullptr;

			}
			o_mesh = nullptr;
		}

		return result;
	}
}

eae6320::cResult eae6320_001::Mesh::LoadAsset(const char * const i_path) {
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
	this->InitializeGeometry(VertexData, m_VertexDataCount, IndexData, m_indexCount);


	// Pop the table
	lua_pop(luaState, 1);

OnExit:

	if (luaState)
	{
		// If I haven't made any mistakes
		// there shouldn't be anything on the stack,
		// regardless of any errors encountered while loading the file:
		EAE6320_ASSERT(lua_gettop(luaState) == 0);

		lua_close(luaState);
		luaState = nullptr;
	}

	return result;
}

eae6320::cResult eae6320_001::Mesh::LoadTableValues(lua_State& io_luaState) {
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

eae6320::cResult eae6320_001::Mesh::LoadTableValues_VertexData(lua_State& io_luaState) {
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

eae6320::cResult eae6320_001::Mesh::LoadTableValues_VertexData_Values(lua_State& io_luaState) {
	auto result = eae6320::Results::Success;

	//std::cout << "Iterating through every texture path:" << std::endl;
	const auto VertexDataCount = luaL_len(&io_luaState, -1);
	m_VertexDataCount =	 static_cast<size_t>( VertexDataCount);
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


		*(VertexData  + (i - 1)) = eae6320_001::VertexFormat::sMeshVertexDataFormat(*positionColor);

	}
	return result;

}

eae6320::cResult eae6320_001::Mesh::LoadTableValues_IndexData(lua_State& io_luaState) {
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

eae6320::cResult eae6320_001::Mesh::LoadTableValues_IndexData_Values(lua_State& io_luaState) {
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
		*(IndexData + (i-1)) = static_cast<uint16_t>(lua_tointeger(&io_luaState, -1));
		lua_pop(&io_luaState, 1);
	}
	return result;

}

eae6320::cResult eae6320_001::Mesh::LoadTableValues_VertexData_Values_Position(lua_State& io_luaState ,
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
		position[i-1] = retrivedValue;
		lua_pop(&io_luaState, 1);
	}
	
	lua_pop(&io_luaState, 1);
	o_positionColor.x = position[0];
	o_positionColor.y = position[1];
	o_positionColor.z = position[2];
//OnExit:
	return result;
}

eae6320::cResult eae6320_001::Mesh::LoadTableValues_VertexData_Values_Color(lua_State& io_luaState,
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

eae6320::cResult eae6320_001::Mesh::LoadTableValues_VertexData_Values_UV(lua_State& io_luaState,
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

*/