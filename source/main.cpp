#include <iostream>
#include <opencv2/opencv.hpp>
#include "Painterly.h"
#include <vector>


int main()
{

	stroke circle; // ������������ �׳� ����� �ٲ㺽

	cv::Mat image = cv::imread("Cat.jpg");
	int height = image.rows;
	int width = image.cols;

	cv::Mat canvas = cv::imread("Cat.jpg");
	canvas = cv::imread("Cat.jpg");
	canvas = cv::Scalar(255, 255, 255); // ĵ���� �ܻ����� �����

	cv::Mat blur_image = cv::imread("Cat.jpg");

	double g_sigma; // ����þ� �����Լ� �ñ׸�
	std::cout << "����þ� �ñ׸��� �Է��ϱ� : ";
	std::cin >> g_sigma;
	blur_image = blurring(blur_image, g_sigma);

	int layer_num;
	std::cout << "���̾��� ����(�귯���� ����) �Է��ϱ� :";
	std::cin >> layer_num;
	circle.put_layersize(layer_num);

	circle.layer_list=circle.Painterly_initialize();
	//���� ��� �Լ� �� ȿ�������� ����Ű��

	//int layer_num = circle.get_layersize();
	//���̾��� ������ŭ ����Ʈĥ �ܰ谡 �ʿ�
	int T = 100; // ����Ʈ ĥ������ ��谪
	circle.paint(T, canvas, blur_image, circle.layer_list);

	/*
	���� ��ȹ (�� ���̾�� �ݺ��� ���� ����ǵ���.
	for (int i = 0; i < layer_num; i++)
	{
		//�̹��� ��ü�� ��ȸ�ϸ� ������ ���� ����Ʈĥ
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