#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <opencv2/opencv.hpp>
#include "Painterly.h"
#include <vector>
#include <opencv2/saliency/saliencySpecializedClasses.hpp> // saliency를 쓰기 위해 추가해야 하는 헤더파일


int main()
{

	stroke circle;

	//cv::Mat brush = cv::imread("brush_1.png", cv::IMREAD_GRAYSCALE);
	//cv::Mat brush = cv::imread("brush_2.png", cv::IMREAD_GRAYSCALE);
	//cv::Mat brush = cv::imread("brush_3.png", cv::IMREAD_GRAYSCALE);
	//cv::Mat brush = cv::imread("airbrush_2.png", cv::IMREAD_GRAYSCALE);
	cv::Mat brush = cv::imread("airbrush_4.png", cv::IMREAD_GRAYSCALE);

	int b_w = brush.cols;

	std::vector<std::vector<int>> brush_vec;
	brush_vec.assign(b_w, std::vector<int>(b_w, 0));

	//브러시의 밝기 저장하기
	for (int x = 0; x < b_w; x++)
	{
		for (int y = 0; y < b_w; y++)
		{
			brush_vec[x][y] = brush.at<uchar>(y, x);
		}
	}


	//cv::Mat image = cv::imread("landscape.jpg");
	//cv::Mat image = cv::imread("gra_2.png");
	cv::Mat image = cv::imread("lenna.jpg");
	//cv::Mat image = cv::imread("cloud.jpeg");
	int height = image.rows;
	int width = image.cols;

	cv::Mat canvas = cv::imread("empty_canvas_final.png");
	//cv::Mat canvas = cv::imread("empty_canvas_2.jpg");
	//cv::Mat canvas = cv::imread("Newsprint.jpg");
	//cv::Mat canvas = cv::imread("Washi.jpg");
	//canvas = cv::Scalar(255, 255, 255); // 캔버스가 흰색 단색이었으면 좋겠을 때

	cv::resize(canvas, canvas, cv::Size(width + 2*MARGIN, height + 2*MARGIN), 0, 0);

	//cv::Mat blur_image = cv::imread("landscape.jpg");
	cv::Mat blur_image = cv::imread("lenna.jpg");
	//cv::Mat blur_image = cv::imread("cloud.jpeg");
	//cv::Mat blur_image = cv::imread("gra_2.png");

	FILE* etf;
	//etf = fopen("landscape.etf", "rb");
	etf = fopen("lenna_2.etf", "rb");
	//etf = fopen("cloud.etf", "rb");
	//etf = fopen("gra_2.etf", "rb");

	if (etf == NULL)
	{
		exit(1);
	}

	cv::Mat saliency_output;

	cv::saliency::StaticSaliencySpectralResidual Sal;
	Sal.computeSaliency(image, saliency_output);

	//뻗는지 확인하기 위해 넣은 코드
	cv::Mat grad_x = cv::Mat::zeros(cv::Size(width, height), CV_32FC1);
	cv::Mat grad_y = cv::Mat::zeros(cv::Size(width, height), CV_32FC1);

	cv::Sobel(image, grad_x, CV_32FC1, 1, 0, 5);     // x 방향
	cv::Sobel(image, grad_y, CV_32FC1, 0, 1, 5);     // y 방향
	//뻗 확인 끝
	

;	float* buffer = new float[2 * width * height]; // 벡터로 하니까 안 된다.
	std::vector<std::vector<float>> image_etf_dx;
	std::vector<std::vector<float>> image_etf_dy;

	image_etf_dx.assign(width, std::vector<float>(height, 0));
	image_etf_dy.assign(width, std::vector<float>(height, 0));
	//size of float (width*height*size of float*2) // 이렇게 메모리를 할당을 한 다음에,

	/*
	fseek(etf, 0, SEEK_END); // 파일의 마지막 위치로 이동
	int etf_size = ftell(etf); // 마지막 위치 값을 받아서 파일의 크기 계산 (lenna기준 2457600 (640 * 480 * (size of float == 4) * 2))
	fseek(etf, 0, SEEK_SET); // 파일의 처음 위치로 커서 이동
	*/

	//재완언니 : 단일 배열 하나만 넣음. 여기에 dx, dy 모두 저장했는데 인덱스가 짝수, 홀수냐에 따라 dx,dy저장 가능
	fread(buffer, sizeof(float), 2 * width * height, etf);

	int index = 0;

	//x에 대해서 y가 증가하는거니까 세로방향으로 증가하는 느낌. 즉 etf가 가로세로를 거꾸로 불러왔다
/*	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			image_etf_dx[x][y] = buffer[index++];
			image_etf_dy[x][y] = buffer[index++];
		}
	}
*/
	//y에 대해서 x가 증가하는거니까 이게 가로방향으로 증가하는 느낌. etf는 가로로 쭉 붙이는거기때문에..
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			image_etf_dx[x][y] = buffer[index++];
			image_etf_dy[x][y] = buffer[index++];
		}
	}

	fclose(etf);

	std::cout << "레퍼런스 이미지 선택하기" << '\n';
	std::cout << "1번 : Gaussian blurring" << '\n';
	std::cout << "2번 : Bilateral filtering" << '\n';

	int choice;
	std::cin >> choice;

	if (choice == 1)
		blur_image = blurring(blur_image, GAUSSIAN_SIGMA);

	else if (choice == 2)
		blur_image = Bilateral_filtering(image);


	else
	{
		std::cout << "잘못 입력하셨습니다." << '\n';
		return 1;
	}

	int layer_num;
	std::cout << "레이어의 개수(브러시의 개수) 입력하기 :";
	std::cin >> layer_num;
	circle.put_layersize(layer_num);

	circle.layer_list = circle.Painterly_initialize();

	float T = 0.15;
	canvas = circle.paint_airbrush(T, saliency_output, canvas, blur_image, brush_vec, circle.layer_list, image_etf_dx, image_etf_dy);

	return 0;
}
