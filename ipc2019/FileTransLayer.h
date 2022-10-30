#pragma once
#include "BaseLayer.h"
#include "pch.h"

#define MAX_APP_DATA 1488
#define SIZE_FILE_HEADER 12
#define DONT_FRAG 0x0000
#define FILE_INFO 0x0100
#define FILE_MORE 0x0200
#define FILE_LAST 0x0300


class FileTransLayer :
    public CBaseLayer
{
    CString FilePath;
public:
    FileTransLayer(char *name);
    ~FileTransLayer();
    BOOL Receive(unsigned char* frame);
    BOOL Send(unsigned char* ppayload, int nlength);
    void SetFilePath(CString Path) {
        FilePath = Path;
    }
    CString GetFilePath() {
        return FilePath;
    }
    void SetHeader(
        unsigned long len,
        unsigned short type,
        unsigned long seq_num,
        unsigned char* data
    );

    typedef struct _FILE_APP {
        unsigned long   fapp_totlen;
        unsigned short  fapp_type;
        unsigned char   faa_msg_type;
        unsigned char   unused;
        unsigned long   fapp_seq_num;
        unsigned char   fapp_data[MAX_APP_DATA];
    }FILE_APP, *PFILE_APP;

    FILE_APP mHeader;
    static UINT FILE_SEND(LPVOID pParam);
};

