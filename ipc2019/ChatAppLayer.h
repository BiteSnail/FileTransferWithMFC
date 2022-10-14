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
#include<iostream>
#define CHAT_MAX_DATA 1496

class CChatAppLayer
	: public CBaseLayer
{
private:
	unsigned short totalLength;
	struct FrameSeq	{
		unsigned char *data;
		unsigned char seq;
		FrameSeq* next;
	};
	FrameSeq* Head;
	void	add_after(FrameSeq* prev, unsigned char* data, unsigned char seq);
	bool	seq_compare(FrameSeq* p, unsigned char seq);
	void	add(unsigned char*data, unsigned char seq);
	void	add_first(unsigned char* data, unsigned char seq);
	void	make_frame(unsigned char* ppayoad, unsigned short nlength,unsigned char type, int seq);
	void	deleteList();

public:
	BOOL			Receive(unsigned char* ppayload);
	BOOL			Send(unsigned char* ppayload, int nlength);

	CChatAppLayer(char* pName);
	virtual ~CChatAppLayer();

	typedef struct _CHAT_APP
	{
		unsigned short capp_totlen; // message length
		unsigned char capp_type; // message type. 0x00: 단편화 X, 0x01: 단편화 시작, 0x02: 단편화 중간, 0x03: 단편화 끝
		unsigned char capp_sequence; // 단편화 순서. 
		unsigned char capp_data[CHAR_DATA_MAX_SIZE];
	} CHAT_APP, * LPCHAT_APP;
protected:
	CHAT_APP	m_sChatApp;
};

#endif // !defined(AFX_CHATAPPLAYER_H__E78615DE_0F23_41A9_B814_34E2B3697EF2__INCLUDED_)










