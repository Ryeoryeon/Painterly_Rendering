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

int random_alpha_H_2(int R, int G, int B)
{
	int V = std::max(std::max(R, G), B);
	int S, H;

	int min = std::min(std::min(R, G), B);

	if (V == 0)
		S = 0;

	else
		S = ((V - min) / V);

	if (V == R)
		H = (30 * (G - B)) / (V - min);

	else if (V == G)
		H = 60 + (30 * (B - R)) / (V - min);

	else
		H = 120 + (30 * (R - G)) / (V - min);

	if (H < 0)
		H += 180;

	srand((unsigned int)time(0));

	// 각 값이 10% 범위 내에서만 변하도록.
	int alpha_max = H + 0.1 * H;
	int alpha_min = H + (-0.1) * H;

	H = rand() % (alpha_max - alpha_min + 1) + alpha_min;

	if (H > 180)
		H = 180;

	else if (H < 0)
		H = 0;

	return H;
}

int random_alpha_S_2(int R, int G, int B)
{
	int V = std::max(std::max(R,G),B);
	int S, H;

	int min = std::min(std::min(R, G), B);

	if (V == 0)
		S = 0;
	
	else
		S = ((V - min)/ V);

	srand((unsigned int)time(0));

	// 각 값이 10% 범위 내에서만 변하도록.
	int alpha_max = S + 0.1 * S;
	int alpha_min = S + (-0.1) * S;

	S = rand() % (alpha_max - alpha_min + 1) + alpha_min;

	if (S > 255)
		S = 255;

	else if (S < 0)
		S = 0;

	return S;
}

int random_alpha_V_2(int R, int G, int B)
{
	int V = std::max(std::max(R, G), B);

	srand((unsigned int)time(0));

	// 각 값이 10% 범위 내에서만 변하도록.
	int alpha_max = V + 0.1 * V;
	int alpha_min = V + (-0.1) * V;

	V = rand() % (alpha_max - alpha_min + 1) + alpha_min;

	if (V > 255)
		V = 255;

	else if (V < 0)
		V = 0;

	return V;

}


int random_alpha_SV(int color)
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

int random_alpha_H(int color)
{
	srand((unsigned int)time(0));

	// 각 값이 10% 범위 내에서만 변하도록.
	int alpha_max = color + 0.1 * color;
	int alpha_min = color + (-0.1) * color;

	color = rand() % (alpha_max - alpha_min + 1) + alpha_min;

	if (color > 180)
		color = 180;

	else if (color < 0)
		color = 0;

	return color;
}
