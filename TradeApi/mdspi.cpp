#include "stdafx.h"
#include "MdSpi.h"
#include <iostream>
#include <stdio.h>
using namespace std;
#pragma warning(disable : 4996)
#include "resource.h"
// USER_API参数
//extern CThostFtdcMdApi* m_pUserApi;

// 配置参数	

// 请求编号


CMdSpi::~CMdSpi()
{

}

CMdSpi::CMdSpi(CThostFtdcMdApi* api, string broker_id, string investor_id, 
			   string passwd)
	:m_pUserApi(api),m_requestID(0), m_BrokerId(broker_id), m_InvestorId(investor_id),
		m_Passwd(passwd), 
		m_ConnStatus(false)
{


}

void CMdSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo,
		int nRequestID, bool bIsLast)
{
	IsErrorRspInfo(pRspInfo);
}

void CMdSpi::OnFrontDisconnected(int nReason)
{
	m_ConnStatus = false;
}

void CMdSpi::OnHeartBeatWarning(int nTimeLapse)
{
}

void CMdSpi::OnFrontConnected()
{
	ReqUserLogin();
	m_ConnStatus = true;
}

void CMdSpi::ReqUserLogin()
{
	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, m_BrokerId.c_str());
	strcpy(req.UserID, m_InvestorId.c_str());
	strcpy(req.Password, m_Passwd.c_str());
	int iResult = m_pUserApi->ReqUserLogin(&req, ++m_requestID);
}

void CMdSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{

	if ( pRspUserLogin== NULL || pRspInfo==NULL)
		return;

	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{

	}
	else if (bIsLast && IsErrorRspInfo(pRspInfo))
	{
		Sleep(1000);
		ReqUserLogin();
	}
}

void CMdSpi::SubscribeMarketData(char* ppInstrumentID[], int iInstrumentID)
{
	int iResult = m_pUserApi->SubscribeMarketData(ppInstrumentID, iInstrumentID);
}

void CMdSpi::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
}

void CMdSpi::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
}

void CMdSpi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{

}

bool CMdSpi::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
	// 如果ErrorID != 0, 说明收到了错误的响应
	bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	return bResult;
}
