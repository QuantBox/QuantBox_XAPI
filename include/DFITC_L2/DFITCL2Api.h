/**
 * 版权所有(C)2012-2016, 大连飞创信息技术有限公司
 * 文件名称：DFITCL2Api.h
 * 文件说明：定义level 2行情API接口
 * 当前版本：1.0.6
 * 作者：Datafeed项目组
 * 发布日期：2014年03月14日
 */                         
#ifndef DLMDAPI_H_
#define DLMDAPI_H_

#ifdef WIN32 

#ifdef LEVEL2API_EXPORTS
#define DFITC_L2API_API __declspec(dllexport)
#else
#define DFITC_L2API_API __declspec(dllimport)
#endif

#else
#define DFITC_L2API_API

#endif

#include "DFITCL2ApiDataType.h"

namespace DFITC_L2 {
	class DFITCL2Spi
	{
	public:
		DFITCL2Spi() {}
		/**
		* 连接成功响应:当用户连接成功后，此方法会被调用。
		*/   
		virtual void OnConnected() { }

		/**
		* 连接断开响应:当api与level 2 server失去连接时此方法会被调用。api会自动重新连接，客户端可不做处理
		* /@param nReason 错误原因
		*        1 心跳超时
		*        2 网络断开
		*		 3 收到错误报文
		*/   
		virtual void OnDisconnected(int pReason) { }

		/**
		* 登录请求响应:当用户发出登录请求后，level 2 server返回响应时此方法会被调用，通知用户登录是否成功。
		* @param pErrorField:用户登录返回信息。
		*/           
		virtual void OnRspUserLogin(struct ErrorRtnField * pErrorField) { }

		/**
		* 登出请求响应:当用户发出登出请求后，level 2 server返回响应时此方法会被调用，通知用户登出是否成功。
		* @param pErrorField:用户登出返回信息。
		*/  
		virtual void OnRspUserLogout(struct ErrorRtnField * pErrorField) { }
		
		/**
		* 订阅请求响应:当用户发出订阅请求后，level 2 server返回响应时此方法会被调用，通知用户订阅是否成功。
		* @param pErrorField:用户订阅返回信息。
		*/  
		virtual void OnRspSubscribeMarketData(struct ErrorRtnField * pErrorField) { }

		/**
		* 取消订阅请求响应:当用户发出取消订阅请求后，level 2 server返回响应时此方法会被调用，通知用户取消订阅是否成功。
		* @param pErrorField:取消订阅返回信息。
		*/  
		virtual void OnRspUnSubscribeMarketData(ErrorRtnField * pErrorField) { }

		/**
		* 全部订阅请求响应:当用户发出全部订阅请求后，level 2 server返回响应时此方法会被调用，通知用户全部订阅是否成功。
		* @param pErrorField:用户全部订阅返回信息。
		*/  
		virtual void OnRspSubscribeAll(struct ErrorRtnField * pErrorField) { }

		/**
		* 取消全部订阅请求响应:当用户发出取消全部订阅请求后，level 2 server返回响应时此方法会被调用，通知用户取消全部订阅是否成功。
		* @param pErrorField:取消全部订阅返回信息。
		*/  
		virtual void OnRspUnSubscribeAll(struct ErrorRtnField * pErrorField) { }

		/**
		* 修改密码请求响应:当用户发出修改密码请求后，level 2 server返回响应时此方法会被调用，通知用户修改密码是否成功。
		* @param pErrorField:修改密码返回信息。
		*/  
		virtual void OnRspModifyPassword(struct ErrorRtnField * pErrorField) { }
		
		/**
		* 最优与五档深度行情消息应答:如果订阅行情成功且有行情返回时，该方法会被调用。
		* @param pQuote:指向最优与五档深度行情信息结构的指针，结构体中包含具体的行情信息。
		*/             
		virtual void OnBestAndDeep(MDBestAndDeep * const pQuote) { }
		
		/**
		* 套利行情消息应答:如果订阅行情成功且有行情返回时，该方法会被调用。
		* @param pQuote:套利行情信息结构的指针，结构体中包含具体的行情信息。
		*/ 
		virtual void OnArbi(MDBestAndDeep * const pQuote) { }

		/**
		* 最优价位上十笔委托行情消息应答:如果订阅行情成功且有行情返回时，该方法会被调用。
		* @param pQuote:最优价位上十笔委托行情信息结构的指针，结构体中包含具体的行情信息。
		*/
		virtual void OnTenEntrust(MDTenEntrust * const pQuote) { }

