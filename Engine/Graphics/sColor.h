#ifndef _COLOR_H
#define _COLOR_H

namespace eae6320_001 {
	namespace Graphics {
	
		struct sColor {
			float r;
			float g; 
			float b; 
			float a;


			sColor() {
			}

			sColor& operator= (const sColor& i_copy)
			{
				r = i_copy.r;
				g = i_copy.g;
				b = i_copy.b;
				a = i_copy.a;

			}

			const sColor Red() {
				r = 1.0f; g = 0.0f; b = 0.0f; a = 1.0f;
			};
			const sColor Green() {
				r = 0.0f; g = 1.0f; b = 0.0f; a = 1.0f;
			};
			const sColor Blue() {
				r = 0.0f; g = 0.0f; b = 1.0f; a = 1.0f;
			};
			const sColor White() {
				r = 1.0f; g = 1.0f; b = 1.0f; a = 1.0f;
			}; 
			const sColor Black() {
				r = 0.0f; g = 0.0f; b = 0.0f; a = 1.0f;
			};

		};
	}
}

#endif // 
