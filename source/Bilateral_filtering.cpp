#include <iostream>
#include "Painterly.h"
#include <vector>
#include <cmath>
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

Mat Bilateral_filtering(const Mat &image)
{
    Mat bilateral_image = image;

    int width = image.cols;
    int height = image.rows;

    int size = GAUSSIAN_SIZE;
    double sigma = 17.5;
    double sigma_b = 9.5;
    int kernel_width = (size - 1) / 2;
    double e_exponent = 0;

    vector<vector<double>> G_blur_func;
    G_blur_func.assign(size, vector<double>(size, 0));

    //G_blur_func에 값 저장하기
    /*
    for (int k = 0; k < size; k++) {
        for (int j = 0; j < size; j++) {

            e_exponent = -((pow(kernel_width - k, 2) + pow(kernel_width - j, 2)) / (2 * pow(GAUSSIAN_SIGMA, 2)));
            G_blur_func[k][j] = (1 / (6.283184 * pow(GAUSSIAN_SIGMA, 2))) * pow(2.718282, e_exponent);
        }
    }

    */
    vector<vector<double>> bilateral;

    //본격적인 bilateral_filtering

    for (int y = kernel_width; y < height - kernel_width; y++) {
        for (int x = kernel_width; x < width - kernel_width; x++) {

            bilateral.assign(size, vector<double>(size, 0)); // 매 픽셀을 돌 때마다 0으로 초기화 시켜주기.
            //bilateral_filtering함수 구현하기

            for (int k = 0; k < size; k++) {
                for (int j = 0; j < size; j++) {
                    double e_exponent = 0;

                    int sub_bri_B = abs((image.at<Vec3b>(y, x)[0] - image.at<Vec3b>(y - kernel_width + j, x - kernel_width + k)[0]));
                    int sub_bri_G = abs((image.at<Vec3b>(y, x)[1] - image.at<Vec3b>(y - kernel_width + j, x - kernel_width + k)[1]));
                    int sub_bri_R = abs((image.at<Vec3b>(y, x)[2] - image.at<Vec3b>(y - kernel_width + j, x - kernel_width + k)[2]));

                    e_exponent -= ((pow(sub_bri_B, 2)) / (2 * pow(sigma, 2)));
                    e_exponent -= ((pow(sub_bri_G, 2)) / (2 * pow(sigma, 2)));
                    e_exponent -= ((pow(sub_bri_R, 2)) / (2 * pow(sigma, 2)));

                    e_exponent /= 3;

                    G_blur_func[k][j] = (1 / (6.283184 * pow(sigma, 2))) * pow(2.718282, e_exponent);
                }
            }

            double B_blur = 0;
            double G_blur = 0;
            double R_blur = 0;

            long double G_sum = 0;

            for (int k = 0; k < size; k++) {
                for (int j = 0; j < size; j++) {
                    B_blur += (image.at<cv::Vec3b>(y - kernel_width + j, x - kernel_width + k)[0] * G_blur_func[k][j]);
                    G_blur += (image.at<cv::Vec3b>(y - kernel_width + j, x - kernel_width + k)[1] * G_blur_func[k][j]);
                    R_blur += (image.at<cv::Vec3b>(y - kernel_width + j, x - kernel_width + k)[2] * G_blur_func[k][j]);

                    G_sum += G_blur_func[k][j];
                }
            }

            bilateral_image.at<cv::Vec3b>(y, x)[0] = (B_blur / G_sum);
            bilateral_image.at<cv::Vec3b>(y, x)[1] = (G_blur / G_sum);
            bilateral_image.at<cv::Vec3b>(y, x)[2] = (R_blur / G_sum);
        }
    }

    return bilateral_image;
}

