// ChatAppLayer.cpp: implemtation of the CChatAppLayer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pch.h"
#include "ChatAppLayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CChatAppLayer::CChatAppLayer(char* pName)
	: CBaseLayer(pName),
	mp_Dlg(NULL)
{
	ResetHeader();
}

CChatAppLayer::~CChatAppLayer()
{

}

void CChatAppLayer::SetSourceAddress(unsigned int src_addr)
{
	m_sHeader.app_srcaddr = src_addr;
}

void CChatAppLayer::SetDestinAddress(unsigned int dst_addr)
{
	m_sHeader.app_dstaddr = dst_addr;
}

void CChatAppLayer::ResetHeader()
{
	m_sHeader.app_srcaddr = 0x00000000;
	m_sHeader.app_dstaddr = 0x00000000;
	m_sHeader.app_length = 0x0000;
	m_sHeader.app_type = 0x00;
	memset(m_sHeader.app_data, 0, APP_DATA_SIZE);
}

unsigned int CChatAppLayer::GetSourceAddress()
{
	return m_sHeader.app_srcaddr;
}

unsigned int CChatAppLayer::GetDestinAddress()
{
	return m_sHeader.app_dstaddr;
}


BOOL CChatAppLayer::Send(unsigned char* ppayload, int nlength)
{
	if (nlength < CHAR_DATA_MAX_SIZE) {
		m_sChatApp.capp_totlen = (unsigned short)(nlength + CHAT_HEADER_SIZE);
		m_sChatApp.capp_type = 0x00;
		memcpy(m_sChatApp.capp_data, ppayload, nlength);
		this->GetUnderLayer()->Send((unsigned char*)&m_sChatApp, nlength + CHAT_HEADER_SIZE);
		return TRUE;
	}
	/*else {
		int length = nlength;
		while (length < CHAR_DATA_MAX_SIZE) {
			if (length == nlength) {
				m_sChatApp.capp_totlen = (unsigned short)(CHAT_HEADER_SIZE+CHAR_DATA_MAX_SIZE);
				m_sChatApp.capp_type = 0x01;
				memcpy(m_sChatApp.capp_data, ppayload, CHAR_DATA_MAX_SIZE);
				this->GetUnderLayer()->Send((unsigned char*)&m_sChatApp, CHAT_HEADER_SIZE + CHAR_DATA_MAX_SIZE);
			}
			else {
				m_sChatApp.capp_totlen
			}
		}

	}*/
	return FALSE;
}



BOOL CChatAppLayer::Receive(unsigned char* ppayload)
{
	LPCHAT_APP chat_data = (LPCHAT_APP)ppayload;

	if (chat_data->capp_type == 0x00)
	{
		unsigned char size = chat_data->capp_totlen;
		unsigned char* GetBuff = nullptr;
		memset(GetBuff, '\0', size);  // GetBuff를 초기화해준다.
		memcpy(GetBuff, chat_data->capp_data, size);
		this->mp_aUpperLayer[0]->Receive(chat_data->capp_data);
		return TRUE;
	}
	return FALSE;
}

