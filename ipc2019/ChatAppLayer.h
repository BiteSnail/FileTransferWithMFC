#pragma once
// ChatAppLayer.h: interface for the CChatAppLayer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHATAPPLAYER_H__E78615DE_0F23_41A9_B814_34E2B3697EF2__INCLUDED_)
#define AFX_CHATAPPLAYER_H__E78615DE_0F23_41A9_B814_34E2B3697EF2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseLayer.h"
#include "pch.h"
class CChatAppLayer
	: public CBaseLayer
{
private:
	inline void		ResetHeader();
	CObject* mp_Dlg;
	//
	unsigned short totalLength;
	struct FrameSeq	{
		unsigned char* data;
		unsigned char seq;
		FrameSeq* next;
	};
	FrameSeq* Head;
	void	add_after(FrameSeq* prev, unsigned char* data, unsigned char seq);
	bool	seq_compare(FrameSeq* p, unsigned char seq);
	void	add(unsigned char*data, unsigned char seq);

public:
	BOOL			Receive(unsigned char* ppayload);
	BOOL			Send(unsigned char* ppayload, int nlength);
	unsigned int	GetDestinAddress();
	unsigned int	GetSourceAddress();
	void			SetDestinAddress(unsigned int dst_addr);
	void			SetSourceAddress(unsigned int src_addr);

	CChatAppLayer(char* pName);
	virtual ~CChatAppLayer();

	typedef struct _CHAT_APP_HEADER {

		unsigned int	app_dstaddr; // destination address of application layer
		unsigned int	app_srcaddr; // source address of application layer
		unsigned short	app_length; // total length of the data
		unsigned char	app_type; // type of application data
		unsigned char	app_data[APP_DATA_SIZE]; // application data

	} CHAT_APP_HEADER, * PCHAT_APP_HEADER;



	typedef struct _CHAT_APP
	{
		unsigned short capp_totlen; // message length
		unsigned char capp_type; // message type. 0x00: 단편화 X, 0x01: 단편화 시작, 0x02: 단편화 중간, 0x03: 단편화 끝
		unsigned char capp_sequence; // 단편화 순서. 
		unsigned char capp_data[CHAR_DATA_MAX_SIZE];
	} CHAT_APP, * LPCHAT_APP;
protected:
	CHAT_APP_HEADER		m_sHeader; // 지워야 하는 것
	CHAT_APP	m_sChatApp;

	enum {
		DATA_TYPE_CONT = 0x01,
		DATA_TYPE_END = 0x02
	};
};

#endif // !defined(AFX_CHATAPPLAYER_H__E78615DE_0F23_41A9_B814_34E2B3697EF2__INCLUDED_)










