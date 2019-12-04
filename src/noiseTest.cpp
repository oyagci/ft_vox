#include "SimplexNoise.hpp"
#include <cmath>

typedef char t_color[3];

struct Texture {
	t_color data[512][512];
};

Texture *genTextureData(float freq, float amp, float lac, float pers)
{
	SimplexNoise s = SimplexNoise(freq, amp, lac, pers);
	Texture *texture = new Texture;

	__builtin_memset(texture, 0xff, sizeof(unsigned char) * sizeof(Texture));

	for (int i = 0; i < 512; i++) {
		for (int j = 0; j < 512; j++) {

			float valf = s.fractal(7, i, j);
			char val = std::round((valf + 1) * 255);

			texture->data[i][j][0] = val;
			texture->data[i][j][1] = val;
			texture->data[i][j][2] = val;
		}
	}

	return (texture);
}
