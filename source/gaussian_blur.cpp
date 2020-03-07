#include <iostream>
#include <vector>
#include "opencv2/opencv.hpp"
#include "Painterly.h"

cv::Mat blurring(cv::Mat & image, double g_sigma)
{
    //가우시안 함수 벡터 계산

    int size = GAUSSIAN_SIZE;
    int kernel_width = (size - 1) / 2;
    double e_exponent = 0;

    std::vector<std::vector<double>> G_blur_func;
    G_blur_func.assign(size, std::vector<double>(size, 0));

    for (int k = 0; k < size; k++) {
        for (int j = 0; j < size; j++) {

            e_exponent = -((pow(kernel_width - k, 2) + pow(kernel_width - j, 2)) / (2 * pow(g_sigma, 2)));
            G_blur_func[k][j] = (1 / (6.283184 * pow(g_sigma, 2))) * pow(2.718282, e_exponent);
        }
    }

    //본격적인 이미지 블러링

    int height = image.rows;
    int width = image.cols;

    for (int y = kernel_width; y < height - kernel_width; y++) {
        for (int x = kernel_width; x < width - kernel_width; x++) {

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

            image.at<cv::Vec3b>(y, x)[0] = (B_blur / G_sum);
            image.at<cv::Vec3b>(y, x)[1] = (G_blur / G_sum);
            image.at<cv::Vec3b>(y, x)[2] = (R_blur / G_sum);
        }
    }

	return image;
}

cv::Mat blurring_grayscale(cv::Mat& image, double g_sigma)
{
    //가우시안 함수 벡터 계산

    int size = GAUSSIAN_SIZE;
    int kernel_width = (size - 1) / 2;
    double e_exponent = 0;

    std::vector<std::vector<double>> G_blur_func;
    G_blur_func.assign(size, std::vector<double>(size, 0));

    for (int k = 0; k < size; k++) {
        for (int j = 0; j < size; j++) {

            e_exponent = -((pow(kernel_width - k, 2) + pow(kernel_width - j, 2)) / (2 * pow(g_sigma, 2)));
            G_blur_func[k][j] = (1 / (6.283184 * pow(g_sigma, 2))) * pow(2.718282, e_exponent);
        }
    }

    //본격적인 이미지 블러링

    int height = image.rows;
    int width = image.cols;

    for (int y = kernel_width; y < height - kernel_width; y++) {
        for (int x = kernel_width; x < width - kernel_width; x++) {

            double point_blur = 0;

            long double G_sum = 0;


            for (int k = 0; k < size; k++) {
                for (int j = 0; j < size; j++) {

                    point_blur += (image.at<uchar>(y - kernel_width + j, x - kernel_width + k) * G_blur_func[k][j]);

                    G_sum += G_blur_func[k][j];

                }
            }

            image.at<uchar>(y, x) = (point_blur / G_sum);
        }
    }

    return image;
}