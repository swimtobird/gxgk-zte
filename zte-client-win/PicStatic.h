#if !defined(AFX_PicStatic_H__51FE87F5_60DB_4B64_AFC5_4300136BFECE__INCLUDED_)
#define AFX_PicStatic_H__51FE87F5_60DB_4B64_AFC5_4300136BFECE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PicStatic.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPicStatic window

class CPicStatic : public CStatic
{
private:
// Construction
public:
	CPicStatic();

// Attributes
public:
	LPVOID	pPic;
	DWORD	nFileSize;
	int		x,y;
// Operations
public:
	void	GetPic();
	HRESULT	ShowJpeg(LPVOID picData,DWORD dwFileSize,int x, int y,CDC *pDC);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPicStatic)
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPicStatic();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPicStatic)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PicStatic_H__51FE87F5_60DB_4B64_AFC5_4300136BFECE__INCLUDED_)
