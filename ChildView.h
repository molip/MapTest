#pragma once

#include "Jig/EdgeMesh.h"
#include "Jig/Polygon.h"
#include "Jig/PathFinder.h"

class CChildView : public CWnd
{
public:
	CChildView();
	virtual ~CChildView();

	struct Status
	{
	};

protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	void UpdateStatus();
	CRect GetRect() const;

	Status m_status;
	HBITMAP m_bgBitmap = nullptr;

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnActionsNoise();
};

