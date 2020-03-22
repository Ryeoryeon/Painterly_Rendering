// Painterly_Rendering_mfc.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include "pch.h"
#include "framework.h"
#include "Painterly_Rendering_mfc.h"
#include "yh_dialog.h"
#include "yh_dialog_2.h"
#include "yh_dialog_3.h"
#include "yh_saliency.h"

#include "yh_vector.h"
#include "dwLIC2.h"
#include <opencv2/saliency/saliencySpecializedClasses.hpp> // saliency를 쓰기 위해 추가해야 하는 헤더파일


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 유일한 애플리케이션 개체입니다.

CWinApp theApp;

using namespace std;

int main()
{
    int nRetCode = 0;

    HMODULE hModule = ::GetModuleHandle(nullptr);

    if (hModule != nullptr)
    {
        // MFC를 초기화합니다. 초기화하지 못한 경우 오류를 인쇄합니다.
        if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
        {
            // TODO: 여기에 애플리케이션 동작을 코딩합니다.
            wprintf(L"심각한 오류: MFC 초기화 실패\n");
            nRetCode = 1;
        }

        else
        {
            // TODO: 여기에 애플리케이션 동작을 코딩합니다.


            stroke circle;

            //브러시
            wchar_t szFilter[] = _T("All Image Files|*.BMP;*.GIF;*.JPG;*.JPEG;*.PNG;*.TIF;*.TIFF|All Files (*.*)|*.*||");
            CFileDialog fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter);
            fileDlg.m_ofn.lpstrTitle = _T("Load Brush Image");

            if (IDOK != fileDlg.DoModal())
            {
                wprintf(L"파일을 선택하지 않았습니다.\n");
                nRetCode = 1;
                return nRetCode;
            }

            USES_CONVERSION;
            cv::Mat brush = cv::imread(W2A(fileDlg.GetPathName(), cv::IMREAD_GRAYSCALE));

            //인풋 이미지
            wchar_t szFilter2[] = _T("All Image Files|*.BMP;*.GIF;*.JPG;*.JPEG;*.PNG;*.TIF;*.TIFF|All Files (*.*)|*.*||");
            CFileDialog fileDlg2(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter2);
            fileDlg2.m_ofn.lpstrTitle = _T("Load Input Image");

            if (IDOK != fileDlg2.DoModal())
            {
                wprintf(L"파일을 선택하지 않았습니다.\n");
                nRetCode = 1;
                return nRetCode;
            }

            //USES_CONVERSION;
            cv::Mat image = cv::imread(W2A(fileDlg2.GetPathName()));

            //캔버스 배경 이미지
            wchar_t szFilter3[] = _T("All Image Files|*.BMP;*.GIF;*.JPG;*.JPEG;*.PNG;*.TIF;*.TIFF|All Files (*.*)|*.*||");
            CFileDialog fileDlg3(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter3);
            fileDlg3.m_ofn.lpstrTitle = _T("Load Background canvas Image");

            if (IDOK != fileDlg3.DoModal())
            {
                wprintf(L"파일을 선택하지 않았습니다.\n");
                nRetCode = 1;
                return nRetCode;
            }

            //USES_CONVERSION;
            cv::Mat canvas = cv::imread(W2A(fileDlg3.GetPathName()));
            //여기 resize함수 넣기

            int height = image.rows;
            int width = image.cols;

            cv::resize(canvas, canvas, cv::Size(width + 2 * MARGIN, height + 2 * MARGIN), 0, 0);

            int b_w = brush.cols;

            std::vector<std::vector<int>> brush_vec;
            brush_vec.assign(b_w, std::vector<int>(b_w, 0));

            cv::cvtColor(brush, brush, cv::COLOR_BGR2GRAY);

            cv::Mat imsi_brush = brush.clone();
            imsi_brush = cv::Scalar(255, 255, 255);

            //브러시의 밝기 저장하기
            for (int x = 0; x < b_w; x++)
            {
                for (int y = 0; y < b_w; y++)
                {
                    brush_vec[x][y] = brush.at<uchar>(y, x);
                    imsi_brush.at<uchar>(y,x) = brush_vec[x][y];
                }
            }
            
            //.etf파일
            wchar_t szFilter4[] = _T("Flow Field File|*.ETF|All Files (*.*)|*.*||");
            CFileDialog fileDlg4(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter4);
            fileDlg3.m_ofn.lpstrTitle = _T("Load etf file");

            if (IDOK != fileDlg4.DoModal())
            {
                wprintf(L"파일을 선택하지 않았습니다.\n");
                nRetCode = 1;
                return nRetCode;
            }

            //USES_CONVERSION;
            FILE* etf;
            fopen_s(&etf, W2A(fileDlg4.GetPathName()), "rb");

            float* buffer = new float[2 * width * height]; // 벡터로 하니까 안 된다.
            std::vector<std::vector<float>> image_etf_dx;
            std::vector<std::vector<float>> image_etf_dy;

            image_etf_dx.assign(width, std::vector<float>(height, 0));
            image_etf_dy.assign(width, std::vector<float>(height, 0));

            if (etf)
            {
                //size of float (width*height*size of float*2) // 이렇게 메모리를 할당을 한 다음에,

                //재완언니 : 단일 배열 하나만 넣음. 여기에 dx, dy 모두 저장했는데 인덱스가 짝수, 홀수냐에 따라 dx,dy저장 가능
                fread(buffer, sizeof(float), 2 * width * height, etf);

                int index = 0;

                //y에 대해서 x가 증가하는거니까 이게 가로방향으로 증가하는 느낌. etf는 가로로 쭉 붙이는거기때문에..
                for (int y = 0; y < height; y++)
                {
                    for (int x = 0; x < width; x++)
                    {
                        image_etf_dx[x][y] = buffer[index++];
                        image_etf_dy[x][y] = buffer[index++];
                    }
                }

                fclose(etf);
            }

            else
            {
                std::cout << "파일 열기 실패" << '\n';
                exit(1);
            }

            //saliency 외부입력 O/X 코드
            yh_saliency dlg_saliency;

            cv::Mat saliency_output;

            if (IDOK == dlg_saliency.DoModal())
            {
                CFileDialog fileDlg5(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter3);
                fileDlg5.m_ofn.lpstrTitle = _T("Load saliency Image");

                if (IDOK != fileDlg5.DoModal())
                {
                    wprintf(L"파일을 선택하지 않았습니다.\n");
                    nRetCode = 1;
                    return nRetCode;
                }

                //USES_CONVERSION;
                saliency_output = cv::imread(W2A(fileDlg5.GetPathName()));

            }

            else if (IDOK != dlg_saliency.DoModal())
            {
                cv::saliency::StaticSaliencySpectralResidual Sal;
                Sal.computeSaliency(image, saliency_output);
            }


            //레퍼런스 이미지 블러링 (대화상자1)
            cv::Mat blur_image = image.clone();

            yh_dialog dlg;

            if (IDOK == dlg.DoModal())
            {
                /*
                int blur_choice = dlg.RadioCtrl();

                if (blur_choice == 0)
                {
                    blur_image = blurring(image, 1.5);
                }

                else
                {
                    blur_image = Bilateral_filtering(image);
                }

                */

                int blur_choice;
                blur_choice = dlg.radioMode;
                circle.put_layersize(dlg.input_layer_size);

                if (blur_choice == 0)
                {
                    blur_image = blurring(image, 1.5);
                }

                else if(blur_choice == 1)
                {
                    blur_image = Bilateral_filtering(image);
                }
            }

            else if (IDOK != dlg.DoModal())
            {
                return 0;
            }

 
            //브러시와 그리드 사이즈 리스트 구성, T값 입력 (대화상자2)
            yh_dialog_2 dlg2;

            if (IDOK == dlg2.DoModal())
            {
                float T = dlg2.T_value;
                float K_val = dlg2.K_value;

                //yh_dialog_3는 painterly_initialize()에서 열린다.
                circle.layer_list = circle.Painterly_initialize();

                canvas = circle.paint_airbrush(T, saliency_output, canvas, blur_image, brush_vec, circle.layer_list, image_etf_dx, image_etf_dy, K_val);
            }

            else if (IDOK != dlg2.DoModal())
            {
                return 0;
            }

            //파일 저장
            wchar_t szFilter5[] = _T("All Image Files|*.BMP;*.GIF;*.JPG;*.JPEG;*.PNG;*.TIF;*.TIFF|All Files (*.*)|*.*||");
            CFileDialog fileDlg5(FALSE, NULL, NULL, OFN_HIDEREADONLY, szFilter5);
            fileDlg5.m_ofn.lpstrTitle = _T("Save Result Image");

            if (IDOK != fileDlg5.DoModal())

            {
                wprintf(L"파일을 저장하지 않았습니다.\n");
                nRetCode = 1;
                return nRetCode;
            }

            cv::imwrite(W2A(fileDlg5.GetPathName()), canvas);
            


        }
    }
    else
    {
        // TODO: 오류 코드를 필요에 따라 수정합니다.
        wprintf(L"심각한 오류: GetModuleHandle 실패\n");
        nRetCode = 1;
    }

    return nRetCode;
}
