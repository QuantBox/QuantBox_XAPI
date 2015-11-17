#include "stdafx.h"

#define ISLIB
#define LIB_TRADER_API_EXPORT

#include <stdio.h>
#include "../include/LTS_v2/SecurityFtdcTraderApi.h"


class MyClassLTS :public CSecurityFtdcTraderApi
{

public:
	///创建TraderApi
	///@param pszFlowPath 存贮订阅信息文件的目录，默认为当前目录
	///@return 创建出的UserApi
	static CSecurityFtdcTraderApi *CreateFtdcTraderApi(const char *pszFlowPath = "");

	///删除接口对象本身
	///@remark 不再使用本接口对象时,调用该函数删除接口对象
	virtual void Release(){}

	///初始化
	///@remark 初始化运行环境,只有调用后,接口才开始工作
	virtual void Init()
	{
		m_pSpi->OnFrontConnected();
	}

	///等待接口线程结束运行
	///@return 线程退出代码
	virtual int Join(){ return 0; }

	///获取当前交易日
	///@retrun 获取到的交易日
	///@remark 只有登录成功后,才能得到正确的交易日
	virtual const char *GetTradingDay(){ return nullptr; }

	///注册前置机网络地址
	///@param pszFrontAddress：前置机网络地址。
	///@remark 网络地址的格式为：“protocol://ipaddress:port”，如：”tcp://127.0.0.1:17001”。 
	///@remark “tcp”代表传输协议，“127.0.0.1”代表服务器地址。”17001”代表服务器端口号。
	virtual void RegisterFront(char *pszFrontAddress)
	{

	}

	///注册回调接口
	///@param pSpi 派生自回调接口类的实例
	virtual void RegisterSpi(CSecurityFtdcTraderSpi *pSpi)
	{
		m_pSpi = pSpi;
	}

	///订阅私有流。
	///@param nResumeType 私有流重传方式  
	///        SECURITY_TERT_RESTART:从本交易日开始重传
	///        SECURITY_TERT_RESUME:从上次收到的续传
	///        SECURITY_TERT_QUICK:只传送登录后私有流的内容
	///@remark 该方法要在Init方法前调用。若不调用则不会收到私有流的数据。
	virtual void SubscribePrivateTopic(SECURITY_TE_RESUME_TYPE nResumeType){}

	///订阅公共流。
	///@param nResumeType 公共流重传方式  
	///        SECURITY_TERT_RESTART:从本交易日开始重传
	///        SECURITY_TERT_RESUME:从上次收到的续传
	///        SECURITY_TERT_QUICK:只传送登录后公共流的内容
	///@remark 该方法要在Init方法前调用。若不调用则不会收到公共流的数据。
	virtual void SubscribePublicTopic(SECURITY_TE_RESUME_TYPE nResumeType){}

	///用户登录请求
	virtual int ReqUserLogin(CSecurityFtdcReqUserLoginField *pReqUserLoginField, int nRequestID)
	{
		char buf[512] = { 0 };
		sprintf(buf,"UserID:%s\nPassword:%s\nUserProductInfo:%s\nAuthCode:%s\nRandCode:%s\n已经复制到剪贴板",
			pReqUserLoginField->UserID,
			pReqUserLoginField->Password,
			pReqUserLoginField->UserProductInfo,
			pReqUserLoginField->AuthCode,
			pReqUserLoginField->RandCode);

		if (OpenClipboard(NULL))
		{
			int len = strlen(buf) + 1;

			HGLOBAL hmem = GlobalAlloc(GHND, len);
			char *pmem = (char*)GlobalLock(hmem);

			EmptyClipboard();
			strcpy(pmem, buf);
			SetClipboardData(CF_TEXT, hmem);
			CloseClipboard();
			GlobalFree(hmem);
		}

		MessageBoxA(nullptr, buf, "", MB_OK);

		return 0;
	}


	///登出请求
	virtual int ReqUserLogout(CSecurityFtdcUserLogoutField *pUserLogout, int nRequestID){ return 0; }

	///获取认证随机码请求
	virtual int ReqFetchAuthRandCode(CSecurityFtdcAuthRandCodeField *pAuthRandCode, int nRequestID)
	{
		CSecurityFtdcAuthRandCodeField body = { 0 };

		strcpy(body.RandCode, "ABCDEF123456");

		m_pSpi->OnRspFetchAuthRandCode(&body, nullptr, nRequestID, true);

		return 0;
	}

	///报单录入请求
	virtual int ReqOrderInsert(CSecurityFtdcInputOrderField *pInputOrder, int nRequestID){ return 0; }

	///报单操作请求
	virtual int ReqOrderAction(CSecurityFtdcInputOrderActionField *pInputOrderAction, int nRequestID){ return 0; }

	///用户口令更新请求
	virtual int ReqUserPasswordUpdate(CSecurityFtdcUserPasswordUpdateField *pUserPasswordUpdate, int nRequestID) { return 0; }

	///资金账户口令更新请求
	virtual int ReqTradingAccountPasswordUpdate(CSecurityFtdcTradingAccountPasswordUpdateField *pTradingAccountPasswordUpdate, int nRequestID) { return 0; }

	///Liber发起出金请求
	virtual int ReqFundOutByLiber(CSecurityFtdcInputFundTransferField *pInputFundTransfer, int nRequestID) { return 0; }

	///资金内转请求
	virtual int ReqFundInterTransfer(CSecurityFtdcFundInterTransferField *pFundInterTransfer, int nRequestID) { return 0; }

private:
	CSecurityFtdcTraderSpi *m_pSpi;
};

CSecurityFtdcTraderApi *CSecurityFtdcTraderApi::CreateFtdcTraderApi(const char *pszFlowPath)
{
	return new MyClassLTS();
}

