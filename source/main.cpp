#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <opencv2/opencv.hpp>
#include "Painterly.h"
#include <vector>


int main()
{

	stroke circle;

	cv::Mat image = cv::imread("lenna.jpg");
	int height = image.rows;
	int width = image.cols;

	cv::Mat canvas = cv::imread("empty_canvas.jpg"); // ����� �� �̹��� -> ĵ���� �̹����� ����
	//canvas = cv::Scalar(255, 255, 255); // ĵ���� �ܻ����� �����

	cv::resize(canvas, canvas, cv::Size(width + 2*MARGIN, height + 2*MARGIN), 0, 0);

	cv::Mat blur_image = cv::imread("lenna.jpg");

	FILE* etf;
	etf = fopen("lenna.etf", "rb");

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

	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			image_etf_dx[x][y] = buffer[index++];
			image_etf_dy[x][y] = buffer[index++];
		}
	}

	fclose(etf);

	double g_sigma; // ����þ� �����Լ� �ñ׸�
	std::cout << "����þ� �ñ׸��� �Է��ϱ� : ";
	std::cin >> g_sigma;
	blur_image = blurring(blur_image, g_sigma);

	cvtColor(canvas, canvas, cv::COLOR_BGR2HSV); // �� ���� ������ ���� ���� �ڵ�.
	cvtColor(blur_image, blur_image, cv::COLOR_BGR2HSV);


	int layer_num;
	std::cout << "���̾��� ����(�귯���� ����) �Է��ϱ� :";
	std::cin >> layer_num;
	circle.put_layersize(layer_num);

	circle.layer_list = circle.Painterly_initialize();
	//���� ��� �Լ� �� ȿ�������� ����Ű��

	//int layer_num = circle.get_layersize();
	//���̾��� ������ŭ ����Ʈĥ �ܰ谡 �ʿ�
	float T = 0.05;
	canvas = circle.paint(T, canvas, blur_image, circle.layer_list, image_etf_dx, image_etf_dy);

	cvtColor(canvas, canvas, cv::COLOR_HSV2BGR);
	cv::imshow("Painterly_Rendering", canvas);
	cv::waitKey(0);

	return 0;
}
