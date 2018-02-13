#ifndef EAE6320_CMESHBUILDER_H
#define EAE6320_CMESHBUILDER_H

#include <Tools/AssetBuildLibrary/cbBuilder.h>
#include <Engine/Graphics/Configuration.h>
#include <External/Lua/Includes.h>

#include <Engine\Graphics\sSpriteVertexFormat.h>

namespace eae6320 {

	namespace Assets
	{
		class cMeshBuilder : public cbBuilder
		{
		private :
			//Build
			virtual cResult Build(const std::vector<std::string>& i_arguments) override;


			eae6320::cResult LoadAsset(const char* const i_path);

			eae6320::cResult LoadTableValues(lua_State& io_luaState);

			eae6320::cResult LoadTableValues_VertexData(lua_State& io_luaState);

			eae6320::cResult LoadTableValues_VertexData_Values(lua_State& io_luaState);

			eae6320::cResult LoadTableValues_IndexData(lua_State& io_luaState);

			eae6320::cResult LoadTableValues_IndexData_Values(lua_State& io_luaState);

			eae6320::cResult LoadTableValues_VertexData_Values_Position(lua_State& io_luaState
				, eae6320_001::VertexFormat::sMeshVertexDataFormat &o);

			eae6320::cResult LoadTableValues_VertexData_Values_Color(lua_State& io_luaState,
				eae6320_001::VertexFormat::sMeshVertexDataFormat &o);

			eae6320::cResult LoadTableValues_VertexData_Values_UV(lua_State& io_luaState,
				eae6320_001::VertexFormat::sMeshVertexDataFormat &o);


		};
	}
}



#endif // !EAE6320_CMESHBUILDER_H
