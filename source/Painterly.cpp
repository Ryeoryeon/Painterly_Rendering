#include <iostream>
#include <opencv2/opencv.hpp>
#include "Painterly.h"
#include "dwLIC2.h"
#include <vector>
#include <random>


std::vector<layer> stroke::Painterly_initialize()
{
	//std::vector<layer>layer_list; (��� ���� �̰� �� �־���..?)

	for (int i = 0; i < layer_size; i++)
	{

		//layer_list.reserve(layer_size);

		int test_brush;
		double test_fg;

		std::cout << i << "��° ���̾�, �귯�� ������ �Է� : ";
		std::cin >> test_brush;
		std::cin.ignore(100, '\n');

		/*
		�׸����� ������. fg�� 1�̸� �귯���� ������ ũ�⸸ŭ.
		���� 1���� ������ ��Ʈ��ũ�� �׸��� ������ ���������� ũ�� �� �ݴ���.
		*/

		std::cout << i << "��° ���̾�, f_g ������ �Է� : (���� �۾����� �Ѵ�)";
		std::cin >> test_fg;
		std::cin.ignore(100, '\n');

		if (test_brush * test_fg < 1)
			push_back(test_brush, test_fg, 1); // �׸��� ����� 0�� �Ǵ� ���� ����.

		else
		{
			int grid = test_brush * test_fg;

			if (grid % 2 == 1)
				push_back(test_brush, test_fg, grid);

			else
			{
				int grid_to_odd = grid + 1;
				push_back(test_brush, test_fg, grid_to_odd);
			}

			//push_back(test_brush, test_fg, test_brush * test_fg);
		}
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

cv::Mat stroke::paint(float T, cv::Mat& canvas, const cv::Mat& reference, const std::vector<std::vector<int>> brush_vec, std::vector<layer>& layer_list, const std::vector<std::vector<float>>& image_etf_dx, const std::vector<std::vector<float>>& image_etf_dy)
{
	//���� ū �귯�ð� ù ��° ���̾ ����Ǵ� ���� ����

	T *= 680; //�ִ��� 180(V)+255(S)+255(H)
	int width = reference.cols;
	int height = reference.rows;

	std::default_random_engine r(0);

	//���� ū ������ ���� �� ������� ���ư��� �Ѵ�.
	for (int i = 0; i < layer_list.size(); i++)
	{

		//int temp_area_length = 2 * (layer_list[i].grid_size / 2) + 1; // ������ Ȧ���� �ǵ���. (����ü���� ��������)
		int temp_divide_two = (layer_list[i].grid_size - 1) / 2; // ¦��/2. temp_area_length���� (double)2�� ������ �Ǹ� length�� ¦���� ������ ��찡 ���� ��� �߻� ����.

		int margin_x = calculate_margin(i, width);
		int margin_y = calculate_margin(i, height);

		std::vector<cv::Point_<int>> new_stroke_list;
		std::vector<std::vector<int>> temp_area;


		for (int x = (margin_x + temp_divide_two); x < (width - margin_x - temp_divide_two); x += (int)layer_list[i].grid_size)
		{
			for (int y = (margin_y + temp_divide_two); y < (height - margin_y - temp_divide_two); y += (int)layer_list[i].grid_size)
			{
				//�ٹ��� ���� ���ϱ� (x,y)
				double area_error = 0;

				//�ӽ� ������ ���� ���� ���� �Ҵ�
				temp_area.assign(layer_list[i].grid_size, std::vector<int>(layer_list[i].grid_size, 0));

				int RGB_diff = 0; // ���� ��ü�� ���� canvas���� reference image�� ���� RGB ������ ���� ���� �� ���̴� �ε���
				int RGB_index = 0; // �׸����� �� ĭ�� ���� R,G,B���� ���� ���̰��� ���� �ε���

				//�׸��尡 ��ĭ�� ��
				if (layer_list[i].grid_size <= 1) // if�� �����Ǹ� ������ �׸��� ����� 1.
				{

					for (int i = 0; i < 3; i++)
					{
						int canvas_color = canvas.at<cv::Vec3b>(y, x)[i];
						int reference_color = reference.at<cv::Vec3b>(y, x)[i];
						RGB_index += difference(canvas_color, reference_color);
					}

					area_error = RGB_index;

				}

				//�׸��尡 ��ĭ�� �ƴ϶� �� �̻��� ���̷� ������ ���
				else
				{
					int index_x = x - temp_divide_two;
					int index_y = y - temp_divide_two;

					//�ӽ� ���� ��ü�� ���� ĵ������ �� �̹����� ���� R,G,B�� ���̸� ���ؾ� �Ѵ�. (x,y�� �׸����� ������ ���� ����)
					for (int j = x - (temp_divide_two); j <= x + (temp_divide_two); j++) // ��ȣ ���̴°� �´µ�
					{
						for (int k = y - (temp_divide_two); k <= y + (temp_divide_two); k++)
						{
							RGB_index = 0;

							for (int l = 0; l < 3; l++) // R,G,B�� ��� �� ���̹Ƿ�
							{
								int canvas_color = canvas.at<cv::Vec3b>(k, j)[l];
								int reference_color = reference.at<cv::Vec3b>(k, j)[l];
								RGB_index += difference(canvas_color, reference_color);
							}

							temp_area[j - (index_x)][k - (index_y)] = RGB_index;
							RGB_diff += RGB_index;
						}
					}

					area_error = (RGB_diff / (pow(layer_list[i].grid_size, 2))); // �ܼ��� ���̾��� grid_size^2�� ���� �ȵɰͰ���.
				}

				if (area_error <= T)
				{
					continue;
				}

				else
				{
					//�ӽ� ���� ��ü�� ���� ���� ū ������ ���ؾ� �Ѵ�.

					if (layer_list[i].grid_size == 1)
					{
						cv::Point_<int>biggest_error(x, y);
						new_stroke_list.push_back(biggest_error);
					}

					else
					{
						cv::Point_<int>biggest_error(0, 0);
						for (int j = 0; j < temp_area.size(); j++)
						{
							for (int k = 0; k < temp_area.size(); k++)
							{
								if (j == 0 && k == 0)
									continue;

								else if (temp_area[biggest_error.y][biggest_error.x] < temp_area[j][k])
								{
									biggest_error.y = j;
									biggest_error.x = k;
								}
							}
						}

						//�׸��忡�� ���� ū ������ ã������ ���� x,y��ġ�� ȯ�� �� ��� �Ѵ�.
						//x,y�� �׸��忡�� �����ӿ� ����!! temp_divide_two�� ���͸� ������ �� ���� ���̸� /2 �� ���ӿ� ����!
						//�׷��� �迭�� 0���� �����ϴϱ� �׳� temp_divide_two�� +1 �� ���൵ �� �� ����.

						biggest_error.x = x + (biggest_error.x - (temp_divide_two));
						biggest_error.y = y + (biggest_error.y - temp_divide_two);

						/*
						//������ ���̽� ������ �����ؾ� �ϳ� �����ϰ� �־���.
						if (biggest_error.x > temp_divide_two)
						{
							biggest_error.x = (biggest_error.x - temp_divide_two);
						}
						*/

						new_stroke_list.push_back(biggest_error);
					}

				}
			}

		}


		//���̾� �ϳ� ���� �� ���� new_stroke_list�� ����� ��Ʈ��ũ���� ������ ������� ĥ�ϱ�
		//�ϴ� ������ ���� ������ �̰͵� �ؾ���!!

		//������ ����Ʈ �۾�
		for (int u = 0; u < new_stroke_list.size(); u++)
		{
			//new_stroke_list�� ����� ���� ���������� �����ϰ� �ڵ带 �ۼ�����.
			int ref_color_B = reference.at<cv::Vec3b>(new_stroke_list[u].y, new_stroke_list[u].x)[0];
			int ref_color_G = reference.at<cv::Vec3b>(new_stroke_list[u].y, new_stroke_list[u].x)[1];
			int ref_color_R = reference.at<cv::Vec3b>(new_stroke_list[u].y, new_stroke_list[u].x)[2];


			//�׸��� ������� �귯�� ����� ũ�ų� ���� ���� �� �ѹ��� ��������.
			if (layer_list[i].brush_size >= layer_list[i].grid_size)
			{

				//������ ���� �߰��� �ڵ�

				int alpha_B = ((1-ALPHA) * canvas.at<cv::Vec3b>(MARGIN + new_stroke_list[u].y, MARGIN + new_stroke_list[u].x)[0]) + (ALPHA * ref_color_B);
				int alpha_G = ((1-ALPHA) * canvas.at<cv::Vec3b>(MARGIN + new_stroke_list[u].y, MARGIN + new_stroke_list[u].x)[1]) + (ALPHA * ref_color_G);
				int alpha_R = ((1-ALPHA) * canvas.at<cv::Vec3b>(MARGIN + new_stroke_list[u].y, MARGIN + new_stroke_list[u].x)[2]) + (ALPHA * ref_color_R);

				float alpha_H, alpha_S, alpha_V;

				//HSV ��ȯ �Լ�
				dw_RGB2HSV(alpha_R, alpha_G, alpha_B, alpha_H, alpha_S, alpha_V);

				// [HSV] (H,S,V�� ������ �ְ� �ʹٸ� �� ���ָ� Ǯ�� ���� �Լ�, imwrite�� cvtcolor���� Ǯ��)
				alpha_H = random_alpha_H(alpha_H);
				alpha_S = random_alpha_S(alpha_S);
				alpha_V = random_alpha_V(alpha_V);

				//RGB ��ȯ �Լ�
				dw_HSV2RGB(alpha_H, alpha_S, alpha_V, alpha_R, alpha_G, alpha_B);

				if (alpha_R > 255)
					alpha_R = 255;

				if (alpha_G > 255)
					alpha_G = 255;

				if (alpha_B > 255)
					alpha_B = 255;

				if (alpha_R < 0)
					alpha_R = 0;

				if (alpha_G < 0)
					alpha_G = 0;

				if (alpha_B < 0)
					alpha_B = 0;

				// ���� �ƴ� ��¥ �귯�÷� ����ֱ� ���� �۾�

				int b_w = 100; // �ӽ�

				for (int j = 0; j < 2 * layer_list[i].brush_size; j++)
				{
					for (int k = 0; k < 2 * layer_list[i].brush_size; k++)
					{
						//�ϴ� new_stroke_list�� ��ǥ�� ���� �߽��� �ǰ���.
						//�׷��� j,k�� 0���� �����ϰ����� ������ ��������ŭ ����, ���� �̵������� �Ŀ� ���� ���� �� �� ����.
						float index_x = j / (2.0 * layer_list[i].brush_size);
						float index_y = k / (2.0 * layer_list[i].brush_size);

						if (brush_vec[b_w * index_x][b_w * index_y] != 255)
						{
							int paint_x = MARGIN + new_stroke_list[u].x + (j - (layer_list[i].brush_size));
							int paint_y = MARGIN + new_stroke_list[u].y + (k - (layer_list[i].brush_size));

							if (paint_x >= canvas.cols || paint_x < 0 || paint_y < 0 || paint_y >= canvas.rows)
								continue;

							float brush_alpha = 1 - (brush_vec[b_w * index_x][b_w * index_y] / 255.f); // //1-�귯�� ������ �ȼ� ���/255
							canvas.at<cv::Vec3b>(paint_y, paint_x)[0] = alpha_B * brush_alpha;
							canvas.at<cv::Vec3b>(paint_y, paint_x)[1] = alpha_G * brush_alpha;
							canvas.at<cv::Vec3b>(paint_y, paint_x)[2] = alpha_R * brush_alpha;
						}

					}

				}


				//cv::circle(canvas, cv::Point(new_stroke_list[u].x + MARGIN, new_stroke_list[u].y + MARGIN), layer_list[i].brush_size, cv::Scalar(alpha_H, alpha_S, alpha_V), -1);
			}

			else
			{

				cv::Point_<float> lastDxDy(0, 0); // ���� dx, dy
				cv::Point_<float> present(new_stroke_list[u].x, new_stroke_list[u].y); // ���� x,y�� ��ġ
				cv::Point_<float> DxDy(image_etf_dx[int(present.x + 0.5f)][int(present.y + 0.5)], image_etf_dy[int(present.x + 0.5)][int(present.y + 0.5)]); // ���� x,y�� dx,dy
				//0.5�� �ݿø�

				for (int k = 0; k < (layer_list[i].grid_size / layer_list[i].brush_size) * STROKE_NUMBER; k++) //+1�� �ܼ��� ���� �� (���� ����)
				{
					/*
					float temp = 0; // 90�� ��ȯ�� ���ؼ�
					temp = DxDy.x;
					DxDy.x = -DxDy.y;
					DxDy.y = temp;
					*/

					int canvas_diff = 0; // ���۷��� - ĵ������ �� ���� ���� ����
					int stroke_diff = 0; // ���۷��� - ���� ��Ʈ��ũ�� �� ���� ���� ����

					for (int l = 0; l < 3; l++)
					{
						canvas_diff += difference(canvas.at<cv::Vec3b>(int(present.y + 0.5), int(present.x + 0.5))[l], reference.at<cv::Vec3b>(int(present.y + 0.5), int(present.x + 0.5))[l]);
					}

					stroke_diff += difference(ref_color_B, reference.at<cv::Vec3b>(int(present.y + 0.5), int(present.x + 0.5))[0]);
					stroke_diff += difference(ref_color_G, reference.at<cv::Vec3b>(int(present.y + 0.5), int(present.x + 0.5))[1]);
					stroke_diff += difference(ref_color_R, reference.at<cv::Vec3b>(int(present.y + 0.5), int(present.x + 0.5))[2]);
					//stroke_diff += difference(ref_color_R, reference.at<cv::Vec3b>(present.y, present.x)[2]);

					if (canvas_diff < stroke_diff) // ���۷���-ĵ���� < ���۷���-���� ��Ʈ��ũ�� �����̸� ����.
						break;

					if (DxDy.x == 0 && DxDy.y == 0)
						break;

					if (lastDxDy.x * DxDy.x + lastDxDy.y * DxDy.y < 0)
					{
						DxDy.x *= -1;
						DxDy.y *= -1;

					}

					if (!(getFlowVectorRK4(width, height, present.x, present.y, DxDy.x, DxDy.y, image_etf_dx, image_etf_dy))) // �°� �� �� �³�..?
						break;

					int alpha_B = ((1-ALPHA) * canvas.at<cv::Vec3b>(MARGIN + int(present.y + 0.5), MARGIN + int(present.x + 0.5))[0]) + (ALPHA * ref_color_B);
					int alpha_G = ((1-ALPHA) * canvas.at<cv::Vec3b>(MARGIN + int(present.y + 0.5), MARGIN + int(present.x + 0.5))[1]) + (ALPHA * ref_color_G);
					int alpha_R = ((1-ALPHA) * canvas.at<cv::Vec3b>(MARGIN + int(present.y + 0.5), MARGIN + int(present.x + 0.5))[2]) + (ALPHA * ref_color_R);

					float alpha_H, alpha_S, alpha_V;

					//HSV ��ȯ �Լ�
					dw_RGB2HSV(alpha_R, alpha_G, alpha_B, alpha_H, alpha_S, alpha_V);

					// [HSV] (H,S,V�� ������ �ְ� �ʹٸ� �� ���ָ� Ǯ�� ���� �Լ�, imwrite�� cvtcolor���� Ǯ��)
					alpha_H = random_alpha_H(alpha_H);
					alpha_S = random_alpha_S(alpha_S);
					alpha_V = random_alpha_V(alpha_V);

					//RGB ��ȯ �Լ�
					dw_HSV2RGB(alpha_H, alpha_S, alpha_V, alpha_R, alpha_G, alpha_B);

					// B,G,R���� ���̰� �ϰ� �ʹٸ� ����ó���� �Ʒ��� ����

					if (alpha_R > 255)
						alpha_R = 255;

					if (alpha_G > 255)
						alpha_G = 255;

					if (alpha_B > 255)
						alpha_B = 255;

					if (alpha_R < 0)
						alpha_R = 0;

					if (alpha_G < 0)
						alpha_G = 0;

					if (alpha_B < 0)
						alpha_B = 0;


					int b_w = 100; // �ӽ�

					for (int j = 0; j < 2 * layer_list[i].brush_size; j++)
					{
						for (int k = 0; k < 2 * layer_list[i].brush_size; k++)
						{
							//�ϴ� new_stroke_list�� ��ǥ�� ���� �߽��� �ǰ���.
							//�׷��� j,k�� 0���� �����ϰ����� ������ ��������ŭ ����, ���� �̵������� �Ŀ� ���� ���� �� �� ����.
							float index_x = j / (2.0 * layer_list[i].brush_size);
							float index_y = k / (2.0 * layer_list[i].brush_size);

							if (brush_vec[b_w * index_x][b_w * index_y] != 255)
							{
								int paint_x = MARGIN + present.x + (j - (layer_list[i].brush_size));
								int paint_y = MARGIN + present.y + (k - (layer_list[i].brush_size));

								if (paint_x >= canvas.cols || paint_x < 0 || paint_y < 0 || paint_y >= canvas.rows)
									continue;

								float brush_alpha = 1 - (brush_vec[b_w * index_x][b_w * index_y] / 255.f); // //1-�귯�� ������ �ȼ� ���/255
								canvas.at<cv::Vec3b>(paint_y, paint_x)[0] = alpha_B * brush_alpha;
								canvas.at<cv::Vec3b>(paint_y, paint_x)[1] = alpha_G * brush_alpha;
								canvas.at<cv::Vec3b>(paint_y, paint_x)[2] = alpha_R * brush_alpha;
							}

						}

					}

					//cv::circle(canvas, cv::Point(present.x + MARGIN, present.y + MARGIN), layer_list[i].brush_size, cv::Scalar(alpha_H, alpha_S, alpha_V), -1);

					/*
					dwLIC2 testdw;
					testdw.setFlowField(present.x, present.y, DxDy.x, DxDy.y);

					if (!(testdw.getFlowVectorRK4(present.x, present.y, DxDy.x, DxDy.y)))
						break;
					*/

					lastDxDy.x = DxDy.x;
					lastDxDy.y = DxDy.y;

					present.x += DxDy.x;
					present.y += DxDy.y;

					if (int(present.x + 0.5) >= width || int(present.y + 0.5) >= height)
						break;

					else if (int(present.x + 0.5) < 0 || int(present.y + 0.5) < 0)
						break;

					DxDy.x = image_etf_dx[int(present.x + 0.5)][int(present.y + 0.5)];
					DxDy.y = image_etf_dy[int(present.x + 0.5)][int(present.y + 0.5)];

				}
			}

		}

		cv::Mat canvas_BGR;

		if (i == 0)
		{
			cv::cvtColor(canvas, canvas_BGR, cv::COLOR_HSV2BGR);
			cv::imwrite("0.jpg", canvas); // [HSV] HSV�� convert ���� �ʰ� �߰������� ������ ��
			//cv::imwrite("0.jpg", canvas_BGR); // HSV�� convert�ϰ� �߰������� ������ ��
		}

		else if (i == 1)
		{
			cv::cvtColor(canvas, canvas_BGR, cv::COLOR_HSV2BGR);
			cv::imwrite("1.jpg", canvas);
			//cv::imwrite("1.jpg", canvas_BGR);
		}

		else if (i == 2)
		{
			cv::cvtColor(canvas, canvas_BGR, cv::COLOR_HSV2BGR);
			cv::imwrite("2.jpg", canvas);
			//cv::imwrite("2.jpg", canvas_BGR);
		}

		else if (i == 3)
		{
			cv::cvtColor(canvas, canvas_BGR, cv::COLOR_HSV2BGR);
			cv::imwrite("3.jpg", canvas);
			//cv::imwrite("3.jpg", canvas_BGR);
		}


	}

	return canvas;
}

