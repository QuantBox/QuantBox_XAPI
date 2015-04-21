#pragma once

#include "resource.h"
// CDialogStockDrv dialog

class CDialogStockDrv : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogStockDrv)

public:
	CDialogStockDrv(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDialogStockDrv();

// Dialog Data
	enum { IDD = IDD_DIALOG_STOCKDRV };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
