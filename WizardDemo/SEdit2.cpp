#include "StdAfx.h"
#include "SEdit2.h"

namespace SOUI {


	SEdit2::SEdit2(void): m_bHandleEnter(FALSE)
	{
		m_evtSet.addEvent(EVENTID(EventKeyEnter));
	}


	SEdit2::~SEdit2(void)
	{
	}

	void SEdit2::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
		if(nChar == VK_RETURN) 
		{
			STRACEA("input enter: ");
		}

		if(nChar == VK_RETURN && m_bHandleEnter) {
			EventKeyEnter evt(this);
			evt.data = 100;
			FireEvent(evt);
		} else {
			SEdit::OnKeyDown(nChar, nRepCnt, nFlags);
		}
	}
}
