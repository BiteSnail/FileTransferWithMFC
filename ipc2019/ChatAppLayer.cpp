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


CChatAppLayer::CChatAppLayer(char* pName)
	: CBaseLayer(pName), Head(nullptr), totalLength(0)
{}

CChatAppLayer::~CChatAppLayer()
{}

void CChatAppLayer::make_frame(unsigned char* ppayload, unsigned short nlength, unsigned char type,int seq){
	unsigned short length = nlength;
	if (type == 0x00 || type == 0x03)length++;
	m_sChatApp.capp_totlen = length;
	m_sChatApp.capp_type = type;
	m_sChatApp.capp_sequence = (unsigned char)seq;
	memset(m_sChatApp.capp_data, 0, length);
	memcpy(m_sChatApp.capp_data, ppayload+(seq * CHAR_DATA_MAX_SIZE), nlength);
	((CEthernetLayer*)(this->GetUnderLayer()))->Send((unsigned char*)&m_sChatApp, length + CHAT_HEADER_SIZE, 0x2080);
}


BOOL CChatAppLayer::Send(unsigned char* ppayload, int nlength)
{
	//message length가 1496bytes 보다 작으면, 바로 하위 레이어로 전달
	if (nlength < CHAR_DATA_MAX_SIZE) {
		make_frame(ppayload, nlength, 0x00, 0);
		return TRUE;
	}
	//message length가 1496bytes 보다 큰 경우 단편화 작업...
	else {
		int length = nlength-CHAR_DATA_MAX_SIZE;	//최초 단편화 frame을 고려해 1496bytes 만큼 뺀다.
		int i = 0;	//몇 번 단편화 하는지 count 변수

		//최초 단편화 된 frame을 만들어 하위 레이어로 전달
		make_frame(ppayload, CHAR_DATA_MAX_SIZE, 0x01, i);
		i++;

		for (; nlength - (i * CHAR_DATA_MAX_SIZE) > CHAR_DATA_MAX_SIZE; i++, length -= (i * CHAR_DATA_MAX_SIZE)) {
			//처음과 끝 부분을 제외한 중간 부분 frame을 만들어 하위 레이어로 전달
			make_frame(ppayload, CHAR_DATA_MAX_SIZE, 0x02, i);
		}
		//마지막 단편화 된 frame을 만들어 하위 레이어로 전달
		make_frame(ppayload, length, 0x03, i);
		return TRUE;
	}
	return FALSE;
}



void CChatAppLayer::add_after(FrameSeq* prev, unsigned char* data, unsigned char seq){
	FrameSeq* tmp = new FrameSeq;
	tmp->data = data;
	tmp->seq = seq;
	tmp->next = prev->next;
	prev->next = tmp;
}

bool CChatAppLayer::seq_compare(FrameSeq* p, unsigned char seq){
	if (p->seq < seq)return true;
	else return false;
}

void CChatAppLayer::add(unsigned char* data, unsigned char seq){
	FrameSeq* p = Head;
	FrameSeq* q = nullptr;
	while (p!=nullptr && seq_compare(p, seq)) {
		q = p;
		p = p->next;
	}
	if (q == nullptr)add_first(data, seq);
	else add_after(q, data, seq);
}

void CChatAppLayer::add_first(unsigned char* data, unsigned char seq) {
	FrameSeq* tmp = new FrameSeq;
	tmp->data = data;
	tmp->seq = seq;
	tmp->next = Head;
	Head = tmp;
}

BOOL CChatAppLayer::Receive(unsigned char* ppayload)
{
	LPCHAT_APP chat_data = (LPCHAT_APP)ppayload;

	if (chat_data->capp_type == 0x00)
	{
		unsigned short size = chat_data->capp_totlen;
		unsigned char *GetBuff=(unsigned char*)malloc(sizeof(unsigned char)*(size));
		//memset(GetBuff, '\0', size);  // GetBuff를 초기화해준다.
		memcpy(GetBuff, chat_data->capp_data, size);
		this->mp_aUpperLayer[0]->Receive(GetBuff);
		return true;
	}else {
		if (chat_data->capp_type == 0x01) {
			add(chat_data->capp_data, chat_data->capp_sequence);
			totalLength = chat_data->capp_totlen;
			return true;
		}
		else if (chat_data->capp_type == 0x02) {
			add(chat_data->capp_data, chat_data->capp_sequence);
			return true;
		}
		else {
			unsigned char* GetBuff = (unsigned char*)malloc(sizeof(unsigned char) * (totalLength));
			//memset(GetBuff, '\0', totalLength);  // GetBuff를 초기화해준다.
			int i = 0;
			for (FrameSeq *_head = Head; _head != nullptr; i++, _head = _head->next) {
				memcpy(GetBuff + (i * CHAR_DATA_MAX_SIZE), _head->data, CHAR_DATA_MAX_SIZE);
			}
			if (totalLength != (i * CHAR_DATA_MAX_SIZE) + (chat_data->capp_totlen))return false;		//모든 frame이 잘 들어왔나 확인
			memcpy(GetBuff + (i * CHAR_DATA_MAX_SIZE), chat_data->capp_data, chat_data->capp_totlen);
			mp_aUpperLayer[0]->Receive(GetBuff);
			deleteList();
			return true;
		}
	}
	return false;
}

void CChatAppLayer::deleteList(){
	FrameSeq* tmp = Head;
	while (tmp != nullptr) {
		Head = Head->next;
		delete tmp;
		tmp = Head;
	}
	Head = nullptr;
}
