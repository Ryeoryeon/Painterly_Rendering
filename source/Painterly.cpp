#include <iostream>
#include <opencv2/opencv.hpp>
#include "Painterly.h"
#include <vector>
#include <random>


std::vector<layer> stroke::Painterly_initialize()
{
	//std::vector<layer>layer_list; (잠깐 내가 이걸 왜 넣었지..?)

	for (int i = 0; i < layer_size; i++)
	{

		//layer_list.reserve(layer_size);

		int test_brush;
		double test_fg;

		std::cout << i << "번째 레이어, 브러시 사이즈 입력 : ";
		std::cin >> test_brush;
		std::cin.ignore(100, '\n');

		/*
		그리드의 사이즈. fg가 1이면 브러시의 반지름 크기만큼.
		만약 1보다 작으면 스트로크가 그리드 밖으로 삐져나오고 크면 그 반대임.
		*/

		std::cout << i << "번째 레이어, f_g 변수값 입력 : (점점 작아져야 한다)";
		std::cin >> test_fg;
		std::cin.ignore(100, '\n');

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

cv::Mat stroke::paint(int T, cv::Mat& canvas, const cv::Mat& reference, std::vector<layer>& layer_list)
{
	//가장 큰! 브러시부터 실행해야함 (아마도?)
	//나는 가장 큰 브러시가 첫 번째 레이어에 저장되는 것을 가정해 코드를 짰음
	//후에 수정할 예정

	int width = canvas.cols;
	int height = canvas.rows;

	std::default_random_engine r(0);

	//가장 큰 원부터 작은 원 순서대로 돌아가야 한다.
	for (int i = 0; i < layer_list.size(); i++)
	{

		//int temp_area_length = 2 * (layer_list[i].grid_size / 2) + 1; // 무조건 홀수가 되도록. (구조체에서 적용해줌)
		int temp_divide_two = (layer_list[i].grid_size - 1) / 2; // 짝수/2. temp_area_length에서 (double)2로 나누게 되면 length가 짝수가 나오는 경우가 생겨 모순 발생 가능.

		int margin_x = calculate_margin(i, width);
		int margin_y = calculate_margin(i, height);

		std::vector<cv::Point_<int>> new_stroke_list;
		std::vector<std::vector<int>> temp_area;


		for (int x = (margin_x +  temp_divide_two); x < (width - margin_x - temp_divide_two); x += (int)layer_list[i].grid_size)
		{
			for (int y = (margin_y + temp_divide_two); y < (height - margin_y - temp_divide_two); y += (int)layer_list[i].grid_size)
			{
				//근방의 에러 더하기 (x,y)
				double area_error = 0;

				//임시 공간에 대한 이중 벡터 할당
				//센터를 기준으로 grid_size/2만큼의 길이씩 있으니까. (만일 grid_size가 홀수면 버림해줘야 하니까 저렇게 함)
				//임시 공간에 대한 크기를 그냥 grid*grid로 잡은거 != (논문에 쓰여있는대로)센터를 기준으로 저렇게 잡은거

				temp_area.assign(layer_list[i].grid_size, std::vector<int>(layer_list[i].grid_size, 0)); // $설마 여기서?

				int RGB_diff = 0; // 영역 전체에 대한 canvas값과 reference image에 대한 RGB 차이의 합을 구할 때 쓰이는 인덱스
				int RGB_index = 0; // 그리드의 한 칸에 대해 R,G,B값에 대한 차이값을 더한 인덱스

				//그리드가 한칸일 때
				if (layer_list[i].grid_size <= 1) // 그런데 if가 성립되면 무조건 그리드 사이즈가 1인거.
				{
					//grid_size/(double)2가 0일 때 가능하다.
					//이 때는 어떻게 예외처리를 하지?
					//어쨌든 임시 공간이 한칸이라는 거니까 그 칸에 대해서만 오류 합을 하면 되는 거 아닐까?

					for (int i = 0; i < 3; i++)
					{
						int canvas_color = canvas.at<cv::Vec3b>(y, x)[i];
						int reference_color = reference.at<cv::Vec3b>(y, x)[i];
						RGB_index += difference(canvas_color, reference_color);
					}

					area_error = RGB_index;

				}

				//그리드가 한칸이 아니라 그 이상의 길이로 구성될 경우
				else
				{
					int index_x = x - temp_divide_two;
					int index_y = y - temp_divide_two;

					//임시 공간 전체에 대해 캔버스와 블러 이미지에 대한 R,G,B값 차이를 구해야 한다.
					//x,y가 그리드의 센터일 때 기준으로~!
					for (int j = x - (temp_divide_two); j <= x + (temp_divide_two); j++) // 등호 붙이는게 맞는듯
					{
						for (int k = y - (temp_divide_two); k <= y + (temp_divide_two); k++)
						{
							RGB_index = 0;

							for (int l = 0; l < 3; l++) // R,G,B는 모두 세 개이므로
							{
								int canvas_color = canvas.at<cv::Vec3b>(y, x)[l];
								int reference_color = reference.at<cv::Vec3b>(y, x)[l];
								RGB_index = difference(canvas_color, reference_color);
							}

							temp_area[j - (index_x)][k - (index_y)] = RGB_index;
							RGB_diff += RGB_index;
						}
					}

					area_error = (RGB_diff / (pow(layer_list[i].grid_size, 2))); // 단순히 레이어의 grid_size^2를 빼면 안될것같음.
				}

				if (area_error <= T)
				{
					continue;
				}

				else
				{
					//임시 공간 전체에 대해 가장 큰 에러를 구해야 한다.

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

					/* (생각해보니 배열의 인덱스가 음수가 되네?)
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
				}
			}

		}

		std::cout << i + 1 << "번째 레이어 스트로크 구성 완료" << '\n';

		//레이어 하나 끝날 때 마다 new_stroke_list에 저장된 스트로크들을 랜덤한 순서대로 칠하기
		//일단 랜덤은 구현 안했음 이것도 해야함!!

		for (int u = 0; u < new_stroke_list.size(); u++)
		{
			int ref_color_B = reference.at<cv::Vec3b>(new_stroke_list[u].y, new_stroke_list[u].x)[0];
			int ref_color_G = reference.at<cv::Vec3b>(new_stroke_list[u].y, new_stroke_list[u].x)[1];
			int ref_color_R = reference.at<cv::Vec3b>(new_stroke_list[u].y, new_stroke_list[u].x)[2];

			cv::circle(canvas, new_stroke_list[u], layer_list[i].brush_size, cv::Scalar(ref_color_B, ref_color_G, ref_color_R), -1); // 두께에 -1을 입력하면 원 채우기
		}

		std::cout << i + 1 << "번째 레이어칠하기까지 완료" << '\n';

	}

	return canvas;
}
