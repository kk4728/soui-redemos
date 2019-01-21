#pragma once
#include <core\swnd.h>

namespace SOUI
{


class SMyCtrl :
	public SWindow
{
	SOUI_CLASS_NAME(SMyCtrl, L"myCtrl")
public:
	SMyCtrl(void);
	~SMyCtrl(void);

protected:
	virtual UINT OnGetDlgCode(){
		return SC_WANTALLKEYS;
	}

	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void OnPaint(IRenderTarget *pRT);
	void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	void OnLButtonDown(UINT nFlags, CPoint point);

	SOUI_MSG_MAP_BEGIN()

		MSG_WM_CHAR(OnChar)
		MSG_WM_KEYDOWN(OnKeyDown)
		MSG_WM_PAINT_EX(OnPaint)
		MSG_WM_LBUTTONDOWN(OnLButtonDown)

	SOUI_MSG_MAP_END()

private:
	SStringW  m_strInput;
	SArray<CPoint> m_lstClick;
};


}
