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
