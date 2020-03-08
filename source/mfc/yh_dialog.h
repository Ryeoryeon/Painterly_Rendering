#pragma once


// yh_dialog 대화 상자

class yh_dialog : public CDialogEx
{
	DECLARE_DYNAMIC(yh_dialog)

public:
	yh_dialog(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~yh_dialog();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	

	DECLARE_MESSAGE_MAP()
public:
	int input_layer_size;
	UINT radioMode;
	afx_msg void OnBnClickedRadio1();
	//afx_msg int RadioCtrl();
};
