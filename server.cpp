#include <winsock2.h>
#include <iostream>
#include "compression.h"
#include "Huffman.h"
#include <fstream>
using namespace std;

struct Client {
    HANDLE Thread;
    DWORD ThreadId;
    SOCKET* socket;
    Client* next;
    Client* last;
};
struct Clients {
    Client* head;
    Client* tail;
    int num;
    Clients() {
        head = tail = new Client;
        head->next = NULL;
        num = 0;
    }
    void add_client(Client * client) {
        tail->next = new Client;
        tail =tail->next;
        tail->next = NULL;
        tail->Thread = client->Thread;
        tail->ThreadId = client->ThreadId;
        tail->socket = client->socket;
        num++;
    }
    void delete_client(Client * client) {
        client->last->next = client->next;
        client->next->last = client->last;
        delete client;
        num--;
    }
};

char** pbits;
Clients clients;
//int picture_size = 6221824;
int lock = 1;

void connect_socket(SOCKET& s) {
    WORD wversionrequested = MAKEWORD(2, 2);
    WSADATA wsadata;
    int werror = 1;
    werror = WSAStartup(wversionrequested, &wsadata);
    if (werror)
    {
        if (WSAGetLastError() == 10004) cout << "操作被终止" << endl;
        else if (WSAGetLastError() == 10013) cout << "访问被拒绝" << endl;
        else if (WSAGetLastError() == 10014) cout << "地址错误" << endl;
        else if (WSAGetLastError() == 10022) cout << "参数错误" << endl;
        else if (WSAGetLastError() == 10024) cout << "打开太多的sockets" << endl;
        else if (WSAGetLastError() == 10036) cout << "一个阻塞正在进行" << endl;
        else if (WSAGetLastError() == 10037) cout << "一个操作正在进行" << endl;
    }
    /*建立socket*/
    s = socket(AF_INET, SOCK_STREAM, 0);

    if (s == INVALID_SOCKET)
    {
        cout << "set up s socket error: ";
        cout << GetLastError() << endl;
    }
    int nsendbuf = 6220804;
    int set_buf=setsockopt(s, SOL_SOCKET, SO_SNDBUF, (const char *)&nsendbuf, sizeof(int));
    if (set_buf != 0) {
        cout << "set buf error" << endl;
    }
    
    sockaddr_in saddress;
    memset(&saddress, 0, sizeof(saddress));
    saddress.sin_family = AF_INET;
    saddress.sin_port = htons(6002);
    saddress.sin_addr.s_addr = INADDR_ANY;
    int binding = bind(s, (SOCKADDR*)&saddress, sizeof(SOCKADDR));
    if (binding == SOCKET_ERROR)
    {
        cout << "bind error: ";
        cout << GetLastError() << endl;
        system("pause");
    }
    cout << "bind success" << endl;
    int listening = listen(s, 10);
    if (listening)
    {
        cout << "listen error" << endl;
    }
    cout << "listen success" << endl;
}
void send_picture() {
    while (1) {
        
        while (lock == 1);
        Client* temp;
        temp = clients.head->next;
        //should add second buf replace pBits?
        while (temp != NULL) {
            for (int i = 0;i <= frames_num - 1;i++) {
                int picture_size;
                if (i == 0) {
                    picture_size = 6220800;
                }
                else {
                    picture_size = (*(int*)pbits[i]) * 68 + 4;
                }
                int k=send(*(temp->socket), pbits[i], picture_size, 0);
                cout << "pbits["<<i<<"] size: "<< *(int*)pbits[i] <<" send :"<< k << endl;
                //Sleep(1);
                //if(k==-1)
                cout << WSAGetLastError() << endl;
            }
            temp = temp->next;
        }
        lock = 1;
    }
}
void get_picture() {
    while (1) {
        while (lock==0){}
        double* dctbuf = new double[64];
        pbits = screenshot();
        for (int fenliang = 0;fenliang < 3;fenliang++) {
            for (int i = 0;i < 1080 / 8;i++) {
                for (int j = 0;j < 1920 / 8;j++) {
                    DCT(pbits[0], dctbuf, 1920, 1080, i * 1920 * 8 * 3 + j * 8 * 3 , fenliang);
                    //IDCT(pbits[0], dctbuf, 1920, 1080, i * 1920 * 8 * 3 + j * 8 * 3 , fenliang);
                    //cout << 1 << endl;;
                }
            }
        }
        lock = 0;
    }
}
int main()
{
   SOCKET s;
    connect_socket(s);
    sockaddr_in* newAddr=new sockaddr_in;
    int len = sizeof(SOCKADDR);
    pbits = screenshot();
    Init_Shift_Table();
    /*char t[100] = "adsklj1093804saknlsnncsxld2eouewfhidslka83r43widjkaswqie23urhfeislaksdhax";
    cout << t << endl;
    char re[100];
    char data[199];
    Huffman_Tree*k= CreateHuffmanTree(t, strlen(t));
    HuffmanTree2DataPacket(data, k, t, strlen(t));
    UnHuffman(re, data);
    
    cout << re;
    */
    double* dctbuf = new double[64];
    double Y[8*8];
    double Cb[8*8];
    double Cr[8*8];
    int position = 0;
    
    cout << "===============original=============" << endl;
    for (int i = 0;i < 8;i++) {
        for (int j = 0;j < 8;j++) {
            cout << (int)(char)pbits[0][position + i * 1920 * 3 + j * 3] << "  ";
        }
        cout << endl;
    }
    cout << "===============original==============" << endl;
    Run_Shift_YCbCr(pbits[0], 1920 * 1080 * 3);
    cout << "===============original shift=============" << endl;
    for (int i = 0;i < 8;i++) {
        for (int j = 0;j < 8;j++) {
            cout << (int)(char)pbits[0][position + i * 1920 * 3 + j * 3 ] << "  ";
        }
        cout << endl;
    }
    cout << "===============original shift==============" << endl;
    DCT(pbits[0], dctbuf, 1920, 1080, 0, 0);
    cout << "===============DCT=================" << endl;
    for (int i = 0;i < 8;i++) {
        for (int j = 0;j < 8;j++) {
            cout << dctbuf[i *8 + j] << "     ";
        }
        cout << endl;
    }
    cout << "===============DCT=================" << endl;
    IDCT(Y, dctbuf, 1920, 1080,0, 0);
    cout << "=============== IDCT=================" << endl;
    for (int i = 0;i < 8;i++) {
        for (int j = 0;j < 8;j++) {
            cout << (int)( char)Y[i * 8 + j] << "     ";
        }
        cout << endl;
    }
    cout << "===============IDCT=================" << endl;
    for (int i = 0;i < 1080/8;i++) {
        for (int j = 0;j < 1920/8;j++) {
            
            DCT(pbits[0], dctbuf, 1920, 1080, i * 1920 * 8 * 3 + j * 8 * 3, 0);
            
            IDCT(Y, dctbuf, 1920, 1080, i * 1920 * 8 * 3 + j * 8 * 3, 0);
            DCT(pbits[0], dctbuf, 1920, 1080, i * 1920 * 8 * 3 + j * 8 * 3, 1);
            IDCT(Cb, dctbuf, 1920, 1080, i * 1920 * 8 * 3 + j * 8 * 3, 1);
            DCT(pbits[0], dctbuf, 1920, 1080, i * 1920 * 8 * 3 + j * 8 * 3, 2);
            IDCT(Cr, dctbuf, 1920, 1080, i * 1920 * 8 * 3 + j * 8 * 3,2);
            
            YCbCr_TO_RGB(pbits[0], Y, Cb, Cr, 1920, 1080, i * 1920 * 8 * 3 + j * 8 * 3, 0);
            YCbCr_TO_RGB(pbits[0], Y, Cb, Cr, 1920, 1080, i * 1920 * 8 * 3 + j * 8 * 3, 1);
            YCbCr_TO_RGB(pbits[0], Y, Cb, Cr, 1920, 1080, i * 1920 * 8 * 3 + j * 8 * 3, 2);
        }
    }
    Run_Inverse_Shift(pbits[0], 1920 * 1080 * 3);
    cout << "===============original unshift=============" << endl;
    for (int i = 0;i < 8;i++) {
        for (int j = 0;j < 8;j++) {
            cout << (int)(char)pbits[0][position + i * 1920 * 3 + j * 3 ] << "  ";
        }
        cout << endl;
    }
    cout << "===============original unshift==============" << endl;
    //set borden
    cout << "===============after transform=============" << endl;
    for (int i = 0;i < 8;i++) {
        pbits[0][position + i * 1920 * 3] = 0;
        pbits[0][position + i * 1920 * 3+7*3] = 0;
        pbits[0][position + i * 1920 * 3+1] = 0;
        pbits[0][position + i * 1920 * 3 + 7 * 3+1] = 0;
        pbits[0][position + i * 1920 * 3+2] = 0;
        pbits[0][position + i * 1920 * 3 + 7 * 3+2] = 0;
        for (int j = 0;j < 8;j++) {
            if (i == 0 || i == 7) {
                pbits[0][position+i * 1920 * 3 + j * 3] = 0;
                pbits[0][position + i * 1920 * 3 + j * 3+1] = 0;
                pbits[0][position + i * 1920 * 3 + j * 3+2] = 0;
            }
            cout << (int)pbits[0][position+i * 1920 * 3 + j * 3] << " ";
        }
        cout << endl;
    }
    cout << "================after transform======" << endl;
    BITMAPINFOHEADER infohead;

    infohead.biSize = sizeof(BITMAPINFOHEADER);
    infohead.biWidth = 1920;
    infohead.biHeight = 1080;
    infohead.biPlanes = 1;
    infohead.biClrUsed = 0;
    infohead.biBitCount = 24;
    infohead.biCompression = BI_RGB;
    infohead.biSizeImage = 1920 * 1080 * 3;//修改过要乘以biBitCount
    infohead.biXPelsPerMeter = 0;
    infohead.biYPelsPerMeter = 0;

    BITMAPFILEHEADER filehead;

    filehead.bfType = 0x4D42;  // "BM"
    filehead.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + infohead.biSizeImage;//有歧义
    filehead.bfReserved1 = 0;
    filehead.bfReserved2 = 0;
    filehead.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    ofstream o;

    o.open("dct.bmp", ios::binary | ios::out);
    o.write((char*)&filehead, sizeof(BITMAPFILEHEADER));
    o.write((char*)&infohead, sizeof(BITMAPINFOHEADER));
    o.write((char*)pbits[0], 1920 * 1080 * 3);
    o.close();
    
    /* DWORD get_picture_thread_id;
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)get_picture, 0, 0, &get_picture_thread_id);
    DWORD send_picture_thread_id;
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)send_picture, 0, 0, &send_picture_thread_id);
    while (1) {
        SOCKET* newsocket = new SOCKET;
        *newsocket = accept(s, (SOCKADDR*)newAddr, &len);

        cout << "get client" << endl;
        if (*newsocket == INVALID_SOCKET) {
            cout << "newsocket error" << endl;
        }
        Client* client=new Client;
        client->socket = newsocket;
        //client->Thread=CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)send_picture,newsocket, 0, &client->ThreadId);
        clients.add_client(client);
        
    }
    while (1) {
        
    }*/
    closesocket(s);
    WSACleanup();
    return 0;
}
