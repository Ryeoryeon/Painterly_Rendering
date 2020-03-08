#pragma once


// yh_dialog_3 대화 상자

class yh_dialog_3 : public CDialogEx
{
	DECLARE_DYNAMIC(yh_dialog_3)

public:
	yh_dialog_3(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~yh_dialog_3();


// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG3 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:

	std::vector<int> yh_dialog_3::push_back_brush_size(int layer_size);
	std::vector<float> yh_dialog_3::push_back_f_g_size(int layer_size);
	afx_msg void OnBnClicked65535();
	int brush_size_1;
	float f_g_1;
	int brush_size_2;
	float f_g_2;
	int brush_size_3;
	float f_g_3;
	int brush_size_4;
	float f_g_4;
	int brush_size_5;
	float f_g_5;
	int brush_size_6;
	float f_g_6;
	int brush_size_7;
	float f_g_7;
	int brush_size_8;
	float f_g_8;
	int brush_size_9;
	float f_g_9;
	int brush_size_10;
	float f_g_10;
	int brush_size_11;
	afx_msg void OnEnChangeEdit24();
	float f_g_11;
	int brush_size_12;
	float f_g_12;
};
