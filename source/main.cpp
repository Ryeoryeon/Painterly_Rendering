#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <opencv2/opencv.hpp>
#include "Painterly.h"
#include "yh_vector.h"
#include <vector>
#include <opencv2/saliency/saliencySpecializedClasses.hpp> // saliency�� ���� ���� �߰��ؾ� �ϴ� �������


int main()
{
	stroke circle;

	std::cout << "Saliency Image�� �ܰ� �̹�����, height map�� �ݿ��� �̹��� ������ ���Ͻø� 1��, ������ �����ø� �ٸ� ���ڸ� ������ �ּ���." << '\n';
	int op;
	std::cin >> op;
	circle.put_option(op); 

	//cv::Mat brush = cv::imread("brush_1.png", cv::IMREAD_GRAYSCALE);
	//cv::Mat brush = cv::imread("brush_2.png", cv::IMREAD_GRAYSCALE);
	//cv::Mat brush = cv::imread("brush_3.png", cv::IMREAD_GRAYSCALE);
	cv::Mat brush = cv::imread("airbrush.png", cv::IMREAD_GRAYSCALE);
	//cv::Mat brush = cv::imread("simsim.png", cv::IMREAD_GRAYSCALE);
	//cv::Mat brush = cv::imread("simsim_2.jpg", cv::IMREAD_GRAYSCALE);
	//cv::Mat brush = cv::imread("airbrush_3.png", cv::IMREAD_GRAYSCALE);
	//cv::Mat brush = cv::imread("rough_brush.png", cv::IMREAD_GRAYSCALE);

	int b_w = brush.cols;

	std::vector<std::vector<int>> brush_vec;
	brush_vec.assign(b_w, std::vector<int>(b_w, 0));

	//�귯���� ��� �����ϱ�
	for (int x = 0; x < b_w; x++)
	{
		for (int y = 0; y < b_w; y++)
		{
			brush_vec[x][y] = brush.at<uchar>(y, x);
		}
	}


	//cv::Mat image = cv::imread("landscape.jpg");
	//cv::Mat image = cv::imread("duck.jpg");
	//cv::Mat image = cv::imread("duck_2.jpg");
	//cv::Mat image = cv::imread("lenna.jpg");
	//cv::Mat image = cv::imread("lenna1_crop.jpg");
	//cv::Mat image = cv::imread("Cat.jpg");
	cv::Mat image = cv::imread("Cat_2.jpg");
	//cv::Mat image = cv::imread("Sky.jpg");
	int height = image.rows;
	int width = image.cols;

	cv::Mat canvas = cv::imread("empty_canvas_final.png");
	//cv::Mat canvas = cv::imread("empty_canvas_2.jpg");
	//cv::Mat canvas = cv::imread("Newsprint.jpg");
	//cv::Mat canvas = cv::imread("Washi.jpg");
	//canvas = cv::Scalar(255, 255, 255); // ĵ������ ��� �ܻ��̾����� ������ ��

	cv::resize(canvas, canvas, cv::Size(width + 2*MARGIN, height + 2*MARGIN), 0, 0);

	//cv::Mat blur_image = cv::imread("landscape.jpg");
	//cv::Mat blur_image = cv::imread("duck.jpg");
	//cv::Mat blur_image = cv::imread("duck_2.jpg");
	// cv::Mat blur_image = cv::imread("lenna.jpg");
	//cv::Mat blur_image = cv::imread("lenna1_crop.jpg");
	//cv::Mat blur_image = cv::imread("Cat.jpg");
	cv::Mat blur_image = cv::imread("Cat_2.jpg");
	//cv::Mat blur_image = cv::imread("Sky.jpg");

	FILE* etf;
	//etf = fopen("landscape.etf", "rb");
	//etf = fopen("duck.etf", "rb");
	//etf = fopen("duck_2.etf", "rb");
	//etf = fopen("lenna_2.etf", "rb");
	//etf = fopen("lenna1_crop.etf", "rb");
	//etf = fopen("Cat.etf", "rb");
	etf = fopen("Cat_2.etf", "rb");
	//etf = fopen("Sky.etf", "rb");

	if (etf == NULL)
	{
		exit(1);
	}

	cv::Mat saliency_output;

	cv::saliency::StaticSaliencySpectralResidual Sal;
	Sal.computeSaliency(image, saliency_output);

	//saliency �̹����� ���� �ʹٸ� ���� �����ϱ�
	if ( op == 1)
	{
		//cv::imshow("f", saliency_output);
		//cv::waitKey(0);  
	}

;	float* buffer = new float[2 * width * height]; // ���ͷ� �ϴϱ� �� �ȴ�.
	std::vector<std::vector<float>> image_etf_dx;
	std::vector<std::vector<float>> image_etf_dy;

	image_etf_dx.assign(width, std::vector<float>(height, 0));
	image_etf_dy.assign(width, std::vector<float>(height, 0));
	//size of float (width*height*size of float*2) // �̷��� �޸𸮸� �Ҵ��� �� ������,

	//��Ͼ�� : ���� �迭 �ϳ��� ����. ���⿡ dx, dy ��� �����ߴµ� �ε����� ¦��, Ȧ���Ŀ� ���� dx,dy���� ����
	fread(buffer, sizeof(float), 2 * width * height, etf);

	int index = 0;

	//y�� ���ؼ� x�� �����ϴ°Ŵϱ� �̰� ���ι������� �����ϴ� ����. etf�� ���η� �� ���̴°ű⶧����..
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			image_etf_dx[x][y] = buffer[index++];
			image_etf_dy[x][y] = buffer[index++];
		}
	}

	fclose(etf);

	std::cout << "���۷��� �̹��� �����ϱ�" << '\n';
	std::cout << "1�� : Gaussian blurring" << '\n';
	std::cout << "2�� : Bilateral filtering" << '\n';

	int choice;
	std::cin >> choice;

	if (choice == 1)
		blur_image = blurring(blur_image, GAUSSIAN_SIGMA);

	else if (choice == 2)
		blur_image = Bilateral_filtering(image);


	else
	{
		std::cout << "�߸� �Է��ϼ̽��ϴ�." << '\n';
		return 1;
	}

	int layer_num;
	std::cout << "���̾��� ����(�귯���� ����) �Է��ϱ� :";
	std::cin >> layer_num;
	circle.put_layersize(layer_num);

	circle.layer_list = circle.Painterly_initialize();

	float T = 0.1;
	canvas = circle.paint_airbrush(T, saliency_output, canvas, blur_image, brush_vec, circle.layer_list, image_etf_dx, image_etf_dy);

	return 0;
}

//������ ������ ���� �ڵ�
/*
int main()
{
	cv::Mat canvas;
	canvas = cv::imread("6.jpg");
	cv::Mat accum_image;
	accum_image = cv::imread("accumulate 6.jpg", cv::IMREAD_GRAYSCALE); // �ڿ� ���ڴ� �˾Ƽ� �����ϱ�

	embossing(canvas, accum_image, 0.5);
	return 0;
}
*/



