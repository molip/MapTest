#include "stdafx.h"
#include "MapTest.h"

#include "ChildView.h"
#include "MainFrm.h"

#include "Jig/Convert.h"
#include "Jig/Material.h"
#include "Jig/MemoryDC.h"
#include "Jig/Triangulator.h" 
#include "Jig/Vectoriser.h"

#include "libKernel/Debug.h"
#include "libKernel/Util.h"

#include <bitset>
#include <sstream>

#include <gl/glu.h>

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

SINGLETON(CChildView)

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

void CChildView::Pump()
{
	sf::Event event;
	if (m_window)
		while (m_window->pollEvent(event));
}

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

void CChildView::Toggle3D()
{
	m_is3D = !m_is3D;
	Invalidate();
}

void CChildView::SetThresholdLow(double val)
{
	if (m_thresholdLow == val)
		return;

	m_thresholdLow = val;
	InvalidateBitmapIfNotAnimating();
}

void CChildView::SetThresholdHigh(double val)
{
	if (m_thresholdHigh == val)
		return;

	m_thresholdHigh = val;
	InvalidateBitmapIfNotAnimating();
}

void CChildView::SetQuantise(int val)
{
	m_quantise = val;
	InvalidateBitmapIfNotAnimating();
}

void CChildView::SetRotateX(int val)
{
	m_rotateX = val;
	Invalidate();
}

void CChildView::SetRotateY(int val)
{
	m_rotateY = val;
	Invalidate();
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

	CPaintDC dc(this);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	const auto viewSize = m_window->getSize();
	glViewport(0, 0, viewSize.x, viewSize.y);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, viewSize.x / (double)viewSize.y, 1, 10000);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	const auto sz = m_bitmapSize;

	if (m_is3D)
	{
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glEnable(GL_CULL_FACE);
		glShadeModel(GL_FLAT);
		glEnable(GL_DEPTH_TEST);

		float amb[] = { 0.2f, 0.2f, 0.2f, 0 };
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);

		float light0[] = { -1, 0, 1, 0 };
		glLightfv(GL_LIGHT0, GL_POSITION, light0);

		glTranslated(0, 0, -sz.cy);
		glRotated(m_rotateX, 1, 0, 0);
		glRotated(m_rotateY, 0, 1, 0);
		glTranslated(-(sz.cx / 2.0), 0, -(sz.cy / 2.0));

		Jig::Material(Jig::Colour(1, 0.5f, 0)).Apply();

		glNormal3d(0, 1, 0);
		glBegin(GL_TRIANGLE_STRIP);
		glVertex3d(sz.cx, 0, 0);
		glVertex3d(0, 0, 0);
		glVertex3d(sz.cx, 0, sz.cy);
		glVertex3d(0, 0, sz.cy);
		glEnd();

		Jig::Material(Jig::Colour(0.5f, 1, 0)).Apply();

		for (auto& pp : m_polypolys)
		{
			Jig::Triangulator triangulator(pp.front());

			//for (auto& poly : pp)
			//{
			//	if (&poly != &pp.front())
			//	{
			//		triangulator.AddHole(poly);
			//	}
			//}

			auto mesh = triangulator.Go();

			glCullFace(GL_FRONT);
			glNormal3d(0, 1, 0);
			for (auto& face : mesh.GetFaces())
			{
				KERNEL_ASSERT(face->GetEdgeCount() == 3);

				glBegin(GL_TRIANGLES);
				for (auto& v : face->GetPointLoop())
				{
					glVertex3d(v.x * m_quantise, 10, v.y * m_quantise);
				}
				glEnd();
			}
			glCullFace(GL_BACK);

			//for (auto& poly : pp)
			auto& poly = pp.front();
			{
				Jig::Vec2 last = poly.back();

				auto DoVert = [&](auto& p)
				{
					Jig::Vec2 vec = Jig::Vec2(p - last).Normalised();
					glNormal3d(vec.y, 0, -vec.x);

					glVertex3d(p.x * m_quantise, 0, p.y * m_quantise);
					glVertex3d(p.x * m_quantise, 10, p.y * m_quantise);

				};

				glBegin(GL_TRIANGLE_STRIP);
				for (auto& point : poly)
				{
					DoVert(point);
					last = point;
				}
				DoVert(poly.front());
				glEnd();
			}
		}
	}
	else
	{
		glDisable(GL_LIGHTING);
		glDisable(GL_DEPTH_TEST);

		glColor3f(1, 1, 1);

		glTranslated(-sz.cx / 2.0, -sz.cy / 2.0, -sz.cy * 1.5);

		for (auto& pp : m_polypolys)
		{
			for (auto& poly : pp)
			{
				if (&poly == &pp.front())
				{
					glColor3f(1, 1, 1);
				}
				else
				{
					glColor3f(1, 0, 0);
				}

				glBegin(GL_LINE_LOOP);
				for (auto& point : poly)
				{
					glVertex3d(point.x * m_quantise, sz.cy - point.y * m_quantise, 0);
				}
				glEnd();
			}
		}
	}

	m_window->display();
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
	BITMAPINFO bi{};
	auto& bih = bi.bmiHeader;
	bih.biSize = sizeof bih;
	bih.biWidth = m_bitmapSize.cx;
	bih.biHeight = -m_bitmapSize.cy;
	bih.biPlanes = 1;
	bih.biBitCount = 32;
	bih.biCompression = BI_RGB;

	m_bgBitmap = ::CreateDIBSection(CClientDC(this), &bi, DIB_RGB_COLORS, (void**)&m_data, nullptr, 0);
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

	m_bitmapValid = true;
}

void CChildView::UpdateTrace()
{
	auto startTime = ::GetTickCount64();

#if 1
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

	Jig::Vectoriser::Options options; //{ Jig::Vectoriser::Option::MergeTouching };
	Jig::Vectoriser v(sampler, m_bitmapSize.cx / interval, m_bitmapSize.cy / interval, options);

#else
	const char* lines[] =
	{
		" #### ",
		" # # #",
		" #####",
		" ### #",
		" #### ",
	};

	class Sampler : public Jig::Vectoriser::Sampler
	{
	public:
		Sampler(const char* lines[]) : m_lines(lines) {}
		virtual bool Get(int x, int y) const { return m_lines[y][x] == '#'; }
	private:
		const char** m_lines;
	};

	Sampler sampler(lines);

	Jig::Vectoriser v(sampler, strlen(lines[0]), _countof(lines));

#endif

	v.Go();

	m_status.traceTime = int(::GetTickCount64() - startTime);

	m_polypolys = v.GetResults();

	for (auto& pp : m_polypolys)
	{
		for (auto& poly : pp)
		{
			poly.RemoveHoles();

			std::vector<Jig::Vec2> vec = poly;
			std::sort(vec.begin(), vec.end(), [](auto& lhs, auto& rhs) { return std::make_pair(lhs.x, lhs.y) < std::make_pair(rhs.x, rhs.y);});

			auto it = std::adjacent_find(vec.begin(), vec.end());
			KERNEL_ASSERT(it == vec.end());
		}
	}
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
	//DestroyBitmap();

	if (cx && cy && !m_window)
	{
		sf::ContextSettings settings(32); // Depth bits.
		m_window = std::make_unique<sf::RenderWindow>(GetSafeHwnd(), settings);
	}
	Invalidate();
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

