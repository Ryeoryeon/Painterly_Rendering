#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <opencv2/opencv.hpp>
#include "Painterly.h"
#include "yh_vector.h"
#include <vector>
#include <opencv2/saliency/saliencySpecializedClasses.hpp> // saliency�� ���� ���� �߰��ؾ� �ϴ� �������



int main()
{
	int first_choice = 0;

	std::cout << "���� �̹����� Embossing ���� : 1��" << '\n';
	std::cout << "���ο� �̹��� ���� �� Embossing ���� : ������ ��ȣ��" << '\n';

	std::cin >> first_choice;

	//������ ������ ���� �ڵ�
	if (first_choice == 1)
	{
		cv::Mat canvas;
		canvas = cv::imread("final.jpg");
		cv::Mat accum_image;
		accum_image = cv::imread("final_accum.jpg", cv::IMREAD_GRAYSCALE); // �ڿ� ���ڴ� �˾Ƽ� �����ϱ�

		embossing(canvas, accum_image, 0.2);
		return 0;
	}

	else
	{
		stroke circle;

		std::cout << "Saliency Image�� �ܰ� �̹�����, height map�� �ݿ��� �̹��� ������ ���Ͻø� 1��, ������ �����ø� �ٸ� ���ڸ� ������ �ּ���." << '\n';
		int op;
		std::cin >> op;
		circle.put_option(op);

		//cv::Mat brush = cv::imread("airbrush.png", cv::IMREAD_GRAYSCALE);
		cv::Mat brush = cv::imread("simsim.png", cv::IMREAD_GRAYSCALE);
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

		//std::string file_name = "duck_2.jpg";
		//std::string file_name = "lenna.jpg";
		std::string file_name = "Cat.jpg";
		//std::string file_name = "Cat_2.jpg";

		cv::Mat image = cv::imread(file_name);
		cv::Mat blur_image = cv::imread(file_name);

		//std::string etf_name = "duck_2.etf";
		//std::string etf_name = "lenna.etf";
		std::string etf_name = "Cat.etf";
		//std::string etf_name = "Cat_2.etf";

		const char* etf_name_char = etf_name.c_str();

		FILE* etf;
		etf = fopen(etf_name_char, "rb");

		int height = image.rows;
		int width = image.cols;

		cv::Mat canvas = cv::imread("empty_canvas_final_2.png"); // �� ���� �̹���
		//cv::Mat canvas = cv::imread("empty_canvas_final.jpg");
		//canvas = cv::Scalar(255, 255, 255); // ĵ������ ��� �ܻ��̾����� ������ �� ���

		cv::resize(canvas, canvas, cv::Size(width + 2 * MARGIN, height + 2 * MARGIN), 0, 0);


		if (etf == NULL)
		{
			exit(1);
		}

		cv::Mat saliency_output;

		cv::saliency::StaticSaliencySpectralResidual Sal;
		Sal.computeSaliency(image, saliency_output);

		//saliency �̹����� ���� �ʹٸ� ���� �����ϱ�
		if (op == 1)
		{
			cv::imshow("f", saliency_output);
			cv::waitKey(0);
		}

		float* buffer = new float[2 * width * height]; // ���ͷ� �ϴϱ� �� �ȴ�.
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


}



