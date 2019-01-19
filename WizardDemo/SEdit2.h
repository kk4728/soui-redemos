#pragma once
#include <control\srichedit.h>

namespace SOUI 
{

class EventKeyEnter : public TplEventArgs<EventKeyEnter>
{
	SOUI_CLASS_NAME(EventKeyEnter, L"on_key_enter")
public:
	enum {EventID = EVT_EXTERNAL_BEGIN + 500};
	EventKeyEnter(SObject* pSender) : TplEventArgs<EventKeyEnter>(pSender) {
		data = 0;
	}

	int data;
};

class SEdit2 :
	public SEdit
{
	SOUI_CLASS_NAME(SEdit2, L"edit2")
public:
	SEdit2(void);
	~SEdit2(void);

public:
	SOUI_ATTRS_BEGIN()
		ATTR_BOOL(L"handleEnter", m_bHandleEnter, FALSE)
	SOUI_ATTRS_END()

protected:
	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);


	SOUI_MSG_MAP_BEGIN()
		MSG_WM_KEYDOWN(OnKeyDown)
	SOUI_MSG_MAP_END()



	BOOL m_bHandleEnter;
};

}

