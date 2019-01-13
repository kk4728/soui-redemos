// demo.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "demo.h"

#include <com-loader.hpp>

#ifdef _DEBUG
#define COM_IMGDECODER  _T("imgdecoder-wicd.dll")
#define COM_RENDER_GDI  _T("render-gdid.dll")
#define SYS_NAMED_RESOURCE _T("soui-sys-resourced.dll")
#pragma comment(lib,"lua-52d")
#pragma comment(lib,"scriptmodule-luad")
#else
#define COM_IMGDECODER  _T("imgdecoder-wic.dll")
#define COM_RENDER_GDI  _T("render-gdi.dll")
#define SYS_NAMED_RESOURCE _T("soui-sys-resource.dll")
#pragma comment(lib,"lua-52")
#pragma comment(lib,"scriptmodule-lua")
#endif

int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
	int nRet = 0; 

	//����Ҫ����OleInitialize����ʼ�����л���
	HRESULT hRes = OleInitialize(NULL);
	SASSERT(SUCCEEDED(hRes));

	SComMgr2* pComMgr = new SComMgr2(_T("imgdecoder-png"));

	//�����������·���޸ĵ���Ŀ����Ŀ¼���ڵ�Ŀ¼
	{
		TCHAR szCurrentDir[MAX_PATH]={0};
		GetModuleFileName( NULL, szCurrentDir, sizeof(szCurrentDir) );
		LPTSTR lpInsertPos = _tcsrchr( szCurrentDir, _T('\\') );
		_tcscpy(lpInsertPos+1,_T("..\\uires\\")); //Ŀ¼��ͬһ��Debug·����
		SetCurrentDirectory(szCurrentDir);
	}

	exit:
	{
		if(pComMgr) { delete pComMgr; pComMgr = NULL; }
	}
	OleUninitialize();

	return nRet;
}