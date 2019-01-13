// demo.cpp : 定义应用程序的入口点。
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

	//必须要调用OleInitialize来初始化运行环境
	HRESULT hRes = OleInitialize(NULL);
	SASSERT(SUCCEEDED(hRes));

	SComMgr2* pComMgr = new SComMgr2(_T("imgdecoder-png"));

	//将程序的运行路径修改到项目所在目录所在的目录
	{
		TCHAR szCurrentDir[MAX_PATH]={0};
		GetModuleFileName( NULL, szCurrentDir, sizeof(szCurrentDir) );
		LPTSTR lpInsertPos = _tcsrchr( szCurrentDir, _T('\\') );
		_tcscpy(lpInsertPos+1,_T("..\\uires\\")); //目录在同一的Debug路径下
		SetCurrentDirectory(szCurrentDir);
	}

	exit:
	{
		if(pComMgr) { delete pComMgr; pComMgr = NULL; }
	}
	OleUninitialize();

	return nRet;
}