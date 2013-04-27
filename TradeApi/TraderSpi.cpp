
#include "stdafx.h"
#include <iostream>
using namespace std;
#include ".\\ThostTraderApi\\ThostFtdcTraderApi.h"
#include "TraderSpi.h"
#include <math.h>
#pragma warning(disable : 4996)
// ���ò���


#include "resource.h"
// �Ự����


void CTraderSpi::OnFrontConnected()
{
	///�û���¼����
	ReqUserLogin();
	m_ConnStatus = true;
	CString str;

}

CTraderSpi::~CTraderSpi()
{
}

CTraderSpi::CTraderSpi(CThostFtdcTraderApi* api, string broker_id, 
					   string investor_id, string passwd)
	:m_pTradeApi(api),m_requestID(0),
	  m_BrokerId(broker_id), m_InvestorId(investor_id), m_Passwd(passwd)
{
	memset(&m_account, 0 ,sizeof(m_account));
	// stl library can be buggy with empty maps. Insert some rubbish data here.
}

void CTraderSpi::ReqUserLogin()
{
	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, m_BrokerId.c_str());
	strcpy(req.UserID, m_InvestorId.c_str());
	strcpy(req.Password, m_Passwd.c_str());
	int iResult = m_pTradeApi->ReqUserLogin(&req, ++m_requestID);
}

void CTraderSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
								CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		// ����Ự����
		m_FrontId = pRspUserLogin->FrontID;
		m_SessionId = pRspUserLogin->SessionID;
		int iNextOrderRef = atoi(pRspUserLogin->MaxOrderRef);
		iNextOrderRef++;
		sprintf(m_OrderRef, "%d", iNextOrderRef);
		ReqSettlementInfoConfirm();
	}
	else if (bIsLast && IsErrorRspInfo(pRspInfo))
	{
		Sleep(1000);
		ReqUserLogin();
	}
}

void CTraderSpi::ReqSettlementInfoConfirm()
{
	CThostFtdcSettlementInfoConfirmField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, m_BrokerId.c_str());
	strcpy(req.InvestorID, m_InvestorId.c_str());
	int iResult = m_pTradeApi->ReqSettlementInfoConfirm(&req, ++m_requestID);
}

void CTraderSpi::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
	}
}


void CTraderSpi::ReqQryInstrument(string instrument)
{
	CThostFtdcQryInstrumentField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.InstrumentID, instrument.c_str());
	int iResult = m_pTradeApi->ReqQryInstrument(&req, ++m_requestID);
}

void CTraderSpi::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (pInstrument == NULL)
	{
		return;
	}

	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		///�����ѯͶ���ֲ߳�
	}
}

void CTraderSpi::ReqQryTradingAccount()
{
	CThostFtdcQryTradingAccountField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, m_BrokerId.c_str());
	strcpy(req.InvestorID, m_InvestorId.c_str());
	int iResult = m_pTradeApi->ReqQryTradingAccount(&req, ++m_requestID);
}

void CTraderSpi::OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	
	if (pTradingAccount == NULL)
	{
		return;
	}


    m_account = *pTradingAccount;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{

	}
}


void CTraderSpi::ReqQryInvestorPosition(string instrument)
{
	CThostFtdcQryInvestorPositionField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, m_BrokerId.c_str());
	strcpy(req.InvestorID, m_InvestorId.c_str());
	strcpy(req.InstrumentID, instrument.c_str());
	int iResult = m_pTradeApi->ReqQryInvestorPosition(&req, ++m_requestID);
	
}

void CTraderSpi::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	
	if ( pInvestorPosition != NULL )
	{
		string key = pInvestorPosition->InstrumentID;
	}
	
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		///����¼������
		//ReqOrderInsert();
	}
}

void CTraderSpi::ReqOrderInsert(CThostFtdcInputOrderField& req)
{
	int iResult = m_pTradeApi->ReqOrderInsert(&req, ++m_requestID);
    int iNextOrderRef = atoi(m_OrderRef);
    iNextOrderRef++;
    sprintf(m_OrderRef, "%d", iNextOrderRef);    
}

