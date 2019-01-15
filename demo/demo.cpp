// demo.cpp : ����Ӧ�ó������ڵ㡣
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

	//����Ҫ����OleInitialize����ʼ�����л���
	HRESULT hRes = OleInitialize(NULL);
	SASSERT(SUCCEEDED(hRes));

	SComMgr2* pComMgr = new SComMgr2(_T("imgdecoder-png"));
{
	//��Ⱦ��ѡ��
	int nType=MessageBox(GetActiveWindow(),_T("ѡ����Ⱦ���ͣ�\n[yes]: Skia\n[no]:GDI\n[cancel]:Quit"),_T("select a render"),MB_ICONQUESTION|MB_YESNOCANCEL);
	if(nType == IDCANCEL)
	{
		nRet = -1;
		goto exit;
	}
	
	//UI��Ⱦģ�飬��render-gdi.dll����render-skia.dll�ṩ
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

	//����ͼ�������
	CAutoRefPtr<IImgDecoderFactory> pImgDecoderFactory; //ͼƬ����������imagedecoder-wid.dllģ���ṩ
	bLoaded=pComMgr->CreateImgDecoder((IObjRef**)&pImgDecoderFactory);
	SASSERT_FMT(bLoaded,_T("load interface [%s] failed!"),_T("imgdecoder"));
	
	//��������ת����
	CAutoRefPtr<ITranslatorMgr> trans;                  //�����Է���ģ�飬��translator.dll�ṩ
	bLoaded=pComMgr->CreateTranslator((IObjRef**)&trans);
	SASSERT_FMT(bLoaded,_T("load interface [%s] failed!"),_T("translator"));


	//��־, log4z����
	CAutoRefPtr<ILog4zManager>  pLogMgr;                
	if(pComMgr->CreateLog4z((IObjRef**)&pLogMgr)) {
		if(pLogMgr) {
			pLogMgr->createLogger("soui");//support output soui trace infomation to log
			pLogMgr->start();
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////////
	//Ϊ��Ⱦģ����������Ҫ���õ�ͼƬ����ģ��
	pRenderFactory->SetImgDecoderFactory(pImgDecoderFactory);

	//����Ψһ��SApplication���󣬹�������Ӧ�ó�����Դ
	SApplication* theApp = new SApplication(pRenderFactory, hInstance);

	theApp->SetLogManager(pLogMgr);
	//
	SLOG_INFO("test="<<200);
	SLOGFMTE("log output using ansi format,str=%s, tick=%u","test",GetTickCount());
	SLOGFMTE(L"log output using unicode format,str=%s, tick=%u",L"����",GetTickCount());

	/////////////////////////////////////////////////////////////////////////////////////////////
	//�ؼ�ע��Ҫ����AddResProviderǰ
	//...
	SSkinGif::Gdiplus_Startup();

	//�����Ҫ�ڴ�����ʹ��R::id::namedid���ַ�ʽ��ʹ�ÿؼ�����Ҫ��һ�д��룺2016��2��2�գ�
	//R::id::namedXmlID����uiresbuilder ����-h .\res\resource.h idtable ��3�����������ɵġ�
	//theApp->InitXmlNamedID(namedXmlID,ARRAYSIZE(namedXmlID),TRUE);


	//�����������·���޸ĵ���Ŀ����Ŀ¼���ڵ�Ŀ¼
	//������Խ�һ�����Ƴ����ӵķ�ʽ��������ָ����һĿ¼
	{
		TCHAR szCurrentDir[MAX_PATH]={0};
		GetModuleFileName( NULL, szCurrentDir, sizeof(szCurrentDir) );
		LPTSTR lpInsertPos = _tcsrchr( szCurrentDir, _T('\\') );
		_tcscpy(lpInsertPos+1,_T("..\\uires\\")); //Ŀ¼��ͬһ��Debug·����
		SetCurrentDirectory(szCurrentDir);
	}
	//����һ�˸���Դ�ṩ����,SOUIϵͳ��ʵ����3����Դ���ط�ʽ��
	//�ֱ��Ǵ��ļ����أ���EXE����Դ���ؼ���ZIPѹ��������
	CAutoRefPtr<IResProvider> pResProvider;
#if (RES_TYPE == 0)//���ļ�����
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
#elif (RES_TYPE==1)//��EXE��Դ����
	CreateResProvider(RES_PE,(IObjRef**)&pResProvider);
	pResProvider->Init((WPARAM)hInstance,0);
#elif (RES_TYPE==2)//��ZIP������
	bLoaded=pComMgr->CreateResProvider_ZIP((IObjRef**)&pResProvider);
	SASSERT_FMT(bLoaded,_T("load interface [%s] failed!"),_T("resprovider_zip"));

	ZIPRES_PARAM param;
	param.ZipFile(pRenderFactory, _T("uires.zip"),"souizip");
	bLoaded = pResProvider->Init((WPARAM)&param,0);
	SASSERT(bLoaded);
#endif
	//��������IResProvider����SApplication����
	theApp->AddResProvider(pResProvider);

	//Ƥ��������
	SSkinLoader *SkinLoader = new SSkinLoader(theApp);
	SkinLoader->LoadSkin(_T("themes\\skin1"));


	//����ϵͳ��Դ
	HMODULE hSysResource=LoadLibrary(SYS_NAMED_RESOURCE);
	if(hSysResource)
	{
		CAutoRefPtr<IResProvider> sysSesProvider;
		CreateResProvider(RES_PE,(IObjRef**)&sysSesProvider);
		sysSesProvider->Init((WPARAM)hSysResource,0);
		theApp->LoadSystemNamedResource(sysSesProvider);
	}
	//����hook���Ʋ˵��ı߿�
	CMenuWndHook::InstallHook(hInstance,L"_skin.sys.menu.border");

	SNotifyCenter *pNotifyCenter = new SNotifyCenter;
	{
		//������ʾ���ڲ���
		CTipWnd::SetLayout(_T("layout:dlg_tip"));

		//��������ʾʹ��SOUI����Ӧ�ó��򴰿�,Ϊ�˱��洰�ڶ�������������������󣬰���������һ�㡣
		CMainDlg dlgMain;  
		dlgMain.Create(GetActiveWindow(), 0,0,888,650);

		dlgMain.GetNative()->SendMessage(WM_INITDIALOG);
		dlgMain.CenterWindow();
		dlgMain.ShowWindow(SW_SHOWNORMAL);

		SmileyCreateHook  smileyHook; //��֪��MainDlg���Ŀ��mhook��ͻ�ˣ���win10�У����hook�ŵ�dlgmain.createǰ�ᵼ��hookʧ�ܡ�
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