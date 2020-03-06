#pragma once
#include <opencv2/opencv.hpp>
#include <cmath>

struct yh_vector
{
private:
	float x;
	float y;
	float z;

public:

	int inner(yh_vector v2)
	{
		int inner_output = 0;
		inner_output += x * v2.x;
		inner_output += y * v2.y;
		inner_output += z * v2.z;

		return inner_output;
	}

	yh_vector outer(yh_vector v2)
	{
		yh_vector outer_output;
		outer_output.x = y * v2.z - z * v2.y;
		outer_output.y = z * v2.x - x * v2.z;
		outer_output.z = x * v2.y - y * v2.x;

		return outer_output;
	}

	float get_size()
	{
		float size;
		size = pow(x, 2) + pow(y, 2) + pow(z, 2);
		size = sqrt(size);

		return size;
	}

	void divide_x(float x1) { x /= x1; };
	void divide_y(float y1) { y /= y1; };
	void divide_z(float z1) { z /= z1; };

	float get_z() { return z; };

	yh_vector() { x = 0; y = 0; z = 0; }
	yh_vector(float x1, float y1, float z1) { x = x1; y = y1; z = z1; }
};


static yh_vector LIGHT((-1 / sqrt(3)), (-1 / sqrt(3)), (1 / sqrt(3)));
void embossing(cv::Mat& canvas, const cv::Mat& accum_image, float K_val);