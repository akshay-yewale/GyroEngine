#ifndef  _SPRITE_VERTEX_FORMAT_H_
#define  _SPRITE_VERTEX_FORMAT_H_
#include "Configuration.h"
#include <stdint.h>

namespace eae6320_001 {
	namespace VertexFormat {
		struct sSpriteVertexFormat {
				// POSITION
				// 2 floats == 8 bytes
				// Offset = 0
				float x, y;
				float u, v;

				sSpriteVertexFormat(){
				}

				sSpriteVertexFormat(const sSpriteVertexFormat & i_copy) :
					x(i_copy.x),
					y(i_copy.y),
					u(i_copy.u),
					v(i_copy.v)

				{
				}

				sSpriteVertexFormat(float i_x, float i_y, float i_u, float i_v) {
					x = i_x;
					y = i_y;
					u = i_u;
					v = i_v;
				}
		};

		struct sTextureFormat {
			float u, v;

			sTextureFormat() {
			}


			sTextureFormat(const sTextureFormat & i_copy):
			u(i_copy.u),
			v(i_copy.v)
			{
			}

			sTextureFormat(float i_u, float i_v) {
				u = i_u;
				v = i_v;
			}
		};

		struct sMeshVertexFormat {
			float x, y;
			uint8_t r, b, g, a;

			sMeshVertexFormat() {
			}
		

			sMeshVertexFormat(const sMeshVertexFormat &i_copy) :
				x(i_copy.x),
				y(i_copy.y)
			{
				r = i_copy.r;
				g = i_copy.g;
				b = i_copy.b;
				a = i_copy.a;

			}


			sMeshVertexFormat(float i_x, float i_y, uint8_t i_r, uint8_t i_g, uint8_t i_b, uint8_t i_a) :
				x(i_x),
				y(i_y),
				r(i_r),
				g(i_g),
				b(i_g),
				a(i_a)
			{
			}
		};
		// added uv for texture to mesh
		struct sMeshVertexDataFormat {
			float x, y, z;
			uint8_t r, g, b, a;
			float u, v;

			sMeshVertexDataFormat() {
				x = 0.0f;
				y = 0.0f;
				z = 0.0f;
				r = 255;
				g = 255;
				b = 255;
				a = 255;
				u = 0.0f;
				v = 0.0f;
			}

			sMeshVertexDataFormat(const sMeshVertexDataFormat &i_copy) :
				x(i_copy.x),
				y(i_copy.y),
				z(i_copy.z),
				r(i_copy.r),
				g(i_copy.g),
				b(i_copy.b),
				a(i_copy.a),
				u(i_copy.u),
				v(i_copy.v)
			{
			}

			sMeshVertexDataFormat(float i_x, float i_y, float i_z, uint8_t i_r,
				uint8_t i_g, uint8_t i_b, uint8_t i_a, float i_u, float i_v):
			x(i_x), y(i_y), z(i_z), 
			r(i_r),	g(i_g), b(i_b), a(i_a),
			u(i_u),v(i_v)
			{		
			}
		};
	}
}
#endif // ! _SPRITE_VERTEX_FORMAT_H_
