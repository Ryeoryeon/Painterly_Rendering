#pragma once
#include "opencv2/opencv.hpp"
#include <vector>

struct layer
{
	layer(int brush_size, double f_g, int grid_size)
	{
		this->brush_size = brush_size;
		this->f_g = f_g;
		this->grid_size = grid_size;
	}

	int brush_size;
	double f_g;
	int grid_size;
};

class stroke
{
	int layer_size;


public:
	std::vector<layer> Painterly_initialize();
	std::vector<layer> layer_list; // 나중에 접근함수 만들어서 바꿔주기
	cv::Mat paint(int T, cv::Mat& canvas, const cv::Mat& reference, std::vector<layer>& layer_list);
	int calculate_margin(int layer, int length);

	int get_layersize() { return layer_size; };
	void put_layersize(int input_lay) { layer_size = input_lay; }
	void push_back(int brush_size, double f_g, int grid) { layer_list.push_back(layer(brush_size, f_g, grid)); }

};

cv::Mat blurring(cv::Mat image, double g_sigma);
int difference(int canvas_color, int reference_color);
