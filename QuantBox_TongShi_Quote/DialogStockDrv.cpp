// DialogStockDrv.cpp : implementation file
//

#include "stdafx.h"
#include "QuantBox_TongShi_Quote.h"
#include "DialogStockDrv.h"
#include "afxdialogex.h"

#include "MdUserApi.h"

#define WM_USER_STOCK	2000
// CDialogStockDrv dialog

IMPLEMENT_DYNAMIC(CDialogStockDrv, CDialogEx)

CDialogStockDrv::CDialogStockDrv(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDialogStockDrv::IDD, pParent)
{
	m_pUserApi = nullptr;
}

CDialogStockDrv::~CDialogStockDrv()
{
}

void CDialogStockDrv::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDialogStockDrv, CDialogEx)
	ON_MESSAGE(WM_USER_STOCK, OnTSDataDriver)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CDialogStockDrv message handlers


BOOL CDialogStockDrv::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	if (m_pUserApi)
	{
		m_pUserApi->InitDriver(GetSafeHwnd(), WM_USER_STOCK);
		MoveWindow(0, 0, 0, 0);
		ShowWindow(SW_HIDE);
		ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

LONG CDialogStockDrv::OnTSDataDriver(UINT wParam, LONG lParam)
{
	if (m_pUserApi)
		return m_pUserApi->_OnMsg(wParam, lParam);
	return 1;
}

void CDialogStockDrv::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	if(m_pUserApi)
	{
		m_pUserApi->QuitDriver();
	}

	CDialogEx::OnClose();
}
