#pragma once
#include "opencv2/opencv.hpp"
#include <vector>

static int MARGIN = 30;
static int STROKE_NUMBER = 5;
static float ALPHA = 0.7f;
static float GAUSSIAN_SIGMA = 3;
static int GAUSSIAN_SIZE = 7;

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
	int option;


public:
	std::vector<layer> Painterly_initialize();
	std::vector<layer> layer_list; // 나중에 접근함수 만들어서 바꿔주기
	//std::vector<cv::Point_<int>> dot_list;
	cv::Mat paint(float T, const cv::Mat& saliency_output, cv::Mat& canvas, const cv::Mat& reference, const std::vector<std::vector<int>> brush, std::vector<layer>& layer_list, const std::vector<std::vector<float>> & image_etf_dx, const std::vector<std::vector<float>>& image_etf_dy);
	cv::Mat paint_airbrush(float T, const cv::Mat& saliency_output, cv::Mat& canvas, const cv::Mat& reference, const std::vector<std::vector<int>> brush, std::vector<layer>& layer_list, const std::vector<std::vector<float>>& image_etf_dx, const std::vector<std::vector<float>>& image_etf_dy);
	int calculate_margin(int layer, int length);

	int get_layersize() { return layer_size; };
	void put_layersize(int input_lay) { layer_size = input_lay; }
	int get_option() { return option; }
	void put_option(int op) { option = op; }
	void push_back(int brush_size, double f_g, int grid) { layer_list.push_back(layer(brush_size, f_g, grid)); }

};

cv::Mat blurring(cv::Mat & image, double g_sigma);
cv::Mat Bilateral_filtering(const cv::Mat& image);

int difference(int canvas_color, int reference_color);

float random_alpha_H(float color);
float random_alpha_S(float color);
float random_alpha_V(float color);

void makeVectorCoherent(float ori_dx, float ori_dy, float& coh_dx, float& coh_dy);
bool getFlowVectorInterpolated(int m_nWidth, int m_nHeight, float x, float y, float& dx, float& dy, const std::vector<std::vector<float>>& image_dx, const std::vector<std::vector<float>>& image_dy);
bool getFlowVectorRK4(int m_nWidth, int m_nHeight, float x, float y, float& dx, float& dy, const std::vector<std::vector<float>>& image_dx, const std::vector<std::vector<float>>& image_dy);

void dw_RGB2HSV(int r, int g, int b, float& h, float& s, float& v);	//h(0:360), s(0:1), v(0:255)
void dw_HSV2RGB(float h, float s, float v, int& r, int& g, int& b);	//h(0:360), s(0:1), v(0:255)

//cv::Mat accumulate_image(int width, int height, const std::vector<std::vector<int>> & accum_height);