#pragma once
#include "BaseLayer.h"
#include "pch.h"
#include <iostream>
#include <string>

#define MAX_APP_DATA 1488
#define SIZE_FILE_HEADER 12

class CFileTransLayer :
    public CBaseLayer
{
private:
    unsigned short totalLength;
    struct FrameSeq {
        CString data;
        unsigned char seq;
        FrameSeq* next;
    };
    FrameSeq* Head;
    void	add_after(FrameSeq* prev, unsigned char* data, unsigned char seq);
    bool	seq_compare(FrameSeq* p, unsigned char seq);
    void	add(unsigned char* data, unsigned char seq);
    void	make_frame(unsigned char* ppayoad, unsigned long nlength, unsigned short type, int seq, unsigned char msg);
    void	deleteList();

public:
    CFileTransLayer(char* pName);
    virtual ~CFileTransLayer();
    BOOL Receive(unsigned char* frame); 
    BOOL Send(unsigned char* ppayload, int nlength, CString strPath);

    typedef struct _FILE_APP {
        unsigned long   fapp_totlen; //총 길이
        unsigned short  fapp_type;  //데이터 타입
        unsigned char   fapp_msg_type; // 메시지 종류
        unsigned char   unused; // 사용안함
        unsigned long   fapp_seq_num;   //fragmentation 
        unsigned char   fapp_data[MAX_APP_DATA];
    }FILE_APP, *LPFILE_APP;
protected:
    FILE_APP    m_sFileApp;
};

