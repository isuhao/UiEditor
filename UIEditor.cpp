// dui-demo.cpp : main source file
//

#include "stdafx.h"
#include "MainDlg.h"
#include "SToolbox.h"
#include "SMyButton.h"
#include "SMoveWnd.h"
#include "SouiRealWndHandler.h"

#include "../controls.extend/SChromeTabCtrl.h"
#include "../controls.extend/simagemaskwnd.h"
#include "../controls.extend/SFreeMoveWindow.h"
//#include "../controls.extend/propgrid/SPropertyGrid.h"

#include "extend.skins/ExtendSkins.h"
#include "extend.ctrls/SButtonEx.h"
#include "extend.ctrls/ExtendCtrls.h"
#include "extend.ctrls/imre/SImRichedit.h"
#include "extend.ctrls/SImagePlayer.h"
#include "extend.skins/SSkinMutiFrameImg.h"
#include "extend.ctrls/SText.h"
#include "extend.ctrls/SImageEx.h"
#include "extend.ctrls/SSplitBar.h"
#include "extend.ctrls/Stabctrl2.h"
#include "extend.ctrls/SAnimImg.h"
#include "..\controls.extend\STurn3DView.h"

#include "SImageSwitcher.h"


//从PE文件加载，注意从文件加载路径位置
#define RES_TYPE 0
//#define RES_TYPE 0   //从文件中加载资源
// #define RES_TYPE 1  //从PE资源中加载UI资源

	
//定义唯一的一个R,UIRES对象,ROBJ_IN_CPP是resource.h中定义的宏。
ROBJ_IN_CPP
	
