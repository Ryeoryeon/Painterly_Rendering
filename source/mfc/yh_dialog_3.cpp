// yh_dialog_3.cpp: 구현 파일
//

#include "pch.h"
#include "Painterly_Rendering_mfc.h"
#include "yh_dialog_3.h"
#include "afxdialogex.h"


// yh_dialog_3 대화 상자

IMPLEMENT_DYNAMIC(yh_dialog_3, CDialogEx)

yh_dialog_3::yh_dialog_3(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG3, pParent)
	, brush_size_1(60)
	, f_g_1(1)
	, brush_size_2(40)
	, f_g_2(1.2)
	, brush_size_3(30)
	, f_g_3(1.5)
	, brush_size_4(20)
	, f_g_4(1.2)
	, brush_size_5(17)
	, f_g_5(1)
	, brush_size_6(12)
	, f_g_6(0.9)
	, brush_size_7(10)
	, f_g_7(0.9)
	, brush_size_8(8)
	, f_g_8(0.9)
	, brush_size_9(6)
	, f_g_9(0.8)
	, brush_size_10(0)
	, f_g_10(0)
	, brush_size_11(0)
	, f_g_11(0)
	, brush_size_12(0)
	, f_g_12(0)
	/* // 레이어의 개수가 7개일 때
	, brush_size_1(40)
	, f_g_1(4)
	, brush_size_2(30)
	, f_g_2(3)
	, brush_size_3(20)
	, f_g_3(2)
	, brush_size_4(15)
	, f_g_4(1.5)
	, brush_size_5(10)
	, f_g_5(1)
	, brush_size_6(7)
	, f_g_6(0.7)
	, brush_size_7(6)
	, f_g_7(0.6)
	, brush_size_8(0)
	, f_g_8(0)
	, brush_size_9(0)
	, f_g_9(0)
	, brush_size_10(0)
	, f_g_10(0)
	, brush_size_11(0)
	, f_g_11(0)
	, brush_size_12(0)
	, f_g_12(0)
	*/
{

}

yh_dialog_3::~yh_dialog_3()
{
}

void yh_dialog_3::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT3, brush_size_1);
	DDX_Text(pDX, IDC_EDIT4, f_g_1);
	DDX_Text(pDX, IDC_EDIT5, brush_size_2);
	DDX_Text(pDX, IDC_EDIT6, f_g_2);
	DDX_Text(pDX, IDC_EDIT7, brush_size_3);
	DDX_Text(pDX, IDC_EDIT8, f_g_3);
	DDX_Text(pDX, IDC_EDIT9, brush_size_4);
	DDX_Text(pDX, IDC_EDIT10, f_g_4);
	DDX_Text(pDX, IDC_EDIT12, brush_size_5);
	DDX_Text(pDX, IDC_EDIT13, f_g_5);
	DDX_Text(pDX, IDC_EDIT14, brush_size_6);
	DDX_Text(pDX, IDC_EDIT15, f_g_6);
	DDX_Text(pDX, IDC_EDIT27, brush_size_7);
	DDX_Text(pDX, IDC_EDIT16, f_g_7);
	DDX_Text(pDX, IDC_EDIT17, brush_size_8);
	DDX_Text(pDX, IDC_EDIT18, f_g_8);
	DDX_Text(pDX, IDC_EDIT19, brush_size_9);
	DDX_Text(pDX, IDC_EDIT11, f_g_9);
	DDX_Text(pDX, IDC_EDIT20, brush_size_10);
	DDX_Text(pDX, IDC_EDIT21, f_g_10);
	DDX_Text(pDX, IDC_EDIT23, brush_size_11);
	DDX_Text(pDX, IDC_EDIT24, f_g_11);
	DDX_Text(pDX, IDC_EDIT26, brush_size_12);
	DDX_Text(pDX, IDC_EDIT28, f_g_12);
}


BEGIN_MESSAGE_MAP(yh_dialog_3, CDialogEx)
	ON_BN_CLICKED(65535, &yh_dialog_3::OnBnClicked65535)
	ON_EN_CHANGE(IDC_EDIT24, &yh_dialog_3::OnEnChangeEdit24)
END_MESSAGE_MAP()


// yh_dialog_3 메시지 처리기

std::vector<int> yh_dialog_3::push_back_brush_size(int layer_size)
{
	std::vector<int> v;

	v.assign(layer_size, 0);

	for (int i = 0; i < layer_size; i++)
	{
		switch (i)
		{
		case 0:
			v[i] = brush_size_1;
			break;

		case 1:
			v[i] = brush_size_2;
			break;


		case 2:
			v[i] = brush_size_3;
			break;


		case 3:
			v[i] = brush_size_4;
			break;


		case 4:
			v[i] = brush_size_5;
			break;


		case 5:
			v[i] = brush_size_6;
			break;


		case 6:
			v[i] = brush_size_7;
			break;


		case 7:
			v[i] = brush_size_8;
			break;


		case 8:
			v[i] = brush_size_9;
			break;


		case 9:
			v[i] = brush_size_10;
			break;


		case 10:
			v[i] = brush_size_11;
			break;


		case 11:
			v[i] = brush_size_12;
			break;

		}
	}

	return v;
}

std::vector<float> yh_dialog_3::push_back_f_g_size(int layer_size)
{
	std::vector<float> v;

	v.assign(layer_size, 0);

	for (int i = 0; i < layer_size; i++)
	{
		switch (i)
		{
		case 0:
			v[i] = f_g_1;
			break;

		case 1:
			v[i] = f_g_2;
			break;


		case 2:
			v[i] = f_g_3;
			break;


		case 3:
			v[i] = f_g_4;
			break;


		case 4:
			v[i] = f_g_5;
			break;


		case 5:
			v[i] = f_g_6;
			break;


		case 6:
			v[i] = f_g_7;
			break;


		case 7:
			v[i] = f_g_8;
			break;


		case 8:
			v[i] = f_g_9;
			break;


		case 9:
			v[i] = f_g_10;
			break;


		case 10:
			v[i] = f_g_11;
			break;


		case 11:
			v[i] = f_g_12;
			break;

		}
	}

	return v;
}


void yh_dialog_3::OnBnClicked65535()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void yh_dialog_3::OnEnChangeEdit24()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialogEx::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