		/**
		* 实时结算价行情消息应答:如果订阅行情成功且有行情返回时，该方法会被调用。
		* @param pQuote:实时结算价行情信息结构的指针，结构体中包含具体的行情信息。
		*/
		virtual void OnRealtime(MDRealTimePrice * const pQuote) { }
		
		/**
		* 加权平均及委托行情消息应答:如果订阅行情成功且有行情返回时，该方法会被调用。
		* @param pQuote:加权平均及委托行情信息结构的指针，结构体中包含具体的行情信息。
		*/
		virtual void OnOrderStatistic(MDOrderStatistic * const pQuote) { }

		/**
		* 分价位成交行情消息应答:如果订阅行情成功且有行情返回时，该方法会被调用。
		* @param pQuote:分价位成交行情信息结构的指针，结构体中包含具体的行情信息。
		*/
		virtual void OnMarchPrice(MDMarchPriceQty * const pQuote) { }

		/**
		* 心跳丢失消息应答:如果与level 2 server心跳丢失或网络出现问题，该方法会被调用。
		*/
		virtual void OnHeartBeatLost() { }

		virtual ~DFITCL2Spi(){}
	};

	class DFITC_L2API_API DFITCL2Api{
		
	public:
	    /**
		* 创建行情API实例
		* @return 创建出的UserApi
		*/   
		static DFITCL2Api *CreateDFITCMdApi();
		
		/**
		* 销毁一个api实例。
		* @return 0 - 成功; 非0 - 失败
		*/           
		static int DestoryDFITCMdApi(DFITCL2Api * & p);
		
		/**
		* 用户发出登陆请求
		* @param pReqUserLoginField:指向用户登录请求结构的地址。
		* @return 0 - 登录成功; 非0 - 失败。
		*/       
		virtual int ReqUserLogin(DFITCUserLoginField * pReqUserLoginField);
		
		/**
		* 连接深度行情服务器:选择接收行情方式0为udp。
		* @param pszSvrAddr:level 2 server网络地址。
		*                  例"tcp://127.0.0.1:10915"
		*                  127.0.0.1为level 2 server地址
		*                  10915为level 2 server服务器接收tcp连接的端口
		* @param pSpi:DFITCL2Spi对象实例地址
		* @param RecvQuoteType:0为udp接收行情，1为tcp接收行情
		* @return 0 - 成功;非0 - 失败。
		*/                         
		virtual int Connect(char * pszSvrAddr, DFITCL2Spi * pSpi, unsigned int RecvQuoteType = 0);
		
		/**
		* 订阅行情:该方法发出订阅某个或者某些合约行情请求。
		* @param ppInstrumentID[]:指针数组，每个指针指向一个合约.  
		* @param nCount:合约个数
		* @return 0 - 成功;非0 - 失败。
		*/                   
		virtual int SubscribeMarketData(char * ppInstrumentID[], int nCount);

		/**
		* 退订行情:该方法发出退订某个/某些合约行情请求。
		* @param ppInstrumentID[]:指针数组，每个指针指向一个合约.  
		* @param nCount:合约个数
		* @return 0 - 成功;非0 - 失败。
		*/            
		virtual int UnSubscribeMarketData(char * ppInstrumentID[], int nCount);
		/**
		* 订阅全部合约行情:该方法发送订阅全部合约请求。
		* @return 0 - 成功;非0 - 失败。
		*/ 
		virtual int SubscribeAll();

		/**
		* 取消订阅全部合约行情:该方法发送取消订阅全部合约请求。
		* @return 0 - 成功;非0 - 失败。
		*/ 
		virtual int UnSubscribeAll();
		
		/**
		* 用户发出登出请求
		* @param pReqUserLogoutField:指向用户登录请出结构的地址。
		* @return 0 - 登出成功; 非0 - 失败。
		*/              
		virtual int ReqUserLogout(DFITCUserLogoutField * pReqUserLogoutField);
		
		/**
		* 用户发出密码修改请求
		* @param pReqUserPasswdChangeField:指向用户密码修改请结构的地址。
		* @return 0 - 登出成功; 非0 - 失败。
		*/  
		virtual int ReqChangePassword(DFITCPasswdChangeField * pReqUserPasswdChangeField);

		virtual ~DFITCL2Api();
	protected:
		DFITCL2Api();
	};
}
// 创建连接深度行情服务器api对象
#define NEW_CONNECTOR()     DFITC_L2::DFITCL2Api::CreateDFITCMdApi()
// 销毁对象
#define DELETE_CONNECTOR(p) DFITC_L2::DFITCL2Api::DestoryDFITCMdApi(p)


#endif
