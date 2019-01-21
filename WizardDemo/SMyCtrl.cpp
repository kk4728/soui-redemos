#include "StdAfx.h"
#include "SMyCtrl.h"

namespace SOUI {


SMyCtrl::SMyCtrl(void)
{
	m_bFocusable = 1;
}


SMyCtrl::~SMyCtrl(void)
{
}

void SMyCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	m_strInput.Append((wchar_t)nChar);
	Invalidate();
}

void SMyCtrl::OnPaint(IRenderTarget *pRT)
{
	SPainter painter;
	BeforePaint(pRT, painter);

	CRect rcClient = GetClientRect();
	if(IsFocusable())
	{
		CRect rcFocus = rcClient;
		rcFocus.DeflateRect(2,2);
		DrawDefFocusRect(pRT, rcFocus);
		
	}
	SStringT strText = S_CW2T(m_strInput);
	pRT->TextOut(rcClient.left, rcClient.top, strText, strText.GetLength());

	if(m_lstClick.GetCount() >= 2) {
		CPoint pts[2] = { m_lstClick[0] + rcClient.TopLeft() };
		for (int i = 1; i<m_lstClick.GetCount(); i++)
		{
			pts[1] = m_lstClick[i] + rcClient.TopLeft();
			pRT->DrawLines(pts, 2);
			pts[0] = pts[1];
		}
	} 
	else if(m_lstClick.GetCount() == 1)
	{
		CPoint pt = m_lstClick[0] + rcClient.TopLeft();
		CRect rc(pt, CSize(8, 8));
		rc.OffsetRect(-4,-4);
		pRT->DrawEllipse(rc);
	}

	AfterPaint(pRT, painter);
}

void SMyCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{

}

void SMyCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	__super::OnLButtonDown(nFlags, point);
	CRect rcClient = GetClientRect();
	CPoint pt1 = point-rcClient.TopLeft();
	
	m_lstClick.Add(pt1);
	Invalidate();
}



}
