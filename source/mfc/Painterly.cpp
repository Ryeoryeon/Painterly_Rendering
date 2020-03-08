#include "pch.h"
#include "yh_vector.h"
#include "dwLIC2.h"
#include <random>

#include "targetver.h"
#include "framework.h"
#include "Painterly_Rendering_mfc.h"
#include "resource.h"
#include "yh_dialog_3.h"


std::vector<layer> stroke::Painterly_initialize()
{

	yh_dialog_3 dlg_3;

	if (IDOK == dlg_3.DoModal())
	{
		std::vector<int> dlg_brush = dlg_3.push_back_brush_size(layer_size);
		std::vector<float> dlg_f_g = dlg_3.push_back_f_g_size(layer_size);

		for (int i = 0; i < layer_size; i++)
		{
			int test_brush;
			double test_fg;

			test_brush = dlg_brush[i];
			test_fg = dlg_f_g[i];

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
	}

	else
	{
		std::cout << "����!" << '\n';
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

//����귯�� O
cv::Mat stroke::paint_airbrush(float T, const cv::Mat& saliency_output, cv::Mat& canvas, const cv::Mat& reference, const std::vector<std::vector<int>> brush_vec, std::vector<layer>& layer_list, const std::vector<std::vector<float>>& image_etf_dx, const std::vector<std::vector<float>>& image_etf_dy, float K_val)
{
	//���� �귯���� �������� ū �귯�ð� ù ��° ���̾ ����Ǵ� ���� ����

	int width = reference.cols;
	int height = reference.rows;

	std::default_random_engine r(0);

	std::vector<std::vector<int>> accum_height; // ��⸦ �����ϴ� ĵ���� �������� ����.
	accum_height.assign(canvas.cols, std::vector<int>(canvas.rows, 0));

	T *= 255.f;

	//���� ū ������ ���� �� ������� ���ư��� �Ѵ�.
	for (int i = 0; i < layer_list.size(); i++)
	{
		cv::Mat step_canvas = cv::Mat(canvas.rows, canvas.cols, CV_8UC3, cv::Scalar(255, 255, 255));

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

				float average_brightness = 0;

				//�ٹ��� ���� ���ϱ� (x,y)
				double area_error = 0;

				//�ӽ� ������ ���� ���� ���� �Ҵ�
				temp_area.assign(layer_list[i].grid_size, std::vector<int>(layer_list[i].grid_size, 0));

				int RGB_diff = 0; // ���� ��ü�� ���� canvas���� reference image�� ���� RGB ������ ���� ���� �� ���̴� �ε���
				int RGB_index = 0; // �׸����� �� ĭ�� ���� R,G,B���� ���� ���̰��� ���� �ε���

				int pixel_number;

				//'�׸���'�� ��ĭ�� ��
				if (layer_list[i].grid_size <= 1) // if�� �����Ǹ� ������ �׸��� ����� 1.
				{

					for (int i = 0; i < 3; i++)
					{
						int canvas_color = canvas.at<cv::Vec3b>(y + MARGIN, x + MARGIN)[i];
						int reference_color = reference.at<cv::Vec3b>(y, x)[i]; // ���۷��� �̹����ϱ� �þ ĵ���� ���̸�ŭ �� �����൵ o
						RGB_index += difference(canvas_color, reference_color);
						
					}

					average_brightness = saliency_output.at<float>(y, x);
					area_error = RGB_index;
					pixel_number = 1;

				}

				//�׸��尡 ��ĭ�� �ƴ϶� �� �̻��� ���̷� ������ ���
				else
				{
					int index_x = x - temp_divide_two;
					int index_y = y - temp_divide_two;

					pixel_number = 0; // ��� ��⸦ ���ϱ� ����

					//�ӽ� ���� ��ü�� ���� ĵ������ �� �̹����� ���� R,G,B�� ���̸� ���ؾ� �Ѵ�. (x,y�� �׸����� ������ ���� ����)
					for (int j = x - (temp_divide_two); j <= x + (temp_divide_two); j++) // ��ȣ ���̴°� �´µ�
					{
						for (int k = y - (temp_divide_two); k <= y + (temp_divide_two); k++)
						{
							RGB_index = 0;

							for (int l = 0; l < 3; l++) // R,G,B�� ��� �� ���̹Ƿ�
							{
								int canvas_color = canvas.at<cv::Vec3b>(k + MARGIN, j + MARGIN)[l];
								int reference_color = reference.at<cv::Vec3b>(k, j)[l];
								RGB_index += difference(canvas_color, reference_color);
							}
							
							average_brightness += saliency_output.at<float>(k, j);
							temp_area[j - (index_x)][k - (index_y)] = RGB_index;
							RGB_diff += RGB_index;
							pixel_number++;
						}
					}

					RGB_diff /= 3.f; //R,G,B�������� ������� ���� ���̸� ���ϱ� ���ؼ�
					area_error = (RGB_diff / pixel_number);
					//area_error = (RGB_diff / (pow(layer_list[i].grid_size, 2))); (���� �ڵ�)
					int avg_bri = average_brightness;
					average_brightness /= (float)pixel_number;
				}

				if (i >= (layer_size * (2 / 3.f))) // �Ĺ� ���̾�
				{
					// saliency_image���� �� ������ ��� ��Ⱑ �������� T�� �����ϰ� �ۿ��ؾ� �Ѵ�.
					// average_brightness�� 255��� area_error <=0 �� ���� ĥ���� �ʴ´�.
					// 0�̶�� �� ��ĥ����.
					if (area_error <= (((1 / 255.f) * (255 - average_brightness)) * T))
					{
						continue;
					}
				}
				
				else // �ʹ� ���̾�
				{
					if (area_error <= T)
					{
						continue;
					}
				}


				//T���� ������� ���� ���, �ӽ� ���� ��ü�� ���� ���� ū ������ ���ؾ� �Ѵ�.

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


		//���̾� �ϳ� ���� �� ���� new_stroke_list�� ����� ��Ʈ��ũ���� ������ ������� ĥ�ϱ�
		//�ϴ� ������ ���� ������ �̰͵� �ؾ���!!

		//������ ����Ʈ �۾�
		for (int u = 0; u < new_stroke_list.size(); u++)
		{
			//new_stroke_list�� ����� ���� ���������� �����ϰ� �ڵ带 �ۼ�����.
			//�ʿ� ����?
			/*
			int ref_color_B = reference.at<cv::Vec3b>(new_stroke_list[u].y, new_stroke_list[u].x)[0];
			int ref_color_G = reference.at<cv::Vec3b>(new_stroke_list[u].y, new_stroke_list[u].x)[1];
			int ref_color_R = reference.at<cv::Vec3b>(new_stroke_list[u].y, new_stroke_list[u].x)[2];
			*/


			//�׸��� ������� �귯�� ����� ũ�ų� ���� ���� �� �ѹ��� ��������. -> �����Բ��� �׳� ��� ��µǵ��� �ٲٶ�� �ϼ̴�.

			//����귯�ø� �����Ϸ��� ��Ʈ��ũ ���� �ȼ��鿡 ���� �󸶸�ŭ�� '����'�� ĥ���� ������ ���Ͱ� �ʿ��ϴ�.
			std::vector<std::vector<float>> stroke_alpha;
			stroke_alpha.assign(canvas.cols, std::vector<float>(canvas.rows, 0));


			cv::Point_<float> lastDxDy(0, 0); // ���� dx, dy
			cv::Point_<float> present(new_stroke_list[u].x, new_stroke_list[u].y); // ���� x,y�� ��ġ
			cv::Point_<float> DxDy(image_etf_dx[int(present.x + 0.5f)][int(present.y + 0.5)], image_etf_dy[int(present.x + 0.5)][int(present.y + 0.5)]); // ���� x,y�� dx,dy
			//0.5�� �ݿø�

			std::vector<std::vector<int>> imsi_accum_height; // �ӽ� ����. �� ��Ʈ��ũ���� ��Ⱚ�� �����ϴ� ����. (�ߺ� ������ ����)
			imsi_accum_height.assign(canvas.cols, std::vector<int>(canvas.rows, 0));

			cv::Mat saved_canvas = canvas.clone(); // ��Ʈ��ũ�� ��� �� ��Ȳ�� �����ϴ� ĵ����. clone�Լ��� �� ��� ��Ȳ'��' ������ �ȴ�!!

			int ref_color_B = reference.at<cv::Vec3b>(int(present.y + 0.5), int(present.x + 0.5))[0];
			int ref_color_G = reference.at<cv::Vec3b>(int(present.y + 0.5), int(present.x + 0.5))[1];
			int ref_color_R = reference.at<cv::Vec3b>(int(present.y + 0.5), int(present.x + 0.5))[2];

			//��Ʈ��ũ ���� ���� �����ϰ� ĥ�ϱ� 
			//�ϴ� k�� �����ϴ� �ִ��� ����� ����. (���߿� �����ؾ���)

			float for_num = 0;

			if (layer_list[i].grid_size > 1)
				for_num = layer_list[i].grid_size * STROKE_NUMBER;

			else
				for_num = STROKE_NUMBER / 2.f;

			for (int k = 0; k < for_num; k++)
			{

				int canvas_diff = 0; // ���۷��� - ĵ������ �� ���� ���� ����
				int stroke_diff = 0; // ���۷��� - ���� ��Ʈ��ũ�� �� ���� ���� ����

				for (int l = 0; l < 3; l++)
				{
					canvas_diff += difference(canvas.at<cv::Vec3b>(int(present.y + 0.5), int(present.x + 0.5))[l], reference.at<cv::Vec3b>(int(present.y + 0.5), int(present.x + 0.5))[l]);
				}

				stroke_diff += difference(ref_color_B, reference.at<cv::Vec3b>(int(present.y + 0.5), int(present.x + 0.5))[0]);
				stroke_diff += difference(ref_color_G, reference.at<cv::Vec3b>(int(present.y + 0.5), int(present.x + 0.5))[1]);
				stroke_diff += difference(ref_color_R, reference.at<cv::Vec3b>(int(present.y + 0.5), int(present.x + 0.5))[2]);

				if (canvas_diff < stroke_diff) // ���۷���-ĵ���� < ���۷���-���� ��Ʈ��ũ�� �����̸� ����.
					break;

				if (DxDy.x == 0 && DxDy.y == 0)
					break;

				if (lastDxDy.x * DxDy.x + lastDxDy.y * DxDy.y < 0)
				{
					DxDy.x *= -1;
					DxDy.y *= -1;

				}

				if (!(getFlowVectorRK4(width, height, present.x, present.y, DxDy.x, DxDy.y, image_etf_dx, image_etf_dy)))
					break;

				int b_w = 100; // ������ brush image�� ũ��� 100*100���� �����̹Ƿ�

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
							int paint_x = MARGIN + int(present.x + 0.5) + (j - (layer_list[i].brush_size));
							int paint_y = MARGIN + int(present.y + 0.5) + (k - (layer_list[i].brush_size));

							if (paint_x >= canvas.cols || paint_x < 0 || paint_y < 0 || paint_y >= canvas.rows)
								continue;

							// �귯�� �������� ����� ����귯�� ���� ���İ�
							float brush_rate = (255 - brush_vec[b_w * index_x][b_w * index_y]) / 255.f;

							//�̹� ����Ǿ��ִ� ���İ����� �� ū ���İ��� ���� ���� ���� ��ĥ�Ѵ�.
							if (stroke_alpha[paint_x][paint_y] < brush_rate)
							{
								stroke_alpha[paint_x][paint_y] = brush_rate;

								int saved_B = saved_canvas.at<cv::Vec3b>(paint_y, paint_x)[0];
								int saved_G = saved_canvas.at<cv::Vec3b>(paint_y, paint_x)[1];
								int saved_R = saved_canvas.at<cv::Vec3b>(paint_y, paint_x)[2];

								//����귯�� ����
								int brush_alpha_B = (1 - brush_rate) * saved_B + brush_rate * ref_color_B;
								int brush_alpha_G = (1 - brush_rate) * saved_G + brush_rate * ref_color_G;
								int brush_alpha_R = (1 - brush_rate) * saved_R + brush_rate * ref_color_R;

								float brush_alpha_H, brush_alpha_S, brush_alpha_V; // ����귯��

								dw_RGB2HSV(brush_alpha_R, brush_alpha_G, brush_alpha_B, brush_alpha_H, brush_alpha_S, brush_alpha_V); // ����귯��

								//����귯��
								brush_alpha_H = random_alpha_H(brush_alpha_H);
								brush_alpha_S = random_alpha_S(brush_alpha_S);
								brush_alpha_V = random_alpha_V(brush_alpha_V);

								dw_HSV2RGB(brush_alpha_H, brush_alpha_S, brush_alpha_V, brush_alpha_R, brush_alpha_G, brush_alpha_B); // ����귯��

																//����귯�� ���� O, ��ī���� X
								canvas.at<cv::Vec3b>(paint_y, paint_x)[0] = brush_alpha_B;
								canvas.at<cv::Vec3b>(paint_y, paint_x)[1] = brush_alpha_G;
								canvas.at<cv::Vec3b>(paint_y, paint_x)[2] = brush_alpha_R;

								//������ �ƴ� �ش� �ܰ踸 ĥ���� ĵ���� �����
								step_canvas.at<cv::Vec3b>(paint_y, paint_x)[0] = brush_alpha_B;
								step_canvas.at<cv::Vec3b>(paint_y, paint_x)[1] = brush_alpha_G;
								step_canvas.at<cv::Vec3b>(paint_y, paint_x)[2] = brush_alpha_R;

								//��Ʈ��ũ�� ���� ū �� �����ϱ� (�� ��Ʈ��ũ�� �ߺ����� �ʵ���)
								if (imsi_accum_height[paint_x][paint_y] < (255 - brush_vec[b_w * index_x][b_w * index_y]))
									imsi_accum_height[paint_x][paint_y] = (255 - brush_vec[b_w * index_x][b_w * index_y]);

							}

							else
								continue;

						}

					}

				}


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
				

			//height_map�� ��Ÿ���� ���Ϳ� ���������� ���� ���� (��Ʈ��ũ ������ �����ؾ� ��)
			for (int x = 0; x < canvas.cols; x++)
			{
				for (int y = 0; y < canvas.rows; y++)
				{
					accum_height[x][y] += imsi_accum_height[x][y];
				}
			}

		}

		std::string str = std::to_string(i);
		str += ".jpg";
		std::cout << str << '\n';

		//��ȭ ������ ���Ѵٸ�
		cv::imwrite(str, canvas);

		//����ȭ ������ ���Ѵٸ�
		/*
		cv::cvtColor(canvas, canvas, cv::COLOR_BGR2GRAY);
		cv::imwrite(str, canvas);
		cv::cvtColor(canvas, canvas, cv::COLOR_GRAY2BGR);
		*/

		std::string str2 = "step";
		str2 += std::to_string(i);
		str2 += ".jpg";

		//��ȭ ������ ���Ѵٸ�
		cv::imwrite(str2, step_canvas);

		//����ȭ ������ ���Ѵٸ�
		/*
		cv::cvtColor(step_canvas, step_canvas, cv::COLOR_BGR2GRAY);
		cv::imwrite(str2, step_canvas);
		cv::cvtColor(step_canvas, step_canvas, cv::COLOR_GRAY2BGR);

		cv::imwrite(str2, step_canvas);
		*/

		//���̸� ����!
		cv::Mat accum_image = canvas.clone();
		accum_image = cv::Scalar(255, 255, 255);
		cvtColor(accum_image, accum_image, cv::COLOR_BGR2GRAY);

		int max = 0; //max�� �귯�ð� ���� ���� ���� ��

		for (int x = 0; x < canvas.cols; x++)
		{
			for (int y = 0; y < canvas.rows; y++)
			{
				if (accum_height[x][y] > max)
					max = accum_height[x][y];

			}
		}

		//rate == �迭��/max (0~1������ ���� ���´�.)
		for (int x = 0; x < canvas.cols; x++)
		{
			for (int y = 0; y < canvas.rows; y++)
			{
				accum_image.at<uchar>(y, x) = uchar((1 - ((float)accum_height[x][y] / max)) * 255); // max�ϼ��� ��ž� �ϴµ� ��..

			}
		}

		//���� ���� �̹���
		std::string str3 = "accumulate ";
		str3 += std::to_string(i);
		str3 += ".jpg";
		cv::imwrite(str3, accum_image);

		int layer_size = layer_list.size();

		if (i == layer_size - 1)
		{
			embossing(canvas, accum_image, K_val);
		}
	
	}
	
	return canvas;
}

//����귯�� X
cv::Mat stroke::paint(float T, const cv::Mat& saliency_output, cv::Mat& canvas, const cv::Mat& reference, const std::vector<std::vector<int>> brush_vec, std::vector<layer>& layer_list, const std::vector<std::vector<float>>& image_etf_dx, const std::vector<std::vector<float>>& image_etf_dy)
{
	//���� �귯���� �������� ū �귯�ð� ù ��° ���̾ ����Ǵ� ���� ����

	T *= 680; //�ִ��� 180(V)+255(S)+255(H)
	int width = reference.cols;
	int height = reference.rows;

	std::default_random_engine r(0);

	std::vector<std::vector<int>> accum_height; // ��⸦ �����ϴ� ĵ���� �������� ����.
	accum_height.assign(canvas.cols, std::vector<int>(canvas.rows, 0));

	//���� ū ������ ���� �� ������� ���ư��� �Ѵ�.
	for (int i = 0; i < layer_list.size(); i++)
	{
		cv::Mat step_canvas = cv::Mat(canvas.rows, canvas.cols, CV_8UC3, cv::Scalar(255, 255, 255));

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

				float average_brightness = 0;

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
						int canvas_color = canvas.at<cv::Vec3b>(y + MARGIN, x + MARGIN)[i];
						int reference_color = reference.at<cv::Vec3b>(y, x)[i]; // ���۷��� �̹����ϱ� �þ ĵ���� ���̸�ŭ �� �����൵ o
						RGB_index += difference(canvas_color, reference_color);

					}

					average_brightness = saliency_output.at<float>(y, x);
					area_error = RGB_index;

				}

				//�׸��尡 ��ĭ�� �ƴ϶� �� �̻��� ���̷� ������ ���
				else
				{
					int index_x = x - temp_divide_two;
					int index_y = y - temp_divide_two;

					int pixel_number = 0; // ��� ��⸦ ���ϱ� ����

					//�ӽ� ���� ��ü�� ���� ĵ������ �� �̹����� ���� R,G,B�� ���̸� ���ؾ� �Ѵ�. (x,y�� �׸����� ������ ���� ����)
					for (int j = x - (temp_divide_two); j <= x + (temp_divide_two); j++) // ��ȣ ���̴°� �´µ�
					{
						for (int k = y - (temp_divide_two); k <= y + (temp_divide_two); k++)
						{
							RGB_index = 0;

							for (int l = 0; l < 3; l++) // R,G,B�� ��� �� ���̹Ƿ�
							{
								int canvas_color = canvas.at<cv::Vec3b>(k + MARGIN, j + MARGIN)[l];
								int reference_color = reference.at<cv::Vec3b>(k, j)[l];
								RGB_index += difference(canvas_color, reference_color);
							}

							average_brightness += saliency_output.at<float>(k, j);
							temp_area[j - (index_x)][k - (index_y)] = RGB_index;
							RGB_diff += RGB_index;
							pixel_number++;
						}
					}

					area_error = (RGB_diff / (pow(layer_list[i].grid_size, 2))); // �ܼ��� ���̾��� grid_size^2�� ���� �ȵɰͰ���.
					average_brightness /= (float)pixel_number;
				}


				if (i >= (layer_size * (2 / 3.f))) // �Ĺ� ���̾�
				{
					if (area_error <= ((1 / 255.f) * (255 - average_brightness)) * T)
					{
						continue;
					}
				}

				else // �ʹ� ���̾�
				{
					if (area_error <= T)
					{
						continue;
					}
				}


				//T���� ������� ���� ���, �ӽ� ���� ��ü�� ���� ���� ū ������ ���ؾ� �Ѵ�.

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

				int alpha_B = ((1 - ALPHA) * canvas.at<cv::Vec3b>(MARGIN + new_stroke_list[u].y, MARGIN + new_stroke_list[u].x)[0]) + (ALPHA * ref_color_B);
				int alpha_G = ((1 - ALPHA) * canvas.at<cv::Vec3b>(MARGIN + new_stroke_list[u].y, MARGIN + new_stroke_list[u].x)[1]) + (ALPHA * ref_color_G);
				int alpha_R = ((1 - ALPHA) * canvas.at<cv::Vec3b>(MARGIN + new_stroke_list[u].y, MARGIN + new_stroke_list[u].x)[2]) + (ALPHA * ref_color_R);

				float alpha_H, alpha_S, alpha_V;

				//HSV ��ȯ �Լ�
				dw_RGB2HSV(alpha_R, alpha_G, alpha_B, alpha_H, alpha_S, alpha_V);

				// [HSV] (H,S,V�� ������ �ְ� �ʹٸ� �� ���ָ� Ǯ�� ���� �Լ�, imwrite�� cvtcolor���� Ǯ��)
				alpha_H = random_alpha_H(alpha_H);
				alpha_S = random_alpha_S(alpha_S);
				alpha_V = random_alpha_V(alpha_V);

				//RGB ��ȯ �Լ�
				dw_HSV2RGB(alpha_H, alpha_S, alpha_V, alpha_R, alpha_G, alpha_B);

				// ���� �ƴ� ��¥ �귯�� �������� ����ֱ� ���� �۾�
				int b_w = 100; // �ӽ�

				std::vector<std::vector<int>> imsi_accum_height; // �ӽ� ����. �� ��Ʈ��ũ���� ��Ⱚ�� �����ϴ� ����. (�ߺ� ������ ����)
				imsi_accum_height.assign(canvas.cols, std::vector<int>(canvas.rows, 0));

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

							canvas.at<cv::Vec3b>(paint_y, paint_x)[0] = alpha_B;
							canvas.at<cv::Vec3b>(paint_y, paint_x)[1] = alpha_G;
							canvas.at<cv::Vec3b>(paint_y, paint_x)[2] = alpha_R;

							//������ �ƴ� �ش� �ܰ踸 ĥ���� ĵ���� �����
							step_canvas.at<cv::Vec3b>(paint_y, paint_x)[0] = alpha_B;
							step_canvas.at<cv::Vec3b>(paint_y, paint_x)[1] = alpha_G;
							step_canvas.at<cv::Vec3b>(paint_y, paint_x)[2] = alpha_R;

							//������ ��Ʈ��ũ�� �����ϴ� �귯�ð� �ϳ��ϱ� imsi�� �ʿ� ����.
							accum_height[paint_x][paint_y] = 255 - brush_vec[b_w * index_x][b_w * index_y];


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

				std::vector<std::vector<int>> imsi_accum_height; // �ӽ� ����. �� ��Ʈ��ũ���� ��Ⱚ�� �����ϴ� ����. (�ߺ� ������ ����)
				imsi_accum_height.assign(canvas.cols, std::vector<int>(canvas.rows, 0));

				cv::Mat saved_canvas = canvas.clone(); // ��Ʈ��ũ�� ��� �� ��Ȳ�� �����ϴ� ĵ����. clone�Լ��� �� ��� ��Ȳ'��' ������ �ȴ�!!

				//(�� ����) ��Ʈ��ũ ĥ�ϱ�
				for (int k = 0; k < (layer_list[i].grid_size / layer_list[i].brush_size) * STROKE_NUMBER; k++)
				{

					int ref_color_B = reference.at<cv::Vec3b>(int(present.y + 0.5), int(present.x + 0.5))[0];
					int ref_color_G = reference.at<cv::Vec3b>(int(present.y + 0.5), int(present.x + 0.5))[1];
					int ref_color_R = reference.at<cv::Vec3b>(int(present.y + 0.5), int(present.x + 0.5))[2];

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

					if (!(getFlowVectorRK4(width, height, present.x, present.y, DxDy.x, DxDy.y, image_etf_dx, image_etf_dy)))
						break;

					int b_w = 100; // ������ brush image�� ũ��� 100*100���� �����̹Ƿ�

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
								int paint_x = MARGIN + int(present.x + 0.5) + (j - (layer_list[i].brush_size));
								int paint_y = MARGIN + int(present.y + 0.5) + (k - (layer_list[i].brush_size));

								if (paint_x >= canvas.cols || paint_x < 0 || paint_y < 0 || paint_y >= canvas.rows)
									continue;

								int saved_B = saved_canvas.at<cv::Vec3b>(paint_y, paint_x)[0];
								int saved_G = saved_canvas.at<cv::Vec3b>(paint_y, paint_x)[1];
								int saved_R = saved_canvas.at<cv::Vec3b>(paint_y, paint_x)[2];

								int alpha_B = (1 - ALPHA) * saved_B + ALPHA * ref_color_B;
								int alpha_G = (1 - ALPHA) * saved_G + ALPHA * ref_color_G;
								int alpha_R = (1 - ALPHA) * saved_R + ALPHA * ref_color_R;

								float alpha_H, alpha_S, alpha_V;

								dw_RGB2HSV(alpha_R, alpha_G, alpha_B, alpha_H, alpha_S, alpha_V);

								alpha_H = random_alpha_H(alpha_H);
								alpha_S = random_alpha_S(alpha_S);
								alpha_V = random_alpha_V(alpha_V);

								dw_HSV2RGB(alpha_H, alpha_S, alpha_V, alpha_R, alpha_G, alpha_B);

								//����귯�� ���� X, ��ī���� O
								canvas.at<cv::Vec3b>(paint_y, paint_x)[0] = alpha_B;
								canvas.at<cv::Vec3b>(paint_y, paint_x)[1] = alpha_G;
								canvas.at<cv::Vec3b>(paint_y, paint_x)[2] = alpha_R;

								//������ �ƴ� �ش� �ܰ踸 ĥ���� ĵ���� �����
								step_canvas.at<cv::Vec3b>(paint_y, paint_x)[0] = alpha_B;
								step_canvas.at<cv::Vec3b>(paint_y, paint_x)[1] = alpha_G;
								step_canvas.at<cv::Vec3b>(paint_y, paint_x)[2] = alpha_R;

								//��Ʈ��ũ�� ���� ū �� �����ϱ� (�� ��Ʈ��ũ�� �ߺ����� �ʵ���)
								if (imsi_accum_height[paint_x][paint_y] < (255 - brush_vec[b_w * index_x][b_w * index_y]))
									imsi_accum_height[paint_x][paint_y] = (255 - brush_vec[b_w * index_x][b_w * index_y]);

							}

						}

					}


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

				//height_map�� ��Ÿ���� ���Ϳ� ���������� ���� ���� (��Ʈ��ũ ������ �����ؾ� ��)
				for (int x = 0; x < canvas.cols; x++)
				{
					for (int y = 0; y < canvas.rows; y++)
					{
						accum_height[x][y] += imsi_accum_height[x][y];
					}
				}
			}

		}

		//cv::Mat canvas_BGR;

		std::string str = std::to_string(i);
		str += ".jpg";
		std::cout << str << '\n';

		cv::imwrite(str, canvas);

		std::string str2 = "step";
		str2 += std::to_string(i);
		str2 += ".jpg";

		cv::imwrite(str2, step_canvas);

	}

	//���� ���� �̹���
	//cv::Mat accum_image = accumulate_image(canvas.cols, canvas.rows, accum_height);

	cv::Mat accum_image = canvas;
	accum_image = cv::Scalar(255, 255, 255);
	cvtColor(accum_image, accum_image, cv::COLOR_BGR2GRAY);

	int max = 0; //max�� �귯�ð� ���� ���� ���� ��

	for (int x = 0; x < canvas.cols; x++)
	{
		for (int y = 0; y < canvas.rows; y++)
		{
			if (accum_height[x][y] > max)
				max = accum_height[x][y];

		}
	}



	//rate == �迭��/max (0~1������ ���� ���´�.)
	for (int x = 0; x < canvas.cols; x++)
	{
		for (int y = 0; y < canvas.rows; y++)
		{
			accum_image.at<uchar>(y, x) = uchar((1 - ((float)accum_height[x][y] / max)) * 255);

		}
	}

	cv::imwrite("accumulate.jpg", accum_image);

	return canvas;
}

