#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <opencv2/opencv.hpp>
#include "Painterly.h"
#include "yh_vector.h"
#include <vector>
#include <opencv2/saliency/saliencySpecializedClasses.hpp> // saliency를 쓰기 위해 추가해야 하는 헤더파일


int main()
{
	int first_choice = 0;

	std::cout << "기존 이미지에 Embossing 적용 : 1번" << '\n';
	std::cout << "새로운 이미지 생성 후 Embossing 적용 : 나머지 번호들" << '\n';

	std::cin >> first_choice;

	//엠버싱 디버깅용 빠른 코드
	if (first_choice == 1)
	{
		cv::Mat canvas;
		canvas = cv::imread("6.jpg");
		cv::Mat accum_image;
		accum_image = cv::imread("accumulate 6.jpg", cv::IMREAD_GRAYSCALE); // 뒤에 숫자는 알아서 조절하기

		embossing(canvas, accum_image, 0.15);
		return 0;
	}

	else
	{
		stroke circle;

		std::cout << "Saliency Image와 단계 이미지들, height map이 반영된 이미지 저장을 원하시면 1을, 원하지 않으시면 다른 숫자를 선택해 주세요." << '\n';
		int op;
		std::cin >> op;
		circle.put_option(op);

		//cv::Mat brush = cv::imread("airbrush.png", cv::IMREAD_GRAYSCALE);
		//cv::Mat brush = cv::imread("simsim.png", cv::IMREAD_GRAYSCALE);
		//cv::Mat brush = cv::imread("new_3.png", cv::IMREAD_GRAYSCALE);
		//cv::Mat brush = cv::imread("simsim_2.jpg", cv::IMREAD_GRAYSCALE);
		//cv::Mat brush = cv::imread("airbrush_3.png", cv::IMREAD_GRAYSCALE);
		//cv::Mat brush = cv::imread("rough_brush.png", cv::IMREAD_GRAYSCALE);
		//cv::Mat brush = cv::imread("nonalpha.png", cv::IMREAD_GRAYSCALE);
		//cv::Mat brush = cv::imread("gradient_2.png", cv::IMREAD_GRAYSCALE);
		cv::Mat brush = cv::imread("watercolor.png", cv::IMREAD_GRAYSCALE);

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

		//std::string file_name = "white_rabbit.jpg";
		//std::string file_name = "duck_2.jpg";
		//std::string file_name = "woman_2.jpg";
		std::string file_name = "ballet_2.jpg";
		//std::string file_name = "Cat.jpg";
		//std::string file_name = "Cat_2.jpg";

		cv::Mat image = cv::imread(file_name);
		cv::Mat blur_image = cv::imread(file_name);

		//std::string etf_name = "white_rabbit.etf";
		//std::string etf_name = "duck_2.etf";
		//std::string etf_name = "woman_2.etf";
		std::string etf_name = "ballet_2.etf";
		//std::string etf_name = "Cat.etf";
		//std::string etf_name = "Cat_2.etf";

		const char* etf_name_char = etf_name.c_str();

		FILE* etf;
		etf = fopen(etf_name_char, "rb");

		int height = image.rows;
		int width = image.cols;

		cv::Mat canvas = cv::imread("empty_canvas_final_2.png"); // 더 밝은 이미지
		//cv::Mat canvas = cv::imread("empty_canvas_final.jpg");
		//canvas = cv::Scalar(255, 255, 255); // 캔버스가 흰색 단색이었으면 좋겠을 때 사용

		cv::resize(canvas, canvas, cv::Size(width + 2 * MARGIN, height + 2 * MARGIN), 0, 0);


		if (etf == NULL)
		{
			exit(1);
		}

		cv::Mat saliency_output;

		cv::saliency::StaticSaliencySpectralResidual Sal;
		Sal.computeSaliency(image, saliency_output);

		if (op == 1)
		{
			save_saliency(saliency_output);
		}

		float* buffer = new float[2 * width * height]; // 벡터로 하니까 안 된다.
		std::vector<std::vector<float>> image_etf_dx;
		std::vector<std::vector<float>> image_etf_dy;

		image_etf_dx.assign(width, std::vector<float>(height, 0));
		image_etf_dy.assign(width, std::vector<float>(height, 0));
		//size of float (width*height*size of float*2) // 이렇게 메모리를 할당을 한 다음에,

		//재완언니 : 단일 배열 하나만 넣음. 여기에 dx, dy 모두 저장했는데 인덱스가 짝수, 홀수냐에 따라 dx,dy저장 가능
		fread(buffer, sizeof(float), 2 * width * height, etf);

		int index = 0;

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
		std::cout << "블러링 하고 싶지 않으시면 다른 번호를 선택해 주세요." << '\n';

		int choice;
		std::cin >> choice;

		if (choice == 1)
			blur_image = blurring(blur_image, GAUSSIAN_SIGMA);

		else if (choice == 2)
			blur_image = Bilateral_filtering(image);

		cv::imwrite("20200323_b.png", blur_image);

		int layer_num;
		std::cout << "레이어의 개수(브러시의 개수) 입력하기 :";
		std::cin >> layer_num;
		circle.put_layersize(layer_num);

		circle.layer_list = circle.Painterly_initialize();

		float T = 0.2;
		canvas = circle.paint_airbrush(T, saliency_output, canvas, blur_image, brush_vec, circle.layer_list, image_etf_dx, image_etf_dy);

		return 0;
	}


}



