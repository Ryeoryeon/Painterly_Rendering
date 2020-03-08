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
				push_back(test_brush, test_fg, 1); // 그리드 사이즈가 0이 되는 것을 방지.

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
		std::cout << "오류!" << '\n';
	}



	//원래는 한꺼번에 구조체 벡터를 push_back으로 받아주려고 했으나 버퍼문제 때문에 수정
	return layer_list;
}

int stroke::calculate_margin(int layer, int length)
{
	if (layer_list[layer].grid_size == 0) // 그냥 예외처리 하자.
		layer_list[layer].grid_size = 1;

	int grid_number = length / layer_list[layer].grid_size; // 그리드의 개수
	int index = (length - (grid_number * (int)layer_list[layer].grid_size)) / 2;
	return index;
}

//에어브러시 O
cv::Mat stroke::paint_airbrush(float T, const cv::Mat& saliency_output, cv::Mat& canvas, const cv::Mat& reference, const std::vector<std::vector<int>> brush_vec, std::vector<layer>& layer_list, const std::vector<std::vector<float>>& image_etf_dx, const std::vector<std::vector<float>>& image_etf_dy, float K_val)
{
	//가장 브러시의 반지름이 큰 브러시가 첫 번째 레이어에 저장되는 것을 가정

	int width = reference.cols;
	int height = reference.rows;

	std::default_random_engine r(0);

	std::vector<std::vector<int>> accum_height; // 밝기를 누적하는 캔버스 사이즈의 벡터.
	accum_height.assign(canvas.cols, std::vector<int>(canvas.rows, 0));

	T *= 255.f;

	//가장 큰 원부터 작은 원 순서대로 돌아가야 한다.
	for (int i = 0; i < layer_list.size(); i++)
	{
		cv::Mat step_canvas = cv::Mat(canvas.rows, canvas.cols, CV_8UC3, cv::Scalar(255, 255, 255));

		//int temp_area_length = 2 * (layer_list[i].grid_size / 2) + 1; // 무조건 홀수가 되도록. (구조체에서 적용해줌)
		int temp_divide_two = (layer_list[i].grid_size - 1) / 2; // 짝수/2. temp_area_length에서 (double)2로 나누게 되면 length가 짝수가 나오는 경우가 생겨 모순 발생 가능.

		int margin_x = calculate_margin(i, width);
		int margin_y = calculate_margin(i, height);

		std::vector<cv::Point_<int>> new_stroke_list;
		std::vector<std::vector<int>> temp_area;


		for (int x = (margin_x + temp_divide_two); x < (width - margin_x - temp_divide_two); x += (int)layer_list[i].grid_size)
		{
			for (int y = (margin_y + temp_divide_two); y < (height - margin_y - temp_divide_two); y += (int)layer_list[i].grid_size)
			{

				float average_brightness = 0;

				//근방의 에러 더하기 (x,y)
				double area_error = 0;

				//임시 공간에 대한 이중 벡터 할당
				temp_area.assign(layer_list[i].grid_size, std::vector<int>(layer_list[i].grid_size, 0));

				int RGB_diff = 0; // 영역 전체에 대한 canvas값과 reference image에 대한 RGB 차이의 합을 구할 때 쓰이는 인덱스
				int RGB_index = 0; // 그리드의 한 칸에 대해 R,G,B값에 대한 차이값을 더한 인덱스

				int pixel_number;

				//'그리드'가 한칸일 때
				if (layer_list[i].grid_size <= 1) // if가 성립되면 무조건 그리드 사이즈가 1.
				{

					for (int i = 0; i < 3; i++)
					{
						int canvas_color = canvas.at<cv::Vec3b>(y + MARGIN, x + MARGIN)[i];
						int reference_color = reference.at<cv::Vec3b>(y, x)[i]; // 레퍼런스 이미지니까 늘어난 캔버스 길이만큼 안 더해줘도 o
						RGB_index += difference(canvas_color, reference_color);
						
					}

					average_brightness = saliency_output.at<float>(y, x);
					area_error = RGB_index;
					pixel_number = 1;

				}

				//그리드가 한칸이 아니라 그 이상의 길이로 구성될 경우
				else
				{
					int index_x = x - temp_divide_two;
					int index_y = y - temp_divide_two;

					pixel_number = 0; // 평균 밝기를 구하기 위해

					//임시 공간 전체에 대해 캔버스와 블러 이미지에 대한 R,G,B값 차이를 구해야 한다. (x,y가 그리드의 센터일 때를 기준)
					for (int j = x - (temp_divide_two); j <= x + (temp_divide_two); j++) // 등호 붙이는게 맞는듯
					{
						for (int k = y - (temp_divide_two); k <= y + (temp_divide_two); k++)
						{
							RGB_index = 0;

							for (int l = 0; l < 3; l++) // R,G,B는 모두 세 개이므로
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

					RGB_diff /= 3.f; //R,G,B관점에서 평균적인 색의 차이를 구하기 위해서
					area_error = (RGB_diff / pixel_number);
					//area_error = (RGB_diff / (pow(layer_list[i].grid_size, 2))); (원래 코드)
					int avg_bri = average_brightness;
					average_brightness /= (float)pixel_number;
				}

				if (i >= (layer_size * (2 / 3.f))) // 후반 레이어
				{
					// saliency_image에서 그 영역의 평균 밝기가 높을수록 T에 관대하게 작용해야 한다.
					// average_brightness가 255라면 area_error <=0 일 때만 칠하지 않는다.
					// 0이라면 다 안칠해짐.
					if (area_error <= (((1 / 255.f) * (255 - average_brightness)) * T))
					{
						continue;
					}
				}
				
				else // 초반 레이어
				{
					if (area_error <= T)
					{
						continue;
					}
				}


				//T에서 통과하지 않은 경우, 임시 공간 전체에 대해 가장 큰 에러를 구해야 한다.

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

					//그리드에서 가장 큰 지점을 찾았으면 실제 x,y위치로 환원 해 줘야 한다.
					//x,y는 그리드에서 센터임에 주의!! temp_divide_two는 센터를 제외한 양 옆의 길이를 /2 한 것임에 주의!
					//그런데 배열은 0부터 시작하니까 그냥 temp_divide_two에 +1 안 해줘도 될 것 같다.

					biggest_error.x = x + (biggest_error.x - (temp_divide_two));
					biggest_error.y = y + (biggest_error.y - temp_divide_two);

					/*
					//원래는 케이스 나눠서 대입해야 하나 생각하고 있었음.
					if (biggest_error.x > temp_divide_two)
					{
						biggest_error.x = (biggest_error.x - temp_divide_two);
					}
					*/

					new_stroke_list.push_back(biggest_error);
				}

			}

		}


		//레이어 하나 끝날 때 마다 new_stroke_list에 저장된 스트로크들을 랜덤한 순서대로 칠하기
		//일단 랜덤은 구현 안했음 이것도 해야함!!

		//진정한 페인트 작업
		for (int u = 0; u < new_stroke_list.size(); u++)
		{
			//new_stroke_list에 저장된 점을 시작점으로 가정하고 코드를 작성하자.
			//필요 없나?
			/*
			int ref_color_B = reference.at<cv::Vec3b>(new_stroke_list[u].y, new_stroke_list[u].x)[0];
			int ref_color_G = reference.at<cv::Vec3b>(new_stroke_list[u].y, new_stroke_list[u].x)[1];
			int ref_color_R = reference.at<cv::Vec3b>(new_stroke_list[u].y, new_stroke_list[u].x)[2];
			*/


			//그리드 사이즈보다 브러시 사이즈가 크거나 같을 때는 점 한번만 찍히도록. -> 교수님께서 그냥 길게 출력되도록 바꾸라고 하셨다.

			//에어브러시를 적용하려면 스트로크 구성 픽셀들에 대해 얼마만큼의 '알파'로 칠할지 저장할 벡터가 필요하다.
			std::vector<std::vector<float>> stroke_alpha;
			stroke_alpha.assign(canvas.cols, std::vector<float>(canvas.rows, 0));


			cv::Point_<float> lastDxDy(0, 0); // 지난 dx, dy
			cv::Point_<float> present(new_stroke_list[u].x, new_stroke_list[u].y); // 현재 x,y의 위치
			cv::Point_<float> DxDy(image_etf_dx[int(present.x + 0.5f)][int(present.y + 0.5)], image_etf_dy[int(present.x + 0.5)][int(present.y + 0.5)]); // 현재 x,y의 dx,dy
			//0.5는 반올림

			std::vector<std::vector<int>> imsi_accum_height; // 임시 벡터. 각 스트로크에서 밝기값을 저장하는 벡터. (중복 방지를 위해)
			imsi_accum_height.assign(canvas.cols, std::vector<int>(canvas.rows, 0));

			cv::Mat saved_canvas = canvas.clone(); // 스트로크를 찍기 전 상황을 저장하는 캔버스. clone함수를 써 줘야 상황'만' 복제가 된다!!

			int ref_color_B = reference.at<cv::Vec3b>(int(present.y + 0.5), int(present.x + 0.5))[0];
			int ref_color_G = reference.at<cv::Vec3b>(int(present.y + 0.5), int(present.x + 0.5))[1];
			int ref_color_R = reference.at<cv::Vec3b>(int(present.y + 0.5), int(present.x + 0.5))[2];

			//스트로크 알파 벡터 구성하고 칠하기 
			//일단 k를 제어하는 최댓값은 상수로 고정. (나중에 수정해야함)

			float for_num = 0;

			if (layer_list[i].grid_size > 1)
				for_num = layer_list[i].grid_size * STROKE_NUMBER;

			else
				for_num = STROKE_NUMBER / 2.f;

			for (int k = 0; k < for_num; k++)
			{

				int canvas_diff = 0; // 레퍼런스 - 캔버스의 색 차이 저장 변수
				int stroke_diff = 0; // 레퍼런스 - 현재 스트로크의 색 차이 저장 변수

				for (int l = 0; l < 3; l++)
				{
					canvas_diff += difference(canvas.at<cv::Vec3b>(int(present.y + 0.5), int(present.x + 0.5))[l], reference.at<cv::Vec3b>(int(present.y + 0.5), int(present.x + 0.5))[l]);
				}

				stroke_diff += difference(ref_color_B, reference.at<cv::Vec3b>(int(present.y + 0.5), int(present.x + 0.5))[0]);
				stroke_diff += difference(ref_color_G, reference.at<cv::Vec3b>(int(present.y + 0.5), int(present.x + 0.5))[1]);
				stroke_diff += difference(ref_color_R, reference.at<cv::Vec3b>(int(present.y + 0.5), int(present.x + 0.5))[2]);

				if (canvas_diff < stroke_diff) // 레퍼런스-캔버스 < 레퍼런스-현재 스트로크의 색깔이면 종료.
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

				int b_w = 100; // 어차피 brush image의 크기는 100*100으로 고정이므로

				for (int j = 0; j < 2 * layer_list[i].brush_size; j++)
				{
					for (int k = 0; k < 2 * layer_list[i].brush_size; k++)
					{
						//일단 new_stroke_list의 좌표가 원의 중심이 되겠지.
						//그러면 j,k는 0부터 시작하겠지만 실제론 반지름만큼 왼쪽, 위로 이동시켜준 후에 색을 찍어야 할 것 같아.
						float index_x = j / (2.0 * layer_list[i].brush_size);
						float index_y = k / (2.0 * layer_list[i].brush_size);

						if (brush_vec[b_w * index_x][b_w * index_y] != 255)
						{
							int paint_x = MARGIN + int(present.x + 0.5) + (j - (layer_list[i].brush_size));
							int paint_y = MARGIN + int(present.y + 0.5) + (k - (layer_list[i].brush_size));

							if (paint_x >= canvas.cols || paint_x < 0 || paint_y < 0 || paint_y >= canvas.rows)
								continue;

							// 브러시 패턴으로 계산한 에어브러시 적용 알파값
							float brush_rate = (255 - brush_vec[b_w * index_x][b_w * index_y]) / 255.f;

							//이미 저장되어있는 알파값보다 더 큰 알파값이 들어올 때만 새로 색칠한다.
							if (stroke_alpha[paint_x][paint_y] < brush_rate)
							{
								stroke_alpha[paint_x][paint_y] = brush_rate;

								int saved_B = saved_canvas.at<cv::Vec3b>(paint_y, paint_x)[0];
								int saved_G = saved_canvas.at<cv::Vec3b>(paint_y, paint_x)[1];
								int saved_R = saved_canvas.at<cv::Vec3b>(paint_y, paint_x)[2];

								//에어브러시 색깔
								int brush_alpha_B = (1 - brush_rate) * saved_B + brush_rate * ref_color_B;
								int brush_alpha_G = (1 - brush_rate) * saved_G + brush_rate * ref_color_G;
								int brush_alpha_R = (1 - brush_rate) * saved_R + brush_rate * ref_color_R;

								float brush_alpha_H, brush_alpha_S, brush_alpha_V; // 에어브러시

								dw_RGB2HSV(brush_alpha_R, brush_alpha_G, brush_alpha_B, brush_alpha_H, brush_alpha_S, brush_alpha_V); // 에어브러시

								//에어브러시
								brush_alpha_H = random_alpha_H(brush_alpha_H);
								brush_alpha_S = random_alpha_S(brush_alpha_S);
								brush_alpha_V = random_alpha_V(brush_alpha_V);

								dw_HSV2RGB(brush_alpha_H, brush_alpha_S, brush_alpha_V, brush_alpha_R, brush_alpha_G, brush_alpha_B); // 에어브러시

																//에어브러시 적용 O, 마카느낌 X
								canvas.at<cv::Vec3b>(paint_y, paint_x)[0] = brush_alpha_B;
								canvas.at<cv::Vec3b>(paint_y, paint_x)[1] = brush_alpha_G;
								canvas.at<cv::Vec3b>(paint_y, paint_x)[2] = brush_alpha_R;

								//누적이 아닌 해당 단계만 칠해진 캔버스 저장용
								step_canvas.at<cv::Vec3b>(paint_y, paint_x)[0] = brush_alpha_B;
								step_canvas.at<cv::Vec3b>(paint_y, paint_x)[1] = brush_alpha_G;
								step_canvas.at<cv::Vec3b>(paint_y, paint_x)[2] = brush_alpha_R;

								//스트로크에 가장 큰 값 대입하기 (한 스트로크에 중복되지 않도록)
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
				

			//height_map을 나타내는 벡터에 최종적으로 값을 저장 (스트로크 끝나고 대입해야 함)
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

		//유화 느낌을 원한다면
		cv::imwrite(str, canvas);

		//수묵화 느낌을 원한다면
		/*
		cv::cvtColor(canvas, canvas, cv::COLOR_BGR2GRAY);
		cv::imwrite(str, canvas);
		cv::cvtColor(canvas, canvas, cv::COLOR_GRAY2BGR);
		*/

		std::string str2 = "step";
		str2 += std::to_string(i);
		str2 += ".jpg";

		//유화 느낌을 원한다면
		cv::imwrite(str2, step_canvas);

		//수묵화 느낌을 원한다면
		/*
		cv::cvtColor(step_canvas, step_canvas, cv::COLOR_BGR2GRAY);
		cv::imwrite(str2, step_canvas);
		cv::cvtColor(step_canvas, step_canvas, cv::COLOR_GRAY2BGR);

		cv::imwrite(str2, step_canvas);
		*/

		//높이를 쌓자!
		cv::Mat accum_image = canvas.clone();
		accum_image = cv::Scalar(255, 255, 255);
		cvtColor(accum_image, accum_image, cv::COLOR_BGR2GRAY);

		int max = 0; //max는 브러시가 가장 많이 쌓인 애

		for (int x = 0; x < canvas.cols; x++)
		{
			for (int y = 0; y < canvas.rows; y++)
			{
				if (accum_height[x][y] > max)
					max = accum_height[x][y];

			}
		}

		//rate == 배열값/max (0~1사이의 값이 나온다.)
		for (int x = 0; x < canvas.cols; x++)
		{
			for (int y = 0; y < canvas.rows; y++)
			{
				accum_image.at<uchar>(y, x) = uchar((1 - ((float)accum_height[x][y] / max)) * 255); // max일수록 까매야 하는데 흠..

			}
		}

		//높이 저장 이미지
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

//에어브러시 X
cv::Mat stroke::paint(float T, const cv::Mat& saliency_output, cv::Mat& canvas, const cv::Mat& reference, const std::vector<std::vector<int>> brush_vec, std::vector<layer>& layer_list, const std::vector<std::vector<float>>& image_etf_dx, const std::vector<std::vector<float>>& image_etf_dy)
{
	//가장 브러시의 반지름이 큰 브러시가 첫 번째 레이어에 저장되는 것을 가정

	T *= 680; //최댓값은 180(V)+255(S)+255(H)
	int width = reference.cols;
	int height = reference.rows;

	std::default_random_engine r(0);

	std::vector<std::vector<int>> accum_height; // 밝기를 누적하는 캔버스 사이즈의 벡터.
	accum_height.assign(canvas.cols, std::vector<int>(canvas.rows, 0));

	//가장 큰 원부터 작은 원 순서대로 돌아가야 한다.
	for (int i = 0; i < layer_list.size(); i++)
	{
		cv::Mat step_canvas = cv::Mat(canvas.rows, canvas.cols, CV_8UC3, cv::Scalar(255, 255, 255));

		//int temp_area_length = 2 * (layer_list[i].grid_size / 2) + 1; // 무조건 홀수가 되도록. (구조체에서 적용해줌)
		int temp_divide_two = (layer_list[i].grid_size - 1) / 2; // 짝수/2. temp_area_length에서 (double)2로 나누게 되면 length가 짝수가 나오는 경우가 생겨 모순 발생 가능.

		int margin_x = calculate_margin(i, width);
		int margin_y = calculate_margin(i, height);

		std::vector<cv::Point_<int>> new_stroke_list;
		std::vector<std::vector<int>> temp_area;


		for (int x = (margin_x + temp_divide_two); x < (width - margin_x - temp_divide_two); x += (int)layer_list[i].grid_size)
		{
			for (int y = (margin_y + temp_divide_two); y < (height - margin_y - temp_divide_two); y += (int)layer_list[i].grid_size)
			{

				float average_brightness = 0;

				//근방의 에러 더하기 (x,y)
				double area_error = 0;

				//임시 공간에 대한 이중 벡터 할당
				temp_area.assign(layer_list[i].grid_size, std::vector<int>(layer_list[i].grid_size, 0));

				int RGB_diff = 0; // 영역 전체에 대한 canvas값과 reference image에 대한 RGB 차이의 합을 구할 때 쓰이는 인덱스
				int RGB_index = 0; // 그리드의 한 칸에 대해 R,G,B값에 대한 차이값을 더한 인덱스

				//그리드가 한칸일 때
				if (layer_list[i].grid_size <= 1) // if가 성립되면 무조건 그리드 사이즈가 1.
				{

					for (int i = 0; i < 3; i++)
					{
						int canvas_color = canvas.at<cv::Vec3b>(y + MARGIN, x + MARGIN)[i];
						int reference_color = reference.at<cv::Vec3b>(y, x)[i]; // 레퍼런스 이미지니까 늘어난 캔버스 길이만큼 안 더해줘도 o
						RGB_index += difference(canvas_color, reference_color);

					}

					average_brightness = saliency_output.at<float>(y, x);
					area_error = RGB_index;

				}

				//그리드가 한칸이 아니라 그 이상의 길이로 구성될 경우
				else
				{
					int index_x = x - temp_divide_two;
					int index_y = y - temp_divide_two;

					int pixel_number = 0; // 평균 밝기를 구하기 위해

					//임시 공간 전체에 대해 캔버스와 블러 이미지에 대한 R,G,B값 차이를 구해야 한다. (x,y가 그리드의 센터일 때를 기준)
					for (int j = x - (temp_divide_two); j <= x + (temp_divide_two); j++) // 등호 붙이는게 맞는듯
					{
						for (int k = y - (temp_divide_two); k <= y + (temp_divide_two); k++)
						{
							RGB_index = 0;

							for (int l = 0; l < 3; l++) // R,G,B는 모두 세 개이므로
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

					area_error = (RGB_diff / (pow(layer_list[i].grid_size, 2))); // 단순히 레이어의 grid_size^2를 빼면 안될것같음.
					average_brightness /= (float)pixel_number;
				}


				if (i >= (layer_size * (2 / 3.f))) // 후반 레이어
				{
					if (area_error <= ((1 / 255.f) * (255 - average_brightness)) * T)
					{
						continue;
					}
				}

				else // 초반 레이어
				{
					if (area_error <= T)
					{
						continue;
					}
				}


				//T에서 통과하지 않은 경우, 임시 공간 전체에 대해 가장 큰 에러를 구해야 한다.

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

					//그리드에서 가장 큰 지점을 찾았으면 실제 x,y위치로 환원 해 줘야 한다.
					//x,y는 그리드에서 센터임에 주의!! temp_divide_two는 센터를 제외한 양 옆의 길이를 /2 한 것임에 주의!
					//그런데 배열은 0부터 시작하니까 그냥 temp_divide_two에 +1 안 해줘도 될 것 같다.

					biggest_error.x = x + (biggest_error.x - (temp_divide_two));
					biggest_error.y = y + (biggest_error.y - temp_divide_two);

					/*
					//원래는 케이스 나눠서 대입해야 하나 생각하고 있었음.
					if (biggest_error.x > temp_divide_two)
					{
						biggest_error.x = (biggest_error.x - temp_divide_two);
					}
					*/

					new_stroke_list.push_back(biggest_error);
				}

			}

		}


		//레이어 하나 끝날 때 마다 new_stroke_list에 저장된 스트로크들을 랜덤한 순서대로 칠하기
		//일단 랜덤은 구현 안했음 이것도 해야함!!

		//진정한 페인트 작업
		for (int u = 0; u < new_stroke_list.size(); u++)
		{
			//new_stroke_list에 저장된 점을 시작점으로 가정하고 코드를 작성하자.
			int ref_color_B = reference.at<cv::Vec3b>(new_stroke_list[u].y, new_stroke_list[u].x)[0];
			int ref_color_G = reference.at<cv::Vec3b>(new_stroke_list[u].y, new_stroke_list[u].x)[1];
			int ref_color_R = reference.at<cv::Vec3b>(new_stroke_list[u].y, new_stroke_list[u].x)[2];


			//그리드 사이즈보다 브러시 사이즈가 크거나 같을 때는 점 한번만 찍히도록.
			if (layer_list[i].brush_size >= layer_list[i].grid_size)
			{

				int alpha_B = ((1 - ALPHA) * canvas.at<cv::Vec3b>(MARGIN + new_stroke_list[u].y, MARGIN + new_stroke_list[u].x)[0]) + (ALPHA * ref_color_B);
				int alpha_G = ((1 - ALPHA) * canvas.at<cv::Vec3b>(MARGIN + new_stroke_list[u].y, MARGIN + new_stroke_list[u].x)[1]) + (ALPHA * ref_color_G);
				int alpha_R = ((1 - ALPHA) * canvas.at<cv::Vec3b>(MARGIN + new_stroke_list[u].y, MARGIN + new_stroke_list[u].x)[2]) + (ALPHA * ref_color_R);

				float alpha_H, alpha_S, alpha_V;

				//HSV 변환 함수
				dw_RGB2HSV(alpha_R, alpha_G, alpha_B, alpha_H, alpha_S, alpha_V);

				// [HSV] (H,S,V로 랜덤을 주고 싶다면 이 각주를 풀고 메인 함수, imwrite의 cvtcolor각주 풀기)
				alpha_H = random_alpha_H(alpha_H);
				alpha_S = random_alpha_S(alpha_S);
				alpha_V = random_alpha_V(alpha_V);

				//RGB 변환 함수
				dw_HSV2RGB(alpha_H, alpha_S, alpha_V, alpha_R, alpha_G, alpha_B);

				// 원이 아닌 진짜 브러시 패턴으로 찍어주기 위한 작업
				int b_w = 100; // 임시

				std::vector<std::vector<int>> imsi_accum_height; // 임시 벡터. 각 스트로크에서 밝기값을 저장하는 벡터. (중복 방지를 위해)
				imsi_accum_height.assign(canvas.cols, std::vector<int>(canvas.rows, 0));

				for (int j = 0; j < 2 * layer_list[i].brush_size; j++)
				{
					for (int k = 0; k < 2 * layer_list[i].brush_size; k++)
					{
						//일단 new_stroke_list의 좌표가 원의 중심이 되겠지.
						//그러면 j,k는 0부터 시작하겠지만 실제론 반지름만큼 왼쪽, 위로 이동시켜준 후에 색을 찍어야 할 것 같아.
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

							//누적이 아닌 해당 단계만 칠해진 캔버스 저장용
							step_canvas.at<cv::Vec3b>(paint_y, paint_x)[0] = alpha_B;
							step_canvas.at<cv::Vec3b>(paint_y, paint_x)[1] = alpha_G;
							step_canvas.at<cv::Vec3b>(paint_y, paint_x)[2] = alpha_R;

							//어차피 스트로크를 구성하는 브러시가 하나니까 imsi가 필요 없다.
							accum_height[paint_x][paint_y] = 255 - brush_vec[b_w * index_x][b_w * index_y];


						}

					}

				}


				//cv::circle(canvas, cv::Point(new_stroke_list[u].x + MARGIN, new_stroke_list[u].y + MARGIN), layer_list[i].brush_size, cv::Scalar(alpha_H, alpha_S, alpha_V), -1);
			}

			else
			{

				cv::Point_<float> lastDxDy(0, 0); // 지난 dx, dy
				cv::Point_<float> present(new_stroke_list[u].x, new_stroke_list[u].y); // 현재 x,y의 위치
				cv::Point_<float> DxDy(image_etf_dx[int(present.x + 0.5f)][int(present.y + 0.5)], image_etf_dy[int(present.x + 0.5)][int(present.y + 0.5)]); // 현재 x,y의 dx,dy
				//0.5는 반올림

				std::vector<std::vector<int>> imsi_accum_height; // 임시 벡터. 각 스트로크에서 밝기값을 저장하는 벡터. (중복 방지를 위해)
				imsi_accum_height.assign(canvas.cols, std::vector<int>(canvas.rows, 0));

				cv::Mat saved_canvas = canvas.clone(); // 스트로크를 찍기 전 상황을 저장하는 캔버스. clone함수를 써 줘야 상황'만' 복제가 된다!!

				//(한 개의) 스트로크 칠하기
				for (int k = 0; k < (layer_list[i].grid_size / layer_list[i].brush_size) * STROKE_NUMBER; k++)
				{

					int ref_color_B = reference.at<cv::Vec3b>(int(present.y + 0.5), int(present.x + 0.5))[0];
					int ref_color_G = reference.at<cv::Vec3b>(int(present.y + 0.5), int(present.x + 0.5))[1];
					int ref_color_R = reference.at<cv::Vec3b>(int(present.y + 0.5), int(present.x + 0.5))[2];

					/*
					float temp = 0; // 90도 변환을 위해서
					temp = DxDy.x;
					DxDy.x = -DxDy.y;
					DxDy.y = temp;
					*/

					int canvas_diff = 0; // 레퍼런스 - 캔버스의 색 차이 저장 변수
					int stroke_diff = 0; // 레퍼런스 - 현재 스트로크의 색 차이 저장 변수

					for (int l = 0; l < 3; l++)
					{
						canvas_diff += difference(canvas.at<cv::Vec3b>(int(present.y + 0.5), int(present.x + 0.5))[l], reference.at<cv::Vec3b>(int(present.y + 0.5), int(present.x + 0.5))[l]);
					}

					stroke_diff += difference(ref_color_B, reference.at<cv::Vec3b>(int(present.y + 0.5), int(present.x + 0.5))[0]);
					stroke_diff += difference(ref_color_G, reference.at<cv::Vec3b>(int(present.y + 0.5), int(present.x + 0.5))[1]);
					stroke_diff += difference(ref_color_R, reference.at<cv::Vec3b>(int(present.y + 0.5), int(present.x + 0.5))[2]);
					//stroke_diff += difference(ref_color_R, reference.at<cv::Vec3b>(present.y, present.x)[2]);

					if (canvas_diff < stroke_diff) // 레퍼런스-캔버스 < 레퍼런스-현재 스트로크의 색깔이면 종료.
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

					int b_w = 100; // 어차피 brush image의 크기는 100*100으로 고정이므로

					for (int j = 0; j < 2 * layer_list[i].brush_size; j++)
					{
						for (int k = 0; k < 2 * layer_list[i].brush_size; k++)
						{
							//일단 new_stroke_list의 좌표가 원의 중심이 되겠지.
							//그러면 j,k는 0부터 시작하겠지만 실제론 반지름만큼 왼쪽, 위로 이동시켜준 후에 색을 찍어야 할 것 같아.
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

								//에어브러시 적용 X, 마카느낌 O
								canvas.at<cv::Vec3b>(paint_y, paint_x)[0] = alpha_B;
								canvas.at<cv::Vec3b>(paint_y, paint_x)[1] = alpha_G;
								canvas.at<cv::Vec3b>(paint_y, paint_x)[2] = alpha_R;

								//누적이 아닌 해당 단계만 칠해진 캔버스 저장용
								step_canvas.at<cv::Vec3b>(paint_y, paint_x)[0] = alpha_B;
								step_canvas.at<cv::Vec3b>(paint_y, paint_x)[1] = alpha_G;
								step_canvas.at<cv::Vec3b>(paint_y, paint_x)[2] = alpha_R;

								//스트로크에 가장 큰 값 대입하기 (한 스트로크에 중복되지 않도록)
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

				//height_map을 나타내는 벡터에 최종적으로 값을 저장 (스트로크 끝나고 대입해야 함)
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

	//높이 저장 이미지
	//cv::Mat accum_image = accumulate_image(canvas.cols, canvas.rows, accum_height);

	cv::Mat accum_image = canvas;
	accum_image = cv::Scalar(255, 255, 255);
	cvtColor(accum_image, accum_image, cv::COLOR_BGR2GRAY);

	int max = 0; //max는 브러시가 가장 많이 쌓인 애

	for (int x = 0; x < canvas.cols; x++)
	{
		for (int y = 0; y < canvas.rows; y++)
		{
			if (accum_height[x][y] > max)
				max = accum_height[x][y];

		}
	}



	//rate == 배열값/max (0~1사이의 값이 나온다.)
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

