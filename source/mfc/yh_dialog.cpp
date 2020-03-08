// yh_dialog.cpp: 구현 파일
//

#include "pch.h"
#include "Painterly_Rendering_mfc.h"
#include "yh_dialog.h"
#include "afxdialogex.h"


// yh_dialog 대화 상자

IMPLEMENT_DYNAMIC(yh_dialog, CDialogEx)

yh_dialog::yh_dialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
	, input_layer_size(7), radioMode(0)
{

}

yh_dialog::~yh_dialog()
{
}

void yh_dialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, input_layer_size);
	DDX_Radio(pDX, IDC_RADIO1, (int&)radioMode);
}


BEGIN_MESSAGE_MAP(yh_dialog, CDialogEx)
	ON_BN_CLICKED(IDC_RADIO1, &yh_dialog::OnBnClickedRadio1)
	//ON_CONTROL_RANGE(BN_CLICKED, IDC_RADIO1, IDC_RADIO2, RadioCtrl)
END_MESSAGE_MAP()


// yh_dialog 메시지 처리기


void yh_dialog::OnBnClickedRadio1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


/*
int yh_dialog::RadioCtrl()
{
	UpdateData(TRUE);

	switch (radioMode)
	{
	case 0:
		return 0;
		break;

	case 1:
		return 1;
		break;

	default:
		break;
	}
}
*/