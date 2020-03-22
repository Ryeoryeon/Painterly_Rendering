// yh_saliency.cpp: 구현 파일
//

#include "pch.h"
#include "Painterly_Rendering_mfc.h"
#include "yh_saliency.h"
#include "afxdialogex.h"


// yh_saliency 대화 상자

IMPLEMENT_DYNAMIC(yh_saliency, CDialogEx)

yh_saliency::yh_saliency(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG5, pParent)
{

}

yh_saliency::~yh_saliency()
{
}

void yh_saliency::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, saliency_yes);
	DDX_Control(pDX, IDCANCEL, saliency_no);
}


BEGIN_MESSAGE_MAP(yh_saliency, CDialogEx)
	ON_BN_CLICKED(IDOK, &yh_saliency::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &yh_saliency::OnBnClickedCancel)
END_MESSAGE_MAP()


// yh_saliency 메시지 처리기


void yh_saliency::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnOK();
}


void yh_saliency::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnCancel();
}
