#pragma once


class CMdUserApi;
// CDialogStockDrv dialog

class CDialogStockDrv : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogStockDrv)

public:
	LONG OnTSDataDriver(UINT wParam, LONG lParam);

	CDialogStockDrv(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDialogStockDrv();

// Dialog Data
	enum { IDD = IDD_DIALOG_STOCKDRV };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CMdUserApi*					m_pUserApi;
	afx_msg void OnClose();
};
