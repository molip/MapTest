#include "stdafx.h"
#include "MapTest.h"

#include "ChildView.h"
#include "MainFrm.h"

#include "Jig/Convert.h"
#include "Jig/MemoryDC.h"
#include "Jig/Vectoriser.h"

#include "libKernel/Debug.h"
#include "libKernel/Util.h"

#include <bitset>
#include <sstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using Jig::Convert;

CChildView::CChildView() : m_status{}
{
	//::srand(::GetTickCount());
	::srand(0);
	m_noise = std::make_unique<Jig::Noise>(::rand());
}

CChildView::~CChildView()
{
	DeleteObject(m_bgBitmap);
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
	ON_WM_TIMER()
	ON_COMMAND(ID_ACTIONS_TEST, &CChildView::OnActionsTest)
END_MESSAGE_MAP()

void CChildView::ToggleAnimateX()
{
	m_isAnimateX = !m_isAnimateX;
	UpdateAnimation();
}

void CChildView::ToggleAnimateY()
{
	m_isAnimateY = !m_isAnimateY;
	UpdateAnimation();
}

void CChildView::ToggleAnimateZ()
{
	m_isAnimateZ = !m_isAnimateZ;
	UpdateAnimation();
}

void CChildView::SetThresholdLow(double val)
{
	m_thresholdLow = val;
	InvalidateBitmapIfNotAnimating();
}

void CChildView::SetThresholdHigh(double val)
{
	m_thresholdHigh = val;
	InvalidateBitmapIfNotAnimating();
}

void CChildView::SetQuantise(int val)
{
	m_quantise = val;
	InvalidateBitmapIfNotAnimating();
}

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
	UpdateBitmap();

	Jig::MemoryDC dc(*this);

	//return;

	if (m_bgBitmap)
	{
		CDC dc2;
		dc2.CreateCompatibleDC(&dc);
		HGDIOBJ old = dc2.SelectObject(m_bgBitmap);
		dc.BitBlt(0, 0, GetRect().Width(), GetRect().Height(), &dc2, 0, 0, SRCCOPY);
		dc2.SelectObject(old);
	}
	else
		dc.FillSolidRect(dc.GetRect(), 0xffffff);

	CPen pen(PS_SOLID, 2, 0x00ff00);
	dc.SelectObject(&pen);

	for (auto& pp : m_polypolys)
	{
		for (auto& poly : pp)
		{
			dc.MoveTo(Convert(poly.back() * m_quantise));
			for (auto& point : poly)
			{
				dc.LineTo(Convert(point * m_quantise));
			}
		}
	}
	dc.SelectStockObject(BLACK_PEN);
}

CRect CChildView::GetRect() const
{
	CRect r;
	GetClientRect(r);
	return r;
}

bool CChildView::IsAnimating() const
{
	return m_isAnimateX || m_isAnimateY || m_isAnimateZ;
}

void CChildView::UpdateStatus()
{
	static_cast<CMainFrame*>(::AfxGetMainWnd())->SetStatus(m_status);
}

void CChildView::UpdateAnimation()
{
	if (IsAnimating())
	{
		m_lastFrameTime = GetTickCount();
		SetTimer(1, 33, nullptr);
	}
	else
		KillTimer(1);
}

void CChildView::DestroyBitmap()
{
	if (m_bgBitmap)
	{
		DeleteObject(m_bgBitmap);
		m_bgBitmap = nullptr;
		m_data = nullptr;
		m_bitmapValid = false;
	}
}

void CChildView::InvalidateBitmap()
{
	m_bitmapValid = false;
	Invalidate();
}

void CChildView::InvalidateBitmapIfNotAnimating()
{
	if (!IsAnimating())
		InvalidateBitmap();
}

void CChildView::CreateBitmap()
{
	const CRect rect = GetRect();

	if (rect.IsRectEmpty())
		return;

	BITMAPINFO bi{};
	auto& bih = bi.bmiHeader;
	bih.biSize = sizeof bih;
	bih.biWidth = rect.Width();
	bih.biHeight = -rect.Height();
	bih.biPlanes = 1;
	bih.biBitCount = 32;
	bih.biCompression = BI_RGB;

	m_bgBitmap = ::CreateDIBSection(CClientDC(this), &bi, DIB_RGB_COLORS, (void**)&m_data, nullptr, 0);
	m_bitmapSize = rect.Size();
}

bool CChildView::GetNoise(int x, int y, int interval) const 
{
	double dx = (m_x + x * interval) * m_scale;
	double dy = (m_y + y * interval) * m_scale;
	double dz = m_z * m_scale;
	
	double val = m_noise->Get(dx, dy, dz) * 0.5;

	const double scale2 = 2;
	val += m_noise->Get(dx * scale2, dy * scale2, dz * scale2) * 0.5;

	return val > m_thresholdLow && val < m_thresholdHigh;
}

void CChildView::UpdateBitmap()
{
	if (m_bitmapValid)
		return;

	if (!m_bgBitmap)
		CreateBitmap();

	COLORREF* data = m_data;

	double min = 0, max = 0;

	auto startTime = ::GetTickCount64();

	for (int y = 0; y < m_bitmapSize.cy; ++y)
		for (int x = 0; x < m_bitmapSize.cx; ++x)
		{
			bool val = GetNoise(x / m_quantise, y / m_quantise, m_quantise);
			BYTE c = val ? 127 : 0;

			//min = std::min(min, val);
			//max = std::max(max, val);

			*data = c | (c << 8) | (c << 16) | 0xff000000;
			++data;
		}

	m_status.noiseTime = int(::GetTickCount64() - startTime);

	UpdateTrace();
	UpdateStatus();
}

void CChildView::UpdateTrace()
{
	auto startTime = ::GetTickCount64();

	class Sampler : public Jig::Vectoriser::Sampler
	{
	public:
		Sampler(const COLORREF* data, int width, int interval) : m_data(data), m_width(width), m_interval(interval) {}
		virtual bool Get(int x, int y) const { return (m_data[x * m_interval + y * m_width * m_interval] & 0xffffff) != 0; }
	private:
		const COLORREF* m_data;
		const int m_width, m_interval;
	};

	const int interval = m_quantise;
	Sampler sampler(m_data, m_bitmapSize.cx, interval);

	Jig::Vectoriser v(sampler, m_bitmapSize.cx / interval, m_bitmapSize.cy / interval);
	v.Go();

	m_status.traceTime = int(::GetTickCount64() - startTime);

	m_polypolys = v.GetResults();
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
	DestroyBitmap();
}

BOOL CChildView::OnEraseBkgnd(CDC* pDC)
{
	return true;
}

void CChildView::OnTimer(UINT_PTR nIDEvent)
{
	const double speed = 0.05;

	DWORD now = ::GetTickCount();
	
	double delta = (now - m_lastFrameTime) * speed;
	m_x += delta * m_isAnimateX * 2;
	m_y += delta * m_isAnimateY * 2;
	m_z += delta * m_isAnimateZ;

	m_lastFrameTime = now;

	InvalidateBitmap();
}

void CChildView::OnActionsTest()
{
	CWaitCursor wc;

	std::wostringstream oss;

	int n = 0;
	ULONGLONG start = ::GetTickCount64();
	while (::GetTickCount64() < start + 1000)
	//for (; n < 10; ++n)
	{
		UpdateTrace();
		++n;
	}

	oss << n << L" traces/s\n";

	::AfxMessageBox(oss.str().c_str());
}

