#include <iostream>
#include <vector>
#include <ctime>
#include "opencv2/opencv.hpp"
#include "Painterly.h"

int difference(int canvas_color, int reference_color)
{
	if (canvas_color - reference_color < 0)
		return (-1) * (canvas_color - reference_color);

	else
		return canvas_color - reference_color;

}

void dw_RGB2HSV(int r, int g, int b, float& h, float& s, float& v)	//h(0:360), s(0:1), v(0:255)
{
	float	fMin, fMax, fDelta;
	float	newH, newS, newV;

	fMin = r < g ? r : g;
	fMin = fMin < b ? fMin : b;

	fMax = r > g ? r : g;
	fMax = fMax > b ? fMax : b;

	newV = fMax;				// v
	fDelta = fMax - fMin;
	if (fMax > 0.0)				// NOTE: if Max is == 0, this divide would cause a crash
	{
		newS = (fDelta / fMax);	// s
	}
	else						// if max is 0, then r = g = b = 0              
	{							// s = 0, v is undefined
		newS = 0.0;
		newH = 0;				// its now undefined

		h = newH;
		s = newS;
		v = newV;

		return;
	}

	if (fDelta == 0.f)
		newH = 0;
	else
	{
		if (r >= fMax)						// > is bogus, just keeps compilor happy
			newH = (g - b) / fDelta;		// between yellow & magenta
		else if (g >= fMax)
			newH = 2.0 + (b - r) / fDelta;	// between cyan & yellow
		else
			newH = 4.0 + (r - g) / fDelta;	// between magenta & cyan
	}
	newH *= 60.0;						// degrees

	if (newH < 0.0)
		newH += 360.0;

	h = newH;
	s = newS;
	v = newV;
	return;
}

void dw_HSV2RGB(float h, float s, float v, int& r, int& g, int& b)	//h(0:360), s(0:1), v(0:255)
{
	float	hh, p, q, t, ff;
	int		i;

	if (s <= 0.0)			// < is bogus, just shuts up warnings
	{
		r = v;
		g = v;
		b = v;
		return;
	}

	hh = h;
	if (hh >= 360.0)
		hh = 0.0;

	hh /= 60.0;
	i = (int)hh;
	ff = hh - i;
	p = v * (1.0 - s);
	q = v * (1.0 - (s * ff));
	t = v * (1.0 - (s * (1.0 - ff)));

	switch (i) {
	case 0:
		r = v;
		g = t;
		b = p;
		break;
	case 1:
		r = q;
		g = v;
		b = p;
		break;
	case 2:
		r = p;
		g = v;
		b = t;
		break;

	case 3:
		r = p;
		g = q;
		b = v;
		break;
	case 4:
		r = t;
		g = p;
		b = v;
		break;
	case 5:
	default:
		r = v;
		g = p;
		b = q;
		break;
	}

	if (r < 0) r = 0;
	if (g < 0) g = 0;
	if (b < 0) b = 0;

	if (r > 255) r = 255;
	if (g > 255) g = 255;
	if (b > 255) b = 255;

	return;
}

float random_alpha_V(float color)
{
	srand((unsigned int)time(0));

	// 각 값이 10% 범위 내에서만 변하도록.
	int alpha_max = color + 0.1 * color;
	int alpha_min = color + (-0.1) * color;

	color = rand() % (alpha_max - alpha_min + 1) + alpha_min;

	if (color > 255)
		color = 255;

	else if (color < 0)
		color = 0;

	return color;
}

float random_alpha_S(float color)
{
	srand((unsigned int)time(0));

	// 각 값이 10% 범위 내에서만 변하도록.
	float alpha_max = color + 0.05 * color;
	float alpha_min = color + (-0.05) * color;

	//color = rand() % (alpha_max - alpha_min + 1) + alpha_min;
	color = ((alpha_max - alpha_min) * ((float)rand() / RAND_MAX)) + alpha_max; // float형

	if (color > 1)
		color = 1;

	else if (color < 0)
		color = 0;

	return color;
}

float random_alpha_H(float color)
{
	srand((unsigned int)time(0));

	// 각 값이 10% 범위 내에서만 변하도록.
	int alpha_max = color + 0.1 * color;
	int alpha_min = color + (-0.1) * color;

	color = rand() % (alpha_max - alpha_min + 1) + alpha_min;

	if (color > 360)
		color = 360;

	else if (color < 0)
		color = 0;

	return color;
}
