#pragma once


// yh_saliency 대화 상자

class yh_saliency : public CDialogEx
{
	DECLARE_DYNAMIC(yh_saliency)

public:
	yh_saliency(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~yh_saliency();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG5 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CButton saliency_yes;
	CButton saliency_no;
	afx_msg void OnBnClickedCancel();
};
