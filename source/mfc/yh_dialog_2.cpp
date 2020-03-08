// yh_dialog_2.cpp: 구현 파일
//

#include "pch.h"
#include "Painterly_Rendering_mfc.h"
#include "yh_dialog_2.h"
#include "afxdialogex.h"


// yh_dialog_2 대화 상자

IMPLEMENT_DYNAMIC(yh_dialog_2, CDialogEx)

yh_dialog_2::yh_dialog_2(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG2, pParent)
{

}

yh_dialog_2::~yh_dialog_2()
{
}

void yh_dialog_2::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, T_value);
	DDX_Text(pDX, IDC_EDIT2, K_value);
}


BEGIN_MESSAGE_MAP(yh_dialog_2, CDialogEx)
	ON_BN_CLICKED(IDOK, &yh_dialog_2::OnBnClickedOk)
END_MESSAGE_MAP()


// yh_dialog_2 메시지 처리기



void yh_dialog_2::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnOK();
}
