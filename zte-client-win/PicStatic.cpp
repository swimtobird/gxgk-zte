
/**************************************************************************************
			The Luzj's Zte Project
			//////////////////////
			Copyleft ? 2009 Luzj
		Author:Luzj		QQ:86829232
		http://blog.csdn.net/luzjqq
		Email: luzjcn@gmail.com
	///////////////////////////////////
关于Luzj's Zte认证端的声明：

1、本软件所有涉及的中兴认证的功能的实现均是通过黑盒分析得来，并未通过任何不正当方法获得。

2、本软件仅供研究学习之用，不得使用本软件损害中兴公司商业利益。

3、本软件不可用于任何商业和非法用途，否则责任自负。

4、本软件在发布前均通过一般性应用测试，但不保证任何情况下对机器无害，
由于未知的使用环境或不当的使用对计算机造成的损害，责任由使用者全部承担。

5.本软件版权没有，翻印不究，但请协助改进本作品。

6.本软件属开源软件，如果需要修改本软件源码以进行二次发布，也请继续公开源代码。

由于任何不遵守上叙条例引起的纠纷，均与本人无关，如不同意该声明请不要使用该软件，谢谢合作。
**************************************************************************************/

#include "stdafx.h"
#include "PicStatic.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPicStatic

CPicStatic::CPicStatic()
{
	pPic=NULL;
	nFileSize=0;
	x=y=0;
	GetPic();
}

CPicStatic::~CPicStatic()
{
}


BEGIN_MESSAGE_MAP(CPicStatic, CStatic)
	//{{AFX_MSG_MAP(CPicStatic)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPicStatic message handlers

void CPicStatic::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	if (pPic!=NULL)
	{
		ShowJpeg(pPic,nFileSize,x,y,&dc);
	}
	// Do not call CStatic::OnPaint() for painting messages
}

void CPicStatic::GetPic()
{
	HRSRC picSrc;
	HGLOBAL picGbl;
	if ((picSrc=FindResource(GetModuleHandle(NULL),MAKEINTRESOURCE(133),"jpg"))!=NULL)
	{
		if ((picGbl=LoadResource(GetModuleHandle(NULL),picSrc))!=NULL)
		{
			pPic = LockResource(picGbl);
			nFileSize = SizeofResource(GetModuleHandle(NULL), picSrc);
		}
		else
		{
			MessageBox("无法加载程序顶部图片");
		}
	}
	else
	{
		MessageBox("没找到程序顶部图片");
	}
}

HRESULT CPicStatic::ShowJpeg(LPVOID picData,DWORD dwFileSize,int x, int y,CDC *pDC)
{
	IStream   *pStm;
	IPicture   *pPic;
	BOOL   bResult;     
   
	HGLOBAL   hGlobal   =   GlobalAlloc(GMEM_MOVEABLE,   dwFileSize);     
	LPVOID   pvData   =   NULL;
	if (hGlobal  ==   NULL)
		return   false;
	pvData   =   GlobalLock(hGlobal);
	if ( pvData ==   NULL)  
		return   false;

	memcpy(pvData,picData,dwFileSize);
	GlobalUnlock(hGlobal);
	CreateStreamOnHGlobal(hGlobal,   TRUE,   &pStm);
	bResult=OleLoadPicture(pStm,dwFileSize,TRUE,IID_IPicture,(LPVOID*)&pPic);

	if(FAILED(bResult))
	{
//		CloseHandle(hFile);
		return   false;
	}
	OLE_XSIZE_HIMETRIC   hmWidth;
	OLE_YSIZE_HIMETRIC   hmHeight;
	pPic->get_Width(&hmWidth);     
	pPic->get_Height(&hmHeight);     
	//将图形输出到屏幕上（有点像BitBlt）     
	bResult=pPic->Render(pDC->m_hDC,0,0,x,y,0,hmHeight,hmWidth,-hmHeight,NULL);     

//	CloseHandle(hFile);
	pStm->Release();
	pPic->Release();
	if   (SUCCEEDED(bResult))     
	{
		return   S_OK;     
	}     
	else     
	{     
		return   E_FAIL;     
	}   
}
