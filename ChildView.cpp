#include "stdafx.h"
#include "MapTest.h"
#include "ChildView.h"
#include "MainFrm.h"

#include "Jig/Convert.h"
#include "Jig/MemoryDC.h"
#include "Jig/Noise.h"

#include "libKernel/Debug.h"

#include <sstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using Jig::Convert;

CChildView::CChildView() 
{
}

CChildView::~CChildView()
{
}

BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_ACTIONS_NOISE, &CChildView::OnActionsNoise)
END_MESSAGE_MAP()

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);

	return TRUE;
}

void CChildView::OnPaint() 
{
	Jig::MemoryDC dc(*this);

	if (m_bgBitmap)
	{
		CDC dc2;
		dc2.CreateCompatibleDC(&dc);
		HGDIOBJ old = dc2.SelectObject(m_bgBitmap);
		dc.BitBlt(0, 0, GetRect().Width(), GetRect().Height(), &dc2, 0, 0, SRCCOPY);
		dc2.SelectObject(old);
		return;
	}
	else
		dc.FillSolidRect(dc.GetRect(), 0xffffff);
}

CRect CChildView::GetRect() const
{
	CRect r;
	GetClientRect(r);
	return r;
}

void CChildView::UpdateStatus()
{
	static_cast<CMainFrame*>(::AfxGetMainWnd())->SetStatus(m_status);
}

void CChildView::OnRButtonDown(UINT nFlags, CPoint point)
{
}

void CChildView::OnLButtonDown(UINT nFlags, CPoint point)
{
}

void CChildView::OnLButtonUp(UINT nFlags, CPoint point)
{
}

void CChildView::OnRButtonUp(UINT nFlags, CPoint point)
{
}

void CChildView::OnMouseMove(UINT nFlags, CPoint point)
{
}

void CChildView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
}

BOOL CChildView::OnEraseBkgnd(CDC* pDC)
{
	return true;
}

void CChildView::OnActionsNoise()
{
	if (m_bgBitmap)
		DeleteObject(m_bgBitmap);

	const CRect rect = GetRect();

	BITMAPINFO bi{};
	auto& bih = bi.bmiHeader;
	bih.biSize = sizeof bih;
	bih.biWidth = rect.Width();
	bih.biHeight = -rect.Height();
	bih.biPlanes = 1;
	bih.biBitCount = 32;
	bih.biCompression = BI_RGB;

	COLORREF* data = nullptr;
	m_bgBitmap = ::CreateDIBSection(CClientDC(this), &bi, DIB_RGB_COLORS, (void**)&data, nullptr, 0);

	::srand(::GetTickCount());
	Jig::Noise noise(::rand());

	const double scale = 0.01;

	for (int y = 0; y < rect.Height(); ++y)
		for (int x = 0; x < rect.Width(); ++x)
		{
			double val = noise.Get(x * scale, y * scale);
			//BYTE c = BYTE((val + 1) * 127.5);
			BYTE c = (val < 0.2 && val > -0.2) ? 0 : 255;
			*data = c | (c << 8) | (c << 16) | 0xff000000;
			++data;
		}
	Invalidate();
}