void CTraderSpi::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	
	if (pInputOrder == NULL)
	{
		return;
	}

	if (IsErrorRspInfo(pRspInfo))
    {
        if( bIsLast )
        {
            // Error order, shuffle open/close flag
            CThostFtdcInputOrderField req= *pInputOrder;

            strcpy(req.OrderRef, m_OrderRef);
            
            // if cash not enough, try to release cash here
	        if (req.CombOffsetFlag[0] == THOST_FTDC_OF_Open)
            {
		        req.CombOffsetFlag[0] = THOST_FTDC_OF_CloseToday;
	            
                //else
		        //req.CombOffsetFlag[0] = THOST_FTDC_OF_Open;

                req.RequestID = m_requestID;
                ReqOrderInsert(req);
            }
            ReqQryInstrument(req.InstrumentID);
        }
    }
}

void CTraderSpi::ReqOrderAction(CThostFtdcOrderField *pOrder)
{	
	CThostFtdcInputOrderActionField req;
	memset(&req, 0, sizeof(req));
	///���͹�˾����
	strcpy(req.BrokerID, pOrder->BrokerID);
	///Ͷ���ߴ���
	strcpy(req.InvestorID, pOrder->InvestorID);
	///������������
	//	TThostFtdcOrderActionRefType	OrderActionRef;
	///��������
	strcpy(req.OrderRef, pOrder->OrderRef);
	///������
	//	TThostFtdcRequestIDType	RequestID;
	///ǰ�ñ��
	req.FrontID = m_FrontId;
	///�Ự���
	req.SessionID = m_SessionId;
	///����������
	//	TThostFtdcExchangeIDType	ExchangeID;
	///�������
	//	TThostFtdcOrderSysIDType	OrderSysID;
	///������־
	req.ActionFlag = THOST_FTDC_AF_Delete;
	///�۸�
	//	TThostFtdcPriceType	LimitPrice;
	///�����仯
	//	TThostFtdcVolumeType	VolumeChange;
	///�û�����
	//	TThostFtdcUserIDType	UserID;
	///��Լ����
	strcpy(req.InstrumentID, pOrder->InstrumentID);
	
	int iResult = m_pTradeApi->ReqOrderAction(&req, ++m_requestID);
}

void CTraderSpi::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	IsErrorRspInfo(pRspInfo);
}

///����֪ͨ
void CTraderSpi::OnRtnOrder(CThostFtdcOrderField *pOrder)
{
	if (pOrder == NULL)
	{
		return;
	}
	else if (strlen(pOrder->OrderSysID) == 0)
	{
		return;
	}

}

///�ɽ�֪ͨ
void CTraderSpi::OnRtnTrade(CThostFtdcTradeField *pTrade)
{
	if (pTrade == NULL)
	{
		return;
	}

}

void CTraderSpi::OnFrontDisconnected(int nReason)
{
	
	m_ConnStatus = false;

}

void CTraderSpi::OnHeartBeatWarning(int nTimeLapse)
{
}

void CTraderSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	IsErrorRspInfo(pRspInfo);
}

bool CTraderSpi::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
	// ���ErrorID != 0, ˵���յ��˴������Ӧ
	bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	
	if (bResult)
	{

	}
	return bResult;
}

bool CTraderSpi::IsMyOrder(CThostFtdcOrderField *pOrder)
{
	if (pOrder==NULL)
		return false;

	return ((pOrder->FrontID == m_FrontId) &&
		(pOrder->SessionID == m_SessionId) );
}

bool CTraderSpi::IsTradingOrder(CThostFtdcOrderField *pOrder)
{
	if (pOrder==NULL)
		return false;

	return ((pOrder->OrderStatus != THOST_FTDC_OST_PartTradedNotQueueing) &&
		(pOrder->OrderStatus != THOST_FTDC_OST_Canceled) &&
		(pOrder->OrderStatus != THOST_FTDC_OST_AllTraded));
}
