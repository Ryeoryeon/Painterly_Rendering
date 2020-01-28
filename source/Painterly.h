#pragma once
#include "opencv2/opencv.hpp"
#include <vector>

static int MARGIN = 10;

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
	//std::vector<cv::Point_<int>> dot_list;
	cv::Mat paint(float T, cv::Mat& canvas, const cv::Mat& reference, std::vector<layer>& layer_list, const std::vector<std::vector<float>> & image_etf_dx, const std::vector<std::vector<float>>& image_etf_dy);
	int calculate_margin(int layer, int length);

	int get_layersize() { return layer_size; };
	void put_layersize(int input_lay) { layer_size = input_lay; }
	void push_back(int brush_size, double f_g, int grid) { layer_list.push_back(layer(brush_size, f_g, grid)); }

};

cv::Mat blurring(cv::Mat & image, double g_sigma);
int difference(int canvas_color, int reference_color);
int random_alpha_H(int color);
int random_alpha_SV(int color);
void makeVectorCoherent(float ori_dx, float ori_dy, float& coh_dx, float& coh_dy);
bool getFlowVectorInterpolated(int m_nWidth, int m_nHeight, float x, float y, float& dx, float& dy, const std::vector<std::vector<float>>& image_dx, const std::vector<std::vector<float>>& image_dy);
bool getFlowVectorRK4(int m_nWidth, int m_nHeight, float x, float y, float& dx, float& dy, const std::vector<std::vector<float>>& image_dx, const std::vector<std::vector<float>>& image_dy);