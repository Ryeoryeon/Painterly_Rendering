#include <iostream>
#include <opencv2/opencv.hpp>
#include "Painterly.h"
#include <vector>
#include <random>


std::vector<layer> stroke::Painterly_initialize()
{
	//std::vector<layer>layer_list; (��� ���� �̰� �� �־���..?)

	/*
	int brush_size;
	int f_g;
	*/

	for (int i = 0; i < layer_size; i++)
	{

		//layer_list.reserve(layer_size);

		int test_brush;
		double test_fg;

		std::cout << i << "��° ���̾�, �귯�� ������ �Է� : ";
		std::cin >> test_brush;
		//layer_list[i].brush_size = test_brush; // ������ �Ϸ��� �ϸ� �ȵȴ�..����?
		//put_brushsize(i, test_brush);
		std::cin.ignore(100, '\n');

		/*
		�׸����� ������. fg�� 1�̸� �귯���� ������ ũ�⸸ŭ.
		���� 1���� ������ ��Ʈ��ũ�� �׸��� ������ ���������� ũ�� �� �ݴ���.
		*/

		std::cout << i << "��° ���̾�, f_g ������ �Է� : (���� �۾����� �Ѵ�)";
		std::cin >> test_fg;
		//put_fg(i, test_fg);
		//std::cin >> layer_list[i].f_g;
		std::cin.ignore(100, '\n');

		//layer_list[i].grid_size = layer_list[i].brush_size * layer_list[i].f_g;

		push_back(test_brush, test_fg, test_brush*test_fg);
	}

	//������ �Ѳ����� ����ü ���͸� push_back���� �޾��ַ��� ������ ���۹��� ������ ����
	return layer_list;
}

int stroke::calculate_margin(int layer, int length)
{
	if (layer_list[layer].grid_size == 0) // �׳� ����ó�� ����.
		layer_list[layer].grid_size = 1;

	int grid_number = length / layer_list[layer].grid_size; // �׸����� ����
	int index = (length - (grid_number * (int)layer_list[layer].grid_size)) / 2;
	return index;
}

