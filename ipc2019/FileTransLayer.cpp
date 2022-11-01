#include "stdafx.h"
#include "pch.h"
#include "FileTransLayer.h"

CFileTransLayer::CFileTransLayer(char* pName)
	: CBaseLayer(pName), Head(nullptr), totalLength(0)
{}

CFileTransLayer::~CFileTransLayer()
{}

void CFileTransLayer::make_frame(unsigned char* ppayload, unsigned long nlength, unsigned short type, int seq, unsigned char msg) {
	m_sFileApp.fapp_totlen = nlength;
	m_sFileApp.fapp_type = type;
	m_sFileApp.fapp_seq_num = (unsigned long)seq;
	m_sFileApp.fapp_msg_type = msg;
	memcpy(m_sFileApp.fapp_data, ppayload + (seq * MAX_APP_DATA), nlength);
	((CEthernetLayer*)(this->GetUnderLayer()))->Send((unsigned char*)&m_sFileApp, nlength + SIZE_FILE_HEADER, 0x2090);
}

void CFileTransLayer::SetFilePath(CString strPath)
{
	//파일 경로를 받아옴
	//받아온 파일 경로에서 파일 이름 추출
	CString strFileName;
	strFileName = strPath.Right(strPath.GetLength() - strPath.ReverseFind('\\'));
	/*unsigned char fName = new unsigned char[strFileName.GetLength + 1];
	fName = strFileName;*/

	CString a = _T("");
	a.Format(_T("%s"), strPath);

	CFile file;
	file.Open(a, CFile::modeRead, NULL);
	int size = file.GetLength();
	CArchive ar(&file, CArchive::load);
	unsigned char* content = new unsigned char[size+1];
	ZeroMemory(content,size);// 초기화
	ar.Read(content, size);	//파일 내용 읽기
	Send(content, size, strFileName);
	ar.Close();
	file.Close();
}

BOOL CFileTransLayer::Send(unsigned char* ppayload, int nlength, CString strFileName)
{	
	int flen = strFileName.GetLength();
	unsigned char* fname = new unsigned char[flen];
	memcpy(fname,strFileName,flen );
	//message length가 1488bytes 보다 작으면, 바로 하위 레이어로 전달
	if (nlength < MAX_APP_DATA) {
		make_frame(ppayload, nlength, 0x00, 0, 0);	// fapp_msg_type : 0 내용만
		make_frame(fname, flen, 0x00, 0, 1); // fapp_msg_type : 1 제목만 들어있음
		return TRUE;
	}
	//message length가 1488bytes 보다 큰 경우 단편화 작업...
	else {
		int length = nlength - MAX_APP_DATA;	//최초 단편화 frame을 고려해 1488bytes 만큼 뺀다.
		int i = 0;	//몇 번 단편화 하는지 count 변수

		//최초 단편화 된 frame을 만들어 하위 레이어로 전달
		make_frame(fname, flen, 0x01, 0, 1);	// fapp_msg_type : 1 제목만 들어있음
		make_frame(ppayload, MAX_APP_DATA, 0x01, i, 0); //파일이름 
		i++;

		for (; nlength - (i * MAX_APP_DATA) > MAX_APP_DATA; i++, length -= MAX_APP_DATA) {
			//처음과 끝 부분을 제외한 중간 부분 frame을 만들어 하위 레이어로 전달
			make_frame(ppayload, MAX_APP_DATA, 0x02, i, 0);
		}
		//마지막 단편화 된 frame을 만들어 하위 레이어로 전달
		make_frame(ppayload, length, 0x03, i, 0);
		return TRUE;
	}
	return FALSE;
}

void CFileTransLayer::add_after(FrameSeq* prev, unsigned char* data, unsigned long seq) {
	FrameSeq* tmp = new FrameSeq;
	memcpy(tmp->data,data,sizeof(unsigned char[1488]));
	/*(tmp->data).Format(_T("%s"), data);*/
	tmp->seq = seq;
	tmp->next = prev->next;
	prev->next = tmp;
}

bool CFileTransLayer::seq_compare(FrameSeq* p, unsigned long seq) {
	if (p->seq < seq)return true;
	else return false;
}

void CFileTransLayer::add(unsigned char* data, unsigned long seq) {
	FrameSeq* p = Head;
	FrameSeq* q = nullptr;
	while (seq_compare(p, seq)) {
		q = p;
		p = p->next;
	}
	add_after(q, data, seq);
}

BOOL CFileTransLayer::Receive(unsigned char* ppayload)
{
	LPFILE_APP file_data = (LPFILE_APP)ppayload;

	CString fname;

	if(file_data->fapp_msg_type == 1) fname = GetFname(ppayload);

	CFile file;

	if (file_data->fapp_type == 0x00)
	{	
		
		unsigned char size = file_data->fapp_totlen;
		unsigned char GetBuff[MAX_APP_DATA];
		memset(GetBuff, '\0', MAX_APP_DATA);  // GetBuff를 초기화해준다.
		memcpy(GetBuff, file_data->fapp_data, size);
		//파일 열고 저장
		file.Open(fname, CFile::modeCreate | CFile::modeWrite, NULL);
		file.Write(GetBuff, size);
		file.Close();
		/*CString Msg;

		Msg.Format(_T("%s"), GetBuff);

		this->mp_aUpperLayer[0]->Receive((unsigned char*)Msg.GetBuffer(0));*/
		return true;
	}
	else {
		if (file_data->fapp_type == 0x01) {
			FrameSeq* FirstFrame = new FrameSeq;
			FirstFrame->seq = 0;
			memcpy(FirstFrame->data,file_data->fapp_data ,MAX_APP_DATA);
			//(FirstFrame->data).Format(_T("%s"), file_data->fapp_data);
			FirstFrame->next = nullptr;
			totalLength = file_data->fapp_totlen;
			Head = FirstFrame;

			//파일 생성
			file.Open(fname, CFile::modeCreate, NULL);
			file.Close();
			return true;
		}
		else if (file_data->fapp_type == 0x02) {
			add(file_data->fapp_data, file_data->fapp_seq_num);
			return true;
		}
		else {
			CString mergeMsg = "";
			file.Open(fname, CFile::modeWrite, NULL);
			int i = 0;
			for (FrameSeq* _head = Head; _head != nullptr; i++, _head = _head->next) {
				//mergeMsg = mergeMsg + _head->data;
				mergeMsg = _head->data;
				file.Seek(0, CFile::end);
				file.Write(mergeMsg, mergeMsg.GetLength());
			}
			file.Close();
			/*mp_aUpperLayer[0]->Receive((unsigned char*)mergeMsg.GetBuffer(0));*/
			deleteList();
			return true;
		}
	}
	return false;
}

void CFileTransLayer::deleteList() {
	FrameSeq* tmp = Head;
	while (tmp != nullptr) {
		Head = Head->next;
		delete tmp;
		tmp = Head;
	}
	Head = nullptr;
}

CString CFileTransLayer::GetFname(unsigned char* ppayload)
{
	LPFILE_APP file_data = (LPFILE_APP)ppayload;
	CString fname = _T("");
	unsigned char size = file_data->fapp_totlen;
	unsigned char GetBuff[MAX_APP_DATA];
	memset(GetBuff, '\0', MAX_APP_DATA);  // GetBuff를 초기화해준다.
	memcpy(GetBuff, file_data->fapp_data, size);
	fname.Format(_T("%s"), GetBuff);
	return fname;
}
