#pragma once

#include "Jig/EdgeMesh.h"
#include "Jig/Noise.h"
#include "Jig/Polygon.h"
#include "Jig/PathFinder.h"

#include "libKernel/Singleton.h"

namespace sf
{
	class RenderWindow;
	class Clock;
}

class CChildView : public CWnd, public Kernel::Singleton<CChildView>
{
public:
	CChildView();
	virtual ~CChildView();

	struct Status
	{
		int noiseTime, traceTime;
	};

	void Pump();

	void ToggleAnimateX();
	void ToggleAnimateY();
	void ToggleAnimateZ();
	void Toggle3D();
	void SetThresholdLow(double val);
	void SetThresholdHigh(double val);
	void SetQuantise(int val);
	void SetRotateX(int val);
	void SetRotateY(int val);

	bool GetNoise(int x, int y, int interval) const;

protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	CRect GetRect() const;
	bool IsAnimating() const;
	void UpdateStatus();
	void UpdateAnimation();
	void CreateBitmap();
	void UpdateBitmap();
	void UpdateTrace();
	void DestroyBitmap();
	void InvalidateBitmap();
	void InvalidateBitmapIfNotAnimating();

	Status m_status;
	
	HBITMAP m_bgBitmap = nullptr;
	COLORREF* m_data = nullptr;
	CSize m_bitmapSize = {1000, 1000};
	bool m_bitmapValid = false;
	
	std::vector<Jig::PolyPolygon> m_polypolys;

	std::unique_ptr<Jig::Noise> m_noise;
	DWORD m_lastFrameTime = 0;

	double m_x = 0, m_y = 0, m_z = 0;
	double m_scale = 0.02;
	bool m_isAnimateX = false, m_isAnimateY = false, m_isAnimateZ = false, m_is3D = false;
	double m_thresholdLow = 0.25, m_thresholdHigh = 1.0;
	int m_quantise = 8;

	int m_rotateX = 20, m_rotateY = 0;

	std::unique_ptr<sf::RenderWindow> m_window;

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
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnActionsTest();
};

