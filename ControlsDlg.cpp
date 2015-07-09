#include "stdafx.h"
#include "ControlsDlg.h"

#include "ChildView.h"
#include "Resource.h"

BEGIN_MESSAGE_MAP(ControlsDlg, CDialog)
	ON_COMMAND(IDC_CHECK_ANIMATE_X, &ControlsDlg::OnCheckAnimateX)
	ON_COMMAND(IDC_CHECK_ANIMATE_Y, &ControlsDlg::OnCheckAnimateY)
	ON_COMMAND(IDC_CHECK_ANIMATE_Z, &ControlsDlg::OnCheckAnimateZ)
	ON_COMMAND(IDC_CHECK_3D, &ControlsDlg::OnCheck3D)
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
	DDX_Control(pDX, IDC_SLIDER_ROTATE_X, m_sliderRotateX);
	DDX_Control(pDX, IDC_SLIDER_ROTATE_Y, m_sliderRotateY);
}

BOOL ControlsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_sliderThresholdLow.SetRange(0, 150);
	m_sliderThresholdHigh.SetRange(0, 150);

	m_sliderThresholdLow.SetPos(75);
	m_sliderThresholdHigh.SetPos(150);

	m_sliderQuantise.SetRange(1, 100);
	m_sliderQuantise.SetPos(1);

	m_sliderRotateX.SetRange(0, 90);
	m_sliderRotateX.SetPos(20);

	m_sliderRotateY.SetRange(0, 360);
	m_sliderRotateY.SetPos(180);

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

void ControlsDlg::OnCheck3D()
{
	m_view.Toggle3D();
}

void ControlsDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	//if (nSBCode != TB_THUMBTRACK && nSBCode != TB_THUMBPOSITION)
	//	return;

	CSliderCtrl* slider = reinterpret_cast<CSliderCtrl*>(pScrollBar);
	nPos = slider->GetPos();

	if (slider == &m_sliderThresholdLow || slider == &m_sliderThresholdHigh)
	{
		const double low = -0.5, high = 1.0;
		double val = low + (high - low) * nPos / 150.0;

		if (slider == &m_sliderThresholdLow)
			m_view.SetThresholdLow(val);
		else
			m_view.SetThresholdHigh(val);
	}
	else if (slider == &m_sliderQuantise)
	{
		m_view.SetQuantise(nPos);
	}
	else if (slider == &m_sliderRotateX)
	{
		m_view.SetRotateX(nPos);
	}
	else if (slider == &m_sliderRotateY)
	{
		m_view.SetRotateY((int)nPos - 180);
	}
}
