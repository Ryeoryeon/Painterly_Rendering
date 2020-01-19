#include <iostream>
#include <vector>
#include "opencv2/opencv.hpp"
#include "Painterly.h"

int difference(int canvas_color, int reference_color)
{
	if (canvas_color - reference_color < 0)
		return (-1) * (canvas_color - reference_color);

	else
		return canvas_color - reference_color;

}

int getRandom(int min, int max)
{
	// Assumes std::srand() has already been called 
	// Assumes max - min <= RAND_MAX

	static const double fraction = 1.0 / (RAND_MAX + 1.0); 
	// static used for efficiency, so we only calculate this value once 
	// evenly distribute the random number across our range 
	
	return min + static_cast<int>((max - min + 1) * (std::rand() * fraction));

}