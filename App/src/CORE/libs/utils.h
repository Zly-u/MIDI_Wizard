#pragma once

#include <cmath>

#include "SDL_pixels.h"


struct utils {
	static double modulo(double value, double m) {
		double mod = std::fmod(value, m);
		if (mod < 0) {
			mod += m;
		}
		return mod;
	}
	
	static SDL_Color HSL2RGB(double hue, double S, double L) {
		const double fixed_hue = modulo(hue, 360.0f);
		
		double C = (1 - std::abs(2.0f*L - 1.0f)) * S;
		double X = C * (
			1ll - std::abs(
				modulo(
					fixed_hue / 60,
					2
				)
				- 1
			)
		);
		double m = L - C/2;

		C *= 255;
		X *= 255;
		m *= 255;
		
		SDL_Color new_color;
		
		if(0 <= fixed_hue and fixed_hue < 60) {
			new_color = SDL_Color(C, X, 0);
		}
		else if (fixed_hue < 120) {
			new_color = SDL_Color(X, C, 0);
		}
		else if (fixed_hue < 180) {
			new_color = SDL_Color(0, C, X);
		}
		else if (fixed_hue < 240) {
			new_color = SDL_Color(0, X, C);
		}
		else if (fixed_hue < 300) {
			new_color = SDL_Color(X, 0, C);
		}
		else if (fixed_hue < 360) {
			new_color = SDL_Color(C, 0, X);
		}

		new_color.r += (uint8_t)m;
		new_color.g += (uint8_t)m;
		new_color.b += (uint8_t)m;
		new_color.a = 255;
		
		return new_color;
	}
};
