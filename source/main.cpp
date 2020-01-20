#include <iostream>
#include <opencv2/opencv.hpp>
#include "Painterly.h"
#include <vector>


int main()
{

	stroke circle; // 전역변수에서 그냥 여기로 바꿔봄

	cv::Mat image = cv::imread("Cat.jpg");
	int height = image.rows;
	int width = image.cols;

	cv::Mat canvas = cv::imread("Cat.jpg");
	canvas = cv::imread("Cat.jpg");
	canvas = cv::Scalar(255, 255, 255); // 캔버스 단색으로 만들기

	cv::Mat blur_image = cv::imread("Cat.jpg");

	double g_sigma; // 가우시안 블러링함수 시그마
	std::cout << "가우시안 시그마값 입력하기 : ";
	std::cin >> g_sigma;
	blur_image = blurring(blur_image, g_sigma);

	int layer_num;
	std::cout << "레이어의 개수(브러시의 개수) 입력하기 :";
	std::cin >> layer_num;
	circle.put_layersize(layer_num);

	circle.layer_list = circle.Painterly_initialize();
	//마진 계산 함수 더 효율적으로 계산시키기

	//int layer_num = circle.get_layersize();
	//레이어의 개수만큼 페인트칠 단계가 필요
	int T = 500;
	circle.paint(T, canvas, blur_image, circle.layer_list);

	/*
	원래 계획 (각 레이어마다 반복문 따로 실행되도록.
	for (int i = 0; i < layer_num; i++)
	{
		//이미지 전체를 순회하며 마진을 빼고 페인트칠
		circle.paint(canvas, blur_image, circle.layer_list);
		for (int k = margin_h_index; k < height-margin_h_index; k++)
		{
			for (int l = margin_w_index; l < width-margin_w_index; l++)
			{
				circle.paint(canvas, blur_image, circle.layer_list);
			}
		}
	}
	*/

	cv::imshow("Painterly_Rendering", canvas);
	cv::waitKey(0);

	return 0;
}
