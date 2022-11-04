#include "pch.h"
#include "FileTransLayer.h"

FileTransLayer::FileTransLayer(char* name):CBaseLayer(name) {
	FilePath = "";
	mHeader.fapp_totlen = 0;
	mHeader.fapp_type = 0;
	mHeader.fapp_seq_num = 0;
	mHeader.faa_msg_type = 0;
	mHeader.unused = 0;
	memset(mHeader.fapp_data, 0, MAX_APP_DATA);
	return;
}

FileTransLayer::~FileTransLayer() {
	return;
}

BOOL FileTransLayer::Send(unsigned char * ppayload, int nlength) {
	return ((CEthernetLayer*)(this->GetUnderLayer()))->Send((unsigned char*)&mHeader, nlength, 0x2090);
}

BOOL FileTransLayer::Receive(unsigned char* frame) {
	PFILE_APP data = (PFILE_APP)frame;
	
	if (data->fapp_type == FILE_INFO) {
		CString file_name;
		file_name.Format(_T("%s"), data->fapp_data);
		WriteFile.Open(file_name, CFile::modeCreate | CFile::modeWrite);
		WriteFile.SetLength(data->fapp_totlen);
	}
	else if (data->fapp_type == FILE_MORE) {
		WriteFile.Seek(data->fapp_seq_num * MAX_APP_DATA, CFile::begin);
		WriteFile.Write(data->fapp_data, data->fapp_totlen);
	}
	else {
		WriteFile.Close();
		AfxMessageBox(_T("ÆÄÀÏ µµÂø!"));
	}

	return true;
}

void FileTransLayer::SetHeader(
	unsigned long len,
	unsigned short type,
	unsigned long seq_num,
	unsigned char* data
) {
	mHeader.fapp_totlen = len;
	mHeader.fapp_type = type;
	mHeader.fapp_seq_num = seq_num;
	memcpy(mHeader.fapp_data, data, len > MAX_APP_DATA ? MAX_APP_DATA : len);
}

void FileTransLayer::SetProgressBar(CProgressCtrl* p) {
	mPro = p;
}

UINT FileTransLayer::FILE_SEND(LPVOID pParam) {
	FileTransLayer* pFL = (FileTransLayer*)pParam;
	CFile SendFile;
	unsigned long totallength;
	unsigned char buffer[MAX_APP_DATA];
	unsigned int seq = -1;
	SendFile.Open(pFL->GetFilePath(), CFile::modeRead); //open file for sending
	totallength = SendFile.GetLength();
	CArchive read_file(&SendFile, CArchive::load);
	pFL->mPro->SetRange(0, totallength / MAX_APP_DATA);
	
	pFL->SetHeader(totallength, FILE_INFO, seq,(unsigned char*)(SendFile.GetFileName().GetBuffer(0)));
	pFL->Send((unsigned char*)&(pFL->mHeader), SIZE_FILE_HEADER + SendFile.GetFileName().GetLength()+1);
	
	while (read_file.Read(buffer, MAX_APP_DATA)) {
		pFL->SetHeader(totallength > MAX_APP_DATA ? MAX_APP_DATA : totallength, FILE_MORE, ++seq, buffer);
		pFL->Send((unsigned char*)&(pFL->mHeader), SIZE_FILE_HEADER + (totallength > MAX_APP_DATA ? MAX_APP_DATA : totallength));
		totallength -= MAX_APP_DATA;
		pFL->mPro->SetPos(seq);
	}
	
	pFL->SetHeader(0, FILE_LAST, ++seq, (unsigned char*)"aaaaaaaa");
	pFL->Send((unsigned char*)&(pFL->mHeader), SIZE_FILE_HEADER + 8);

	SendFile.Close();
	read_file.Close();


	return 0;
}
