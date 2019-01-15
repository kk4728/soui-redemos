// demo.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "demo.h"

#include <com-loader.hpp>
#include <commgr2.h>
#include <gif/SSkinGif.h>
#include <helper/MenuWndHook.h>
#include <event/NotifyCenter.h>
#include <TipWnd.h>

#include "skin/SSkinLoader.h"
#include "MainDlg.h"

#ifdef _DEBUG
#define SYS_NAMED_RESOURCE _T("soui-sys-resourced.dll")
#pragma comment(lib,"lua-52d")
#pragma comment(lib,"scriptmodule-luad")
#else
#define SYS_NAMED_RESOURCE _T("soui-sys-resource.dll")
#pragma comment(lib,"lua-52")
#pragma comment(lib,"scriptmodule-lua")
#endif


ROBJ_IN_CPP


int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
	int nRet = 0; 
	BOOL bLoaded = FALSE;

	//必须要调用OleInitialize来初始化运行环境
	HRESULT hRes = OleInitialize(NULL);
	SASSERT(SUCCEEDED(hRes));

	SComMgr2* pComMgr = new SComMgr2(_T("imgdecoder-png"));
{
	//渲染器选择
	int nType=MessageBox(GetActiveWindow(),_T("选择渲染类型：\n[yes]: Skia\n[no]:GDI\n[cancel]:Quit"),_T("select a render"),MB_ICONQUESTION|MB_YESNOCANCEL);
	if(nType == IDCANCEL)
	{
		nRet = -1;
		goto exit;
	}
	
	//UI渲染模块，由render-gdi.dll或者render-skia.dll提供
	CAutoRefPtr<IRenderFactory> pRenderFactory;         
	if(nType == IDYES) 
	{
		bLoaded = pComMgr->CreateRender_Skia(reinterpret_cast<IObjRef**>(&pRenderFactory));
	}
	else
	{
		//bLoaded = pComMgr->CreateRender_GDI((IObjRef**)(&pRenderFactory));
		bLoaded = pComMgr->CreateRender_GDI(reinterpret_cast<IObjRef**>(&pRenderFactory));
	}
	SASSERT_FMT(bLoaded, _T("Load interface [%s] failed!"), nType==IDYES?_T("render_skia"):_T("render_gdi"));

	//创建图像解码器
	CAutoRefPtr<IImgDecoderFactory> pImgDecoderFactory; //图片解码器，由imagedecoder-wid.dll模块提供
	bLoaded=pComMgr->CreateImgDecoder((IObjRef**)&pImgDecoderFactory);
	SASSERT_FMT(bLoaded,_T("load interface [%s] failed!"),_T("imgdecoder"));
	
	//创建语言转换器
	CAutoRefPtr<ITranslatorMgr> trans;                  //多语言翻译模块，由translator.dll提供
	bLoaded=pComMgr->CreateTranslator((IObjRef**)&trans);
	SASSERT_FMT(bLoaded,_T("load interface [%s] failed!"),_T("translator"));


	//日志, log4z对象
	CAutoRefPtr<ILog4zManager>  pLogMgr;                
	if(pComMgr->CreateLog4z((IObjRef**)&pLogMgr)) {
		if(pLogMgr) {
			pLogMgr->createLogger("soui");//support output soui trace infomation to log
			pLogMgr->start();
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////////
	//为渲染模块设置它需要引用的图片解码模块
	pRenderFactory->SetImgDecoderFactory(pImgDecoderFactory);

	//定义唯一的SApplication对象，管理整个应用程序资源
	SApplication* theApp = new SApplication(pRenderFactory, hInstance);

	theApp->SetLogManager(pLogMgr);
	//
	SLOG_INFO("test="<<200);
	SLOGFMTE("log output using ansi format,str=%s, tick=%u","test",GetTickCount());
	SLOGFMTE(L"log output using unicode format,str=%s, tick=%u",L"中文",GetTickCount());

	/////////////////////////////////////////////////////////////////////////////////////////////
	//控件注册要放在AddResProvider前
	//...
	SSkinGif::Gdiplus_Startup();

	//如果需要在代码中使用R::id::namedid这种方式来使用控件必须要这一行代码：2016年2月2日，
	//R::id::namedXmlID是由uiresbuilder 增加-h .\res\resource.h idtable 这3个参数后生成的。
	//theApp->InitXmlNamedID(namedXmlID,ARRAYSIZE(namedXmlID),TRUE);


	//将程序的运行路径修改到项目所在目录所在的目录
	//这里可以进一步完善成增加的方式，而不是指定单一目录
	{
		TCHAR szCurrentDir[MAX_PATH]={0};
		GetModuleFileName( NULL, szCurrentDir, sizeof(szCurrentDir) );
		LPTSTR lpInsertPos = _tcsrchr( szCurrentDir, _T('\\') );
		_tcscpy(lpInsertPos+1,_T("..\\uires\\")); //目录在同一的Debug路径下
		SetCurrentDirectory(szCurrentDir);
	}
	//定义一人个资源提供对象,SOUI系统中实现了3种资源加载方式，
	//分别是从文件加载，从EXE的资源加载及从ZIP压缩包加载
	CAutoRefPtr<IResProvider> pResProvider;
#if (RES_TYPE == 0)//从文件加载
	CreateResProvider(RES_FILE,(IObjRef**)&pResProvider);
	if(!pResProvider->Init((LPARAM)_T("uires"),0))
	{
		CreateResProvider(RES_PE,(IObjRef**)&pResProvider);
		if(!pResProvider->Init((WPARAM)hInstance,0))
		{
			SASSERT(0);
			delete theApp;
			nRet = 1;
			goto exit;
		}
	}
#elif (RES_TYPE==1)//从EXE资源加载
	CreateResProvider(RES_PE,(IObjRef**)&pResProvider);
	pResProvider->Init((WPARAM)hInstance,0);
#elif (RES_TYPE==2)//从ZIP包加载
	bLoaded=pComMgr->CreateResProvider_ZIP((IObjRef**)&pResProvider);
	SASSERT_FMT(bLoaded,_T("load interface [%s] failed!"),_T("resprovider_zip"));

	ZIPRES_PARAM param;
	param.ZipFile(pRenderFactory, _T("uires.zip"),"souizip");
	bLoaded = pResProvider->Init((WPARAM)&param,0);
	SASSERT(bLoaded);
#endif
	//将创建的IResProvider交给SApplication对象
	theApp->AddResProvider(pResProvider);

	//皮肤加载器
	SSkinLoader *SkinLoader = new SSkinLoader(theApp);
	SkinLoader->LoadSkin(_T("themes\\skin1"));


	//加载系统资源
	HMODULE hSysResource=LoadLibrary(SYS_NAMED_RESOURCE);
	if(hSysResource)
	{
		CAutoRefPtr<IResProvider> sysSesProvider;
		CreateResProvider(RES_PE,(IObjRef**)&sysSesProvider);
		sysSesProvider->Init((WPARAM)hSysResource,0);
		theApp->LoadSystemNamedResource(sysSesProvider);
	}
	//采用hook绘制菜单的边框
	CMenuWndHook::InstallHook(hInstance,L"_skin.sys.menu.border");

	SNotifyCenter *pNotifyCenter = new SNotifyCenter;
	{
		//设置提示窗口布局
		CTipWnd::SetLayout(_T("layout:dlg_tip"));

		//创建并显示使用SOUI布局应用程序窗口,为了保存窗口对象的析构先于其它对象，把它们缩进一层。
		CMainDlg dlgMain;  
		dlgMain.Create(GetActiveWindow(), 0,0,888,650);

		dlgMain.GetNative()->SendMessage(WM_INITDIALOG);
		dlgMain.CenterWindow();
		dlgMain.ShowWindow(SW_SHOWNORMAL);

		SmileyCreateHook  smileyHook; //不知道MainDlg里哪块和mhook冲突了，在win10中，如果hook放到dlgmain.create前会导致hook失败。
		nRet=theApp->Run(dlgMain.m_hWnd);
	}
	if(pNotifyCenter) delete pNotifyCenter;

}
exit:
	{
		if(pComMgr) { delete pComMgr; pComMgr = NULL; }
	}

	OleUninitialize();

	return nRet;
}