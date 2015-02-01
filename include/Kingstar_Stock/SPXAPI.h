#ifndef __SPXAPI_H__
#define __SPXAPI_H__

#include "SPXAPI_struct.h"

#define INTER_FAIL_USER_MESSAGE	-999

#ifdef WIN32
    #ifdef SPXAPI_EXPORTS
    #define SPX_API __declspec(dllexport)
    #else
    #define SPX_API __declspec(dllimport)
    #endif
#else
    #define SPX_API 
#endif

#ifdef __cplusplus
extern "C" {
#endif
	
//API句柄
typedef void *  APIHandle ;


//动态库输出函数


/**
 * @brief 全局初始化
 * @param[in] p_init_para 初始化参数结构
 * @param[out] err_msg 失败时的错误信息
 * @return true为成功，false为失败
 * @details API全局初始化函数，需要且仅需调用一次
 * @see SPX_API_Initialize
 */
SPX_API bool WINAPI SPX_API_Initialize(STInitPara *p_init_para, STRspMsg *err_msg);

/**
 * @brief 全局退出
 * @param[out] err_msg 失败时的错误信息
 * @return true为成功，false为失败
 * @details API全局退出函数，需要且仅需调用一次
 * @see SPX_API_Finalize
 */
SPX_API bool WINAPI SPX_API_Finalize(STRspMsg *err_msg);


/**
 * @brief 创建交易员API句柄 
 * @param[out] err_msg 失败时的错误信息
 * @return API句柄,创建失败时为NULL,创建成功时非NULL
 * @details 
 * @see SPX_API_CreateHandle
 */
SPX_API APIHandle WINAPI SPX_API_CreateHandle(STRspMsg *err_msg);

/**
 * @brief  销毁交易员API句柄，释放资源
 * @param[in] pHandle 要销毁的API句柄指针,不能为NULL
 * @param[out] err_msg 失败时的错误信息
 * @return true为成功，false为失败
 * @details 
 * @see SPX_API_DestroyHandle
 */
SPX_API bool WINAPI SPX_API_DestroyHandle(APIHandle *pHandle, STRspMsg *err_msg);


/**
 * @brief 获取API信息
 * @param[out] p_api_info API信息输出
 * @param[out] err_msg 失败时的错误信息
 * @return true为成功，false为失败
 * @details 查询API信息
 * @see SPX_API_GetApiInfo
 */
SPX_API bool WINAPI SPX_API_GetApiInfo(STApiInfo* p_api_info, STRspMsg * err_msg);


/**
 * @brief  账号登陆校验
 * @param[in] handle API句柄
 * @param[in] p_login 登陆请求信息
 * @param[out] p_login_rsp 登陆返回的客户相关信息
 * @param[out] row_num 登陆返回的客户相关信息的记录数
 * @param[out] err_msg 失败时的错误信息
 * @return true为成功，false为失败
 * @details 账号登陆校验，只有在登陆成功时才返回客户的相关信息。
 * @see SPX_API_Login
 */
SPX_API bool WINAPI SPX_API_Login(APIHandle handle,const STTraderLogin *p_login, STTraderLoginRsp **p_login_rsp,int *row_num,STRspMsg *err_msg);

/**
 * @brief  委托
 * @param[in] handle API句柄
 * @param[in] cust_no 委托的客户号
 * @param[in] market_code 市场代码
 * @param[in] holder_acc_no 股东代码
 * @param[in] order_type 委托类型(0证券买卖/1场内申赎/2ETF申赎/3基金拆分合并)
 * @param[in] p_order 委托请求信息
 * @param[in] order_count 委托记录数
 * @param[in] timeout 委托超时时间，默认取初始化时设置的超时时间
 * @param[out] p_order_rsp 委托返回信息
 * @param[out] row_num 委托返回记录数
 * @param[out] err_msg 失败时的错误信息
 * @return true为成功，false为失败
 * @details 
   1.委托信息p_order中各委托记录作为同一个批次下单，但必须都是同一种委托类型(0证券买卖/1场内申赎/2ETF申赎/3基金拆分合并)
   2.p_order_rsp按委托顺序依次返回同一批次各委托单的处理结果
 * @see SPX_API_Order
 */
SPX_API bool WINAPI SPX_API_Order(APIHandle handle, char *cust_no,char *market_code,char *holder_acc_no,char *order_type,const STOrder *p_order, STOrderRsp **p_order_rsp,int order_count, int *row_num, STRspMsg *err_msg, int timeout=INFINITE-1);

/**
 * @brief  撤单
 * @param[in] handle API句柄
 * @param[in] p_order_cancel 撤单请求信息
 * @param[in] cancel_count 撤单记录数 
 * @param[in] timeout 委托超时时间，默认取初始化时设置的超时时间
 * @param[out] p_order_cancel_rsp 撤单返回信息
 * @param[out] row_num 撤单返回记录数
 * @param[out] err_msg 失败时的错误信息
 * @return true为成功，false为失败
 * @details 
   1.撤单信息p_order_cancel可对多笔委托单进行撤单，可按批号或委托流水号撤单，但不能同时指定批号和委托流水号
   2.p_order_cancel_rsp按撤单顺序依次返回各笔撤单处理结果
 * @see SPX_API_OrderCancel
 */
SPX_API bool WINAPI SPX_API_OrderCancel(APIHandle handle, const STOrderCancel *p_order_cancel, STOrderCancelRsp **p_order_cancel_rsp, int cancel_count, int *row_num, STRspMsg *err_msg, int timeout=INFINITE-1);

/**
 * @brief  查询资金
 * @param[in] handle API句柄
 * @param[in] p_qry_fund 查询资金请求信息
 * @param[out] p_qry_fund_rsp 资金返回信息
 * @param[out] row_num 资金返回记录数
 * @param[out] err_msg 失败时的错误信息
 * @return true为成功，false为失败
 * @details  
 * @see SPX_API_QueryFund
 */
SPX_API bool WINAPI SPX_API_QueryFund(APIHandle handle, STQueryFund *p_qry_fund, STFundInfo **p_qry_fund_rsp, int *row_num, STRspMsg *err_msg);

/**
 * @brief  查询持仓
 * @param[in] handle API句柄
 * @param[in] p_qry_position 查询持仓请求信息
 * @param[out] p_qry_position_rsp 持仓返回信息
 * @param[out] row_num 持仓返回记录数
 * @param[out] err_msg 失败时的错误信息
 * @return true为成功，false为失败
 * @details  
 * @see SPX_API_QueryPosition
 */
SPX_API bool WINAPI SPX_API_QueryPosition(APIHandle handle, STQueryPosition *p_qry_position, STPositionInfo **p_qry_position_rsp, int *row_num, STRspMsg *err_msg);

/**
 * @brief  帐务查询
 * @param[in] handle API句柄
 * @param[in] p_qry_account 帐务查询请求信息
 * @param[out] p_qry_account_rsp 帐务查询返回信息
 * @param[out] row_num 帐务查询返回记录数
 * @param[out] err_msg 失败时的错误信息
 * @return true为成功，false为失败
 * @details  
 * @see SPX_API_QueryAccount
 */
SPX_API bool WINAPI SPX_API_QueryAccount(APIHandle handle, STQueryAccount *p_qry_account, STAccountInfo **p_qry_account_rsp, int *row_num, STRspMsg *err_msg);

/**
 * @brief  查询委托
 * @param[in] handle API句柄
 * @param[in] p_qry_order 查询委托请求信息
 * @param[out] p_qry_order_rsp 查询委托返回信息
 * @param[out] row_num 查询委托返回记录数
 * @param[out] err_msg 失败时的错误信息
 * @return true为成功，false为失败
 * @details  
 * @see SPX_API_QueryOrder
 */ 
SPX_API bool WINAPI SPX_API_QueryOrder(APIHandle handle, STQueryOrder *p_qry_order, STOrderInfo **p_qry_order_rsp, int *row_num, STRspMsg *err_msg);

/**
 * @brief  查询成交
 * @param[in] handle API句柄
 * @param[in] p_qry_done 查询成交请求信息
 * @param[out] p_qry_done_rsp 查询成交返回信息
 * @param[out] row_num 查询成交返回记录数
 * @param[out] err_msg 失败时的错误信息
 * @return true为成功，false为失败
 * @details  
 * @see SPX_API_QueryDone
 */  
SPX_API bool WINAPI SPX_API_QueryDone(APIHandle handle, STQueryDone *p_qry_done, STDoneInfo **p_qry_done_rsp, int *row_num, STRspMsg *err_msg);

/**
 * @brief  查询证券信息
 * @param[in] handle API句柄
 * @param[in] p_qry_seccode 查询证券信息请求信息
 * @param[out] p_qry_seccode_rsp 证券返回信息
 * @param[out] row_num 证券信息返回记录数
 * @param[out] err_msg 失败时的错误信息
 * @return true为成功，false为失败
 * @details  
 * @see SPX_API_QuerySecCode
 */  
SPX_API bool WINAPI SPX_API_QuerySecCode(APIHandle handle, STQuerySecCode *p_qry_seccode, STSecCodeInfo **p_qry_seccode_rsp, int *row_num, STRspMsg *err_msg);

/**
 * @brief  查询ETF基本信息
 * @param[in] handle API句柄
 * @param[in] p_qry_etfbaseinfo 查询ETF基本信息请求信息
 * @param[out] p_qry_etfbaseinfo_rsp ETF基本信息
 * @param[out] row_num ETF基本信息返回记录数
 * @param[out] err_msg 失败时的错误信息
 * @return true为成功，false为失败
 * @details  
 * @see SPX_API_QueryEtfBaseInfo
 */  
SPX_API bool WINAPI SPX_API_QueryEtfBaseInfo(APIHandle handle, STQueryEtfBaseInfo *p_qry_etfbaseinfo, STEtfBaseInfo **p_qry_etfbaseinfo_rsp, int *row_num, STRspMsg *err_msg);

/**
 * @brief  查询ETF篮子信息
 * @param[in] handle API句柄
 * @param[in] p_qry_etfbasketinfo 查询ETF篮子信息请求信息
 * @param[out] p_qry_etfbasketinfo_rsp ETF篮子信息
 * @param[out] row_num ETF篮子信息返回记录数
 * @param[out] err_msg 失败时的错误信息
 * @return true为成功，false为失败
 * @details  
 * @see SPX_API_QueryEtfBasketInfo
 */   
SPX_API bool WINAPI SPX_API_QueryEtfBasketInfo(APIHandle handle, STQueryEtfBasketInfo *p_qry_etfbasketinfo, STEtfBasketInfo **p_qry_etfbasketinfo_rsp, int *row_num, STRspMsg *err_msg);


/**
 * @brief  清除缓存
 * @param[in] handle API句柄
 * @return true为成功，false为失败
 * @details  显式清除请求返回信息的缓存
   各业务请求在返回结果信息时会自动清除上一笔的缓存
 * @see SPX_API_Clear_Buf
 */   
SPX_API bool WINAPI SPX_API_Clear_Buf(APIHandle handle);

/**
 * @brief 参数设置
 * @param[in] type 参数类型
 * @param[in] value 参数值
 * @param[out] err_msg 失败时的错误信息
 * @return true为成功，false为失败
 * @details 对需要启用的API参数进行设置，务必在SPX_API_Initialize前调用
 * @see SPX_API_SetParam
 */
SPX_API bool WINAPI SPX_API_SetParam(PARAM_TYPE type, const char *value, STRspMsg *err_msg);


#ifdef __cplusplus
}
#endif

#endif //__SPXAPI_H__