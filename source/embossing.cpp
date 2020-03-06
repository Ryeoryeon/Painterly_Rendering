#include <iostream>
#include "Painterly.h"
#include <vector>
#include <cmath>
#include "opencv2/opencv.hpp"
#include "yh_vector.h"
#include <ppl.h>

void embossing(cv::Mat & canvas, const cv::Mat & accum_image, float K_val)
{
	int width = canvas.cols;
	int height = canvas.rows;


	//맨 왼쪽과 맨 오른쪽, 맨 위와 맨 아래의 경우는 계산할 픽셀이 없기 때문에.
	Concurrency::parallel_for(1, width, [&](int x)
	//for (int x = 1; x < width - 1; x++)
	{
		for (int y = 1; y < height - 1; y++)
		{
			yh_vector x_vector(2, 0, ((255 - accum_image.at<uchar>(y, x + 1)) - (255 - accum_image.at<uchar>(y, x - 1)))); // x=2, y=0은 고정
			float x_size = x_vector.get_size();

			yh_vector y_vector(0, 2, ((255 - accum_image.at<uchar>(y + 1, x)) - (255 - accum_image.at<uchar>(y - 1, x)))); // x=0, y=2는 고정
			float y_size = y_vector.get_size();

			x_vector.divide_x(x_size);
			x_vector.divide_z(x_size);

			y_vector.divide_y(y_size);
			y_vector.divide_z(y_size);
			
			yh_vector normal_vector = x_vector.outer(y_vector);

			float inner_output = normal_vector.inner(LIGHT); // 두 단위벡터를 내적했으므로 0~1 사이의 값


			float multiply;

			/*
			if (inner_output >= 0)
			{
				for (int l = 0; l < 3; l++)
				{
					float index = (K_val * inner_output * canvas.at<cv::Vec3b>(y, x)[l]) + canvas.at<cv::Vec3b>(y, x)[l];

					if (index > 255)
						canvas.at<cv::Vec3b>(y, x)[l] = 255;

					else
						canvas.at<cv::Vec3b>(y, x)[l] = (K_val * inner_output * canvas.at<cv::Vec3b>(y, x)[l]) + canvas.at<cv::Vec3b>(y, x)[l];
				}

			}
			else
			{
				for (int l = 0; l < 3; l++)
				{
					canvas.at<cv::Vec3b>(y, x)[l] = canvas.at<cv::Vec3b>(y, x)[l];
				}
			}
			*/

			
			if (inner_output >= 0)
			{
				for (int l = 0; l < 3; l++)
				{
					canvas.at<cv::Vec3b>(y, x)[l] = (K_val * inner_output * canvas.at<cv::Vec3b>(y, x)[l]) + ((1 - K_val) * canvas.at<cv::Vec3b>(y, x)[l]);
				}
			}
			else
			{
				for (int l = 0; l < 3; l++)
				{
					canvas.at<cv::Vec3b>(y, x)[l] = (1 - K_val) * canvas.at<cv::Vec3b>(y, x)[l];
				}
			}
			

		}
	});

	cv::imwrite("embossing_canvas.jpg", canvas);
}