int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int /*nCmdShow*/)
{
    HRESULT hRes = OleInitialize(NULL);
    SASSERT(SUCCEEDED(hRes));

    int nRet = 0;
    
    SComMgr *pComMgr = new SComMgr;

    HMODULE hSci = LoadLibrary(_T("SciLexer.dll"));


    //将程序的运行路径修改到项目所在目录所在的目录
    TCHAR szCurrentDir[MAX_PATH] = { 0 };
    GetModuleFileName(NULL, szCurrentDir, sizeof(szCurrentDir));
    LPTSTR lpInsertPos = _tcsrchr(szCurrentDir, _T('\\'));
    _tcscpy(lpInsertPos + 1, _T(".."));
    //SetCurrentDirectory(szCurrentDir);
    {
        BOOL bLoaded=FALSE;
        CAutoRefPtr<SOUI::IImgDecoderFactory> pImgDecoderFactory;
        CAutoRefPtr<SOUI::IRenderFactory> pRenderFactory;

        bLoaded = pComMgr->CreateRender_GDI((IObjRef**)&pRenderFactory); //SRenderFactory_GDI
        SASSERT_FMT(bLoaded,_T("load interface [render] failed!"));

        bLoaded=pComMgr->CreateImgDecoder((IObjRef**)&pImgDecoderFactory);//SImgDecoderFactory_GDIP
        SASSERT_FMT(bLoaded,_T("load interface [%s] failed!"),_T("imgdecoder"));

        pRenderFactory->SetImgDecoderFactory(pImgDecoderFactory);//SImgDecoderFactory_GDIP
        SApplication *theApp = new SApplication(pRenderFactory, hInstance);


		//向app注册自定义类
		theApp->RegisterWndFactory(TplSWindowFactory<SMyButton>());
		theApp->RegisterWndFactory(TplSWindowFactory<SMoveWnd>());
		theApp->RegisterWndFactory(TplSWindowFactory<SFreeMoveWindow>());
		theApp->RegisterWndFactory(TplSWindowFactory<SToolbox>());


		theApp->RegisterWndFactory(TplSWindowFactory<STurn3dView>());

		// extened controls
		theApp->RegisterWndFactory(TplSWindowFactory<SChromeTabCtrl>());//注册ChromeTabCtrl
		theApp->RegisterWndFactory(TplSWindowFactory<SImageMaskWnd>());//注册SImageMaskWnd
		theApp->RegisterWndFactory(TplSWindowFactory<SButtonEx>());
		theApp->RegisterWndFactory(TplSWindowFactory<SWindowEx>());
		theApp->RegisterWndFactory(TplSWindowFactory<SImRichEdit>());
		theApp->RegisterWndFactory(TplSWindowFactory<SImagePlayer>());
		theApp->RegisterWndFactory(TplSWindowFactory<SText>());
		theApp->RegisterWndFactory(TplSWindowFactory<SImageEx>());
		theApp->RegisterWndFactory(TplSWindowFactory<SSplitBar>());
		theApp->RegisterWndFactory(TplSWindowFactory<SImageSwitcher>());

		theApp->RegisterWndFactory(TplSWindowFactory<STabPage2>());//注册STabPage2
		theApp->RegisterWndFactory(TplSWindowFactory<STabCtrl2>());//注册STabCtrl2
		theApp->RegisterWndFactory(TplSWindowFactory<SAnimImg>());//注册SAnimImg

        theApp->RegisterWndFactory(TplSWindowFactory<SPropertyGrid>());//注册属性表控件

		//extened skins
		theApp->RegisterSkinFactory(TplSkinFactory<SColorMask>());
		theApp->RegisterSkinFactory(TplSkinFactory<SSkinMutiFrameImg>());
        {
            CAutoRefPtr<IResProvider> sysResProvider;
            CreateResProvider(RES_PE, (IObjRef**)&sysResProvider);
            sysResProvider->Init((WPARAM)hInstance, 0);
            theApp->LoadSystemNamedResource(sysResProvider);
        }

		

        CAutoRefPtr<IResProvider>   pResProvider;
#if (RES_TYPE == 0)
        CreateResProvider(RES_FILE, (IObjRef**)&pResProvider);
        if (!pResProvider->Init((LPARAM)_T("uires"), 0))
        {
            SASSERT(0);
            return 1;
        }
#else 
        CreateResProvider(RES_PE, (IObjRef**)&pResProvider);
        pResProvider->Init((WPARAM)hInstance, 0);
#endif
		//如果需要在代码中使用R::id::namedid这种方式来使用控件必须要这一行代码：2016年2月2日，R::id::namedXmlID是由uiresbuilder 增加-h .\res\resource.h idtable 这3个参数后生成的。
		//theApp->InitXmlNamedID(namedXmlID,ARRAYSIZE(namedXmlID),TRUE);
		//theApp->Init(_T("XML_INIT"));  //这一句不在需要了 在AddResProvider时自动执行初始化
		

		theApp->AddResProvider(pResProvider, L"uidef:UIDESIGNER_XML_INIT");   // theApp->AddResProvider(pResProvider, L"uidef:xml_init");



		//设置真窗口处理接口
		CSouiRealWndHandler * pRealWndHandler = new CSouiRealWndHandler();
		theApp->SetRealWndHandler(pRealWndHandler);
		pRealWndHandler->Release();

        ////加载LUA脚本模块。
        //CAutoRefPtr<IScriptModule> pScriptLua;
        //bLoaded=pComMgr->CreateScrpit_Lua((IObjRef**)&pScriptLua);
        //SASSERT_FMT(bLoaded,_T("load interface [%s] failed!"),_T("script_lua"));

        ////加载多语言翻译模块。
        //CAutoRefPtr<ITranslatorMgr> trans;
        //bLoaded=pComMgr->CreateTranslator((IObjRef**)&trans);
        //SASSERT_FMT(bLoaded,_T("load interface [%s] failed!"),_T("translator"));
        //if(trans)
        //{//加载语言翻译包
        //    theApp->SetTranslator(trans);
        //    pugi::xml_document xmlLang;
        //    if(theApp->LoadXmlDocment(xmlLang,_T("lang_cn"),_T("translator")))
        //    {
        //        CAutoRefPtr<ITranslator> langCN;
        //        trans->CreateTranslator(&langCN);
        //        langCN->Load(&xmlLang.child(L"language"),1);//1=LD_XML
        //        trans->InstallTranslator(langCN);
        //    }
        //}
        
        // BLOCK: Run application
        {
            CMainDlg dlgMain;
            dlgMain.Create(GetActiveWindow());
            dlgMain.SendMessage(WM_INITDIALOG);
            dlgMain.CenterWindow(dlgMain.m_hWnd);
            dlgMain.ShowWindow(SW_SHOWNORMAL);
            nRet = theApp->Run(dlgMain.m_hWnd);
        }

        delete theApp;
    }
	FreeLibrary(hSci);
    
    delete pComMgr;
    
    OleUninitialize();
    return nRet;
}
