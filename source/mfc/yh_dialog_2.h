#pragma once
#include <vector>


// yh_dialog_2 대화 상자

class yh_dialog_2 : public CDialogEx
{
	DECLARE_DYNAMIC(yh_dialog_2)

public:
	yh_dialog_2(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~yh_dialog_2();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG2 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	float T_value;
	float K_value;

	afx_msg void OnBnClickedOk();
};