void stroke::paint(int T, cv::Mat canvas, const cv::Mat reference, std::vector<layer> layer_list)
{
	//���� ū! �귯�ú��� �����ؾ��� (�Ƹ���?)
	//���� ���� ū �귯�ð� ù ��° ���̾ ����Ǵ� ���� ������ �ڵ带 ®��
	//�Ŀ� ������ ����

		//���̾� 0�� ���
		//�귯�� ������ 3 f_g�� 1.5�� �Է��� �� �׸��� ������� 4.5

	int width = canvas.cols;
	int height = canvas.rows;

	std::default_random_engine r(0);

	//���� ū ������ ���� �� ������� ���ư��� �Ѵ�.
	for (int i = 0; i < layer_list.size(); i++)
	{

		int margin_x = calculate_margin(i, width);
		int margin_y = calculate_margin(i, height);

		std::vector<cv::Point_<int>> new_stroke_list;
		std::vector<std::vector<int>> temp_area;

		for (int x = margin_x; x < width - margin_x; x += (int)layer_list[i].grid_size)
		{
			for (int y = margin_y; y < height - margin_y; y += (int)layer_list[i].grid_size)
			{
				//�ٹ��� ���� ���ϱ� (x,y)
				double area_error = 0;

				//�ӽ� ������ ���� ���� ���� �Ҵ�
				//���͸� �������� grid_size/2��ŭ�� ���̾� �����ϱ�. (���� grid_size�� Ȧ���� ��������� �ϴϱ� ������ ��)
				//�ӽ� ������ ���� ũ�⸦ �׳� grid*grid�� ������ != (���� �����ִ´��)���͸� �������� ������ ������

				int temp_area_length = 2 * (layer_list[i].grid_size / (double)2) + 1; // ������ Ȧ��, �⺻����ζ�� 5

				temp_area.assign(temp_area_length, std::vector<int>(temp_area_length, 0));
				int RGB_diff = 0;

				if (temp_area_length <= 1) // �׷��� ������ 1�̰���. @+1�̴ϱ�.
				{
					//grid_size/(double)2�� 0�� �� �����ϴ�.
					//�� ���� ��� ����ó���� ����?
					//��·�� �ӽ� ������ ��ĭ�̶�� �Ŵϱ� �� ĭ�� ���ؼ��� ���� ���� �ϸ� �Ǵ� �� �ƴұ�?

					for (int i = 0; i < 3; i++)
					{
						int canvas_color = canvas.at<cv::Vec3b>(y, x)[i];
						int reference_color = reference.at<cv::Vec3b>(y, x)[i];
						RGB_diff += difference(canvas_color, reference_color);
					}

					area_error = RGB_diff;
				}

				else
				{
					int temp_divide_two = (temp_area_length - 1) / 2; // ���� ������ ¦��, �׳� �̷��� �ص� �ɵ�.

					//�ӽ� ���� ��ü�� ���� ĵ������ �� �̹����� ���� R,G,B�� ���̸� ���ؾ� �Ѵ�.
					for (int j = x - (temp_divide_two); j <= x + (temp_divide_two); j++) // ��ȣ ���̴°� �´µ�
					{

						for (int k = y - (temp_divide_two); k <= y + (temp_divide_two); k++)
						{
							for (int l = 0; l < 3; l++) // R,G,B�� ��� �� ���̹Ƿ�
							{
								int canvas_color = canvas.at<cv::Vec3b>(y, x)[l];
								int reference_color = reference.at<cv::Vec3b>(y, x)[l];
								RGB_diff += difference(canvas_color, reference_color);
							}

							temp_area[j+temp_divide_two][k+temp_divide_two] += RGB_diff;

						}
					}

					area_error += (RGB_diff / (pow(temp_area_length, 2))); // �ܼ��� grid_size^�� ���� �ȵɰͰ���
				}

				if (area_error <= T)
				{
					continue;
				}

				else
				{
					//�ӽ� ���� ��ü�� ���� ���� ū ������ ���ؾ� �Ѵ�.

					cv::Point_<int>biggest_error (0,0);
					//std::cout << temp_area.size() << '\n';
					for (int j = 0; j < temp_area.size(); j++)
					{
						for (int k = 0; k < temp_area.size(); k++)
						{
							if (j == 0 && k == 0)
								continue;

							else if (temp_area[biggest_error.x][biggest_error.y] < temp_area[j][k])
							{
								biggest_error.x = j;
								biggest_error.y = k;
							}

						}
					}

					/* (�����غ��� �迭�� �ε����� ������ �ǳ�?)
					for (int j = x - (temp_divide_two); j < x + (temp_divide_two); j++)
					{
						for (int k = y - (temp_divide_two); k < y + (temp_divide_two); k++)
						{
							if (j == x - (temp_divide_two) && k == y - (temp_divide_two))
							{
								continue;
							}

							else if (temp_area[j][k])
							{
								biggest_error.x = j;
								biggest_error.y = k;
							}

						}
					}
					*/


					new_stroke_list.push_back(biggest_error);

				}
			}
		}

		//���̾� �ϳ� ���� �� ���� new_stroke_list�� ����� ��Ʈ��ũ���� ������ ������� ĥ�ϱ�
		//�ϴ� ������ ���� ������ �̰͵� �ؾ���!!

		std::cout << i + 1 << "��° ���̾� ��Ʈ��ũ ���� �Ϸ�" << '\n';

		for (int u = 0; u < new_stroke_list.size(); u++)
		{
			int ref_color_B = reference.at<cv::Vec3b>(new_stroke_list[u].x, new_stroke_list[u].y)[0];
			int ref_color_G = reference.at<cv::Vec3b>(new_stroke_list[u].x, new_stroke_list[u].y)[1];
			int ref_color_R = reference.at<cv::Vec3b>(new_stroke_list[u].x, new_stroke_list[u].y)[2];

			cv::circle(canvas, new_stroke_list[u], layer_list[i].brush_size, (ref_color_B, ref_color_G, ref_color_R), -1); // �β��� -1�� �Է��ϸ� �� ä���
		}

		std::cout << i + 1 << "��° ���̾�ĥ�ϱ���� �Ϸ�" << '\n';

	}
	
}