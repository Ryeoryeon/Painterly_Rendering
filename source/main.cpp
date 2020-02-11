#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <opencv2/opencv.hpp>
#include "Painterly.h"
#include <vector>


int main()
{

	stroke circle;

	cv::Mat brush = cv::imread("brush_1.png", cv::IMREAD_GRAYSCALE);
	//cv::Mat brush = cv::imread("brush_2.png", cv::IMREAD_GRAYSCALE);
	//cv::Mat brush = cv::imread("brush_3.png", cv::IMREAD_GRAYSCALE);
	//cv::Mat brush = cv::imread("airbrush.png", cv::IMREAD_GRAYSCALE);
	//cv::Mat brush = cv::imread("airbrush_2.png", cv::IMREAD_GRAYSCALE);

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
	//cv::Mat image = cv::imread("gra_2.png");
	cv::Mat image = cv::imread("lenna.jpg");
	int height = image.rows;
	int width = image.cols;

	cv::Mat canvas = cv::imread("empty_canvas_2.jpg");
	//cv::Mat canvas = cv::imread("empty_canvas_3.jpg");
	//cv::Mat canvas = cv::imread("Newsprint.jpg");
	//cv::Mat canvas = cv::imread("Washi.jpg");
	//canvas = cv::Scalar(255, 255, 255); // ĵ������ ��� �ܻ��̾����� ������ ��

	cv::resize(canvas, canvas, cv::Size(width + 2*MARGIN, height + 2*MARGIN), 0, 0);

	//cv::Mat blur_image = cv::imread("landscape.jpg");
	cv::Mat blur_image = cv::imread("lenna.jpg");
	//cv::Mat blur_image = cv::imread("gra_2.png");

	FILE* etf;
	//etf = fopen("landscape.etf", "rb");
	etf = fopen("lenna_2.etf", "rb");
	//etf = fopen("gra_2.etf", "rb");

	if (etf == NULL)
	{
		exit(1);
	}

;	float* buffer = new float[2 * width * height]; // ���ͷ� �ϴϱ� �� �ȴ�.
	std::vector<std::vector<float>> image_etf_dx;
	std::vector<std::vector<float>> image_etf_dy;

	image_etf_dx.assign(width, std::vector<float>(height, 0));
	image_etf_dy.assign(width, std::vector<float>(height, 0));
	//size of float (width*height*size of float*2) // �̷��� �޸𸮸� �Ҵ��� �� ������,

	/*
	fseek(etf, 0, SEEK_END); // ������ ������ ��ġ�� �̵�
	int etf_size = ftell(etf); // ������ ��ġ ���� �޾Ƽ� ������ ũ�� ��� (lenna���� 2457600 (640 * 480 * (size of float == 4) * 2))
	fseek(etf, 0, SEEK_SET); // ������ ó�� ��ġ�� Ŀ�� �̵�
	*/

	//��Ͼ�� : ���� �迭 �ϳ��� ����. ���⿡ dx, dy ��� �����ߴµ� �ε����� ¦��, Ȧ���Ŀ� ���� dx,dy���� ����
	fread(buffer, sizeof(float), 2 * width * height, etf);

	int index = 0;

	//x�� ���ؼ� y�� �����ϴ°Ŵϱ� ���ι������� �����ϴ� ����. �� etf�� ���μ��θ� �Ųٷ� �ҷ��Դ�
/*	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			image_etf_dx[x][y] = buffer[index++];
			image_etf_dy[x][y] = buffer[index++];
		}
	}
*/
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

	/*
	double g_sigma; // ����þ� �����Լ� �ñ׸�
	std::cout << "����þ� �ñ׸��� �Է��ϱ� : ";
	std::cin >> g_sigma;
	*/
	std::cout << "���۷��� �̹��� �����ϱ�" << '\n';
	std::cout << "1�� : Gaussian blurring" << '\n';
	std::cout << "2�� : Bilateral filtering" << '\n';

	int choice;
	std::cin >> choice;
	
	if (choice == 1)
		blur_image = blurring(blur_image, GAUSSIAN_SIGMA);

	else if (choice == 2) {
		blur_image = Bilateral_filtering(image);
		cv::imshow("Bilateral", blur_image);
		cv::waitKey(0);
	}

	else
	{
		std::cout << "�߸� �Է��ϼ̽��ϴ�." << '\n';
		return 1;
	}

	/* // [HSV] HSV������ ���ؼ���� ���� Ǯ��
	cvtColor(canvas, canvas, cv::COLOR_BGR2HSV); // �� ���� ������ ���� ���� �ڵ�.
	cvtColor(blur_image, blur_image, cv::COLOR_BGR2HSV);
	*/


	int layer_num;
	std::cout << "���̾��� ����(�귯���� ����) �Է��ϱ� :";
	std::cin >> layer_num;
	circle.put_layersize(layer_num);

	circle.layer_list = circle.Painterly_initialize();
	//���� ��� �Լ� �� ȿ�������� ����Ű��

	//int layer_num = circle.get_layersize();
	//���̾��� ������ŭ ����Ʈĥ �ܰ谡 �ʿ�
	float T = 0.05;
	canvas = circle.paint(T, canvas, blur_image, brush_vec, circle.layer_list, image_etf_dx, image_etf_dy);

	// cvtColor(canvas, canvas, cv::COLOR_HSV2BGR); // [HSV] HSV������ ���ؼ���� ���� Ǯ��
	//cv::imshow("Painterly_Rendering", canvas);
	//cv::waitKey(0);

	return 0;
}
