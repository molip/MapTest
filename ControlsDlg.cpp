#include "stdafx.h"
#include "ControlsDlg.h"

#include "ChildView.h"
#include "Resource.h"

BEGIN_MESSAGE_MAP(ControlsDlg, CDialog)
	ON_COMMAND(IDC_CHECK_ANIMATE_X, &ControlsDlg::OnCheckAnimateX)
	ON_COMMAND(IDC_CHECK_ANIMATE_Y, &ControlsDlg::OnCheckAnimateY)
	ON_COMMAND(IDC_CHECK_ANIMATE_Z, &ControlsDlg::OnCheckAnimateZ)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()

ControlsDlg::ControlsDlg(CChildView& view) : CDialog(IDD_CONTROLS), m_view(view)
{

}

ControlsDlg::~ControlsDlg()
{
}

void ControlsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_SLIDER_THRESHOLD_LOW, m_sliderThresholdLow);
	DDX_Control(pDX, IDC_SLIDER_THRESHOLD_HIGH, m_sliderThresholdHigh);
	DDX_Control(pDX, IDC_SLIDER_QUANTISE, m_sliderQuantise);
}

BOOL ControlsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_sliderThresholdLow.SetRange(0, 100);
	m_sliderThresholdHigh.SetRange(0, 100);

	m_sliderThresholdLow.SetPos(50);
	m_sliderThresholdHigh.SetPos(100);

	m_sliderQuantise.SetRange(1, 100);
	m_sliderQuantise.SetPos(1);

	return TRUE;
}

void ControlsDlg::OnCheckAnimateX()
{
	m_view.ToggleAnimateX();
}

void ControlsDlg::OnCheckAnimateY()
{
	m_view.ToggleAnimateY();
}

void ControlsDlg::OnCheckAnimateZ()
{
	m_view.ToggleAnimateZ();
}

void ControlsDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	//if (nSBCode != TB_THUMBTRACK && nSBCode != TB_THUMBPOSITION)
	//	return;

	CSliderCtrl* slider = reinterpret_cast<CSliderCtrl*>(pScrollBar);
	nPos = slider->GetPos();

	if (slider == &m_sliderThresholdLow || slider == &m_sliderThresholdHigh)
	{
		const double low = -0.5, high = 0.9;
		double val = low + (high - low) * nPos / 100.0;

		if (slider == &m_sliderThresholdLow)
			m_view.SetThresholdLow(val);
		else
			m_view.SetThresholdHigh(val);
	}
	else if (slider == &m_sliderQuantise)
		m_view.SetQuantise(nPos);

	
}
