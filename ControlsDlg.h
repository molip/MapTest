#pragma once

class CChildView;

class ControlsDlg : public CDialog
{
public:
	ControlsDlg(CChildView& view);
	virtual ~ControlsDlg();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	CChildView& m_view;
	CSliderCtrl m_sliderThresholdLow, m_sliderThresholdHigh, m_sliderQuantise;
	CSliderCtrl m_sliderRotateX, m_sliderRotateY;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnCheckAnimateX();
	afx_msg void OnCheckAnimateY();
	afx_msg void OnCheckAnimateZ();
	afx_msg void OnCheck3D();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL OnInitDialog();
};
