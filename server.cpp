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

int get_picture(char* outbuf, char* data) {

    int dctbuf[64];
    pbits = screenshot();
    Run_Shift_YCbCr((unsigned char*)pbits[0], 1920 * 1080 * 3);
    int pos = 0;
    for (int i = 0;i < 1080 / 8;i++) {
        for (int j = 0;j < 1920 / 8;j++) {
            DCT((unsigned char*)pbits[0], dctbuf, 1920, 1080, i * 8 * 1920 * 3 + j * 8 * 3, 0);
            pos = ZigZag(dctbuf, outbuf, pos);
            DCT((unsigned char*)pbits[0], dctbuf, 1920, 1080, i * 8 * 1920 * 3 + j * 8 * 3, 1);
            pos = ZigZag(dctbuf, outbuf, pos);
            DCT((unsigned char*)pbits[0], dctbuf, 1920, 1080, i * 8 * 1920 * 3 + j * 8 * 3, 2);
            pos = ZigZag(dctbuf, outbuf, pos);
            //IDCT(pbits[0], dctbuf, 1920, 1080, i * 1920 * 8 * 3 + j * 8 * 3 , fenliang
        }
    }
    Huffman_Tree* t = CreateHuffmanTree(outbuf, pos);
    int size = HuffmanTree2DataPacket(data, t, outbuf, pos);
    //interframe_compression(pbits, 5, 1920, 1080);
    cout << "first picture size = " << size << endl;
    double Y[8 * 8];
    double Cb[8 * 8];
    double Cr[8 * 8];
    //memset(outbuf, 0, 1920 * 1080 * 3 * 2);
    //int after_huffman=UnHuffman(outbuf, data);
    //cout <<"Huffman later "<< after_huffman << endl;
    return size;
}
void send_picture() {
    char* outbuf = new char[1920 * 1080 * 3 * 2];
    char* datapacket = new char[1920 * 1080 * 3 * 2];
    while (true) {
        int size = get_picture(outbuf, datapacket);
        Client* p;
        p = clients.head->next;
        while (p != NULL) {
            SOCKET sock = *(p->socket);
            send(sock, (char*)&size, sizeof(int), 0);
            send(sock, datapacket, size, 0);
            for (int i = 1;i < 5;i++) {
                int len = *(int*)(*(pbits + i));
                send(sock, *(pbits + i), sizeof(int), 0);
                send(sock, *(pbits + i), len, 0);
            }
            p = p->next;
        }
    }
}
int main()
{
   SOCKET s;
    connect_socket(s);
    sockaddr_in* newAddr=new sockaddr_in;
    int len = sizeof(SOCKADDR);
    //pbits = screenshot();
    Init_Shift_Table();
    /*double Y[8 * 8];
    double Cb[8 * 8];
    double Cr[8 * 8];
    int position = 0;
    char* outbuf = new char[1920 * 1080 * 3 * 3];
    char* data = new char[1920 * 1080 * 3 * 2];
    get_picture(outbuf,data);*/
    
    /*int buf[64];
    memset(buf, 0, 64);
    
    char* outbuf = new char[80];
    ZigZag(buf, outbuf, 0);
    UnZigZag(outbuf, buf, 0);*/
    /*char t[100] = "adsklj1093804saknlsnncsxld2eouewfhidslka83r43widjkaswqie23urhfeislaksdhax";
    cout << t << endl;
    char re[100];
    char data[199];
    Huffman_Tree*k= CreateHuffmanTree(t, strlen(t));
    HuffmanTree2DataPacket(data, k, t, strlen(t));
    UnHuffman(re, data);
    
    cout << re;
    */
    
    
   /*cout << "===============original=============" << endl;
    for (int i = 0;i < 8;i++) {
        for (int j = 0;j < 8;j++) {
            cout << (int)(char)pbits[0][position + i * 1920 * 3 + j * 3] << "  ";
        }
        cout << endl;
    }
    cout << "===============original==============" << endl;
    Run_Shift_YCbCr((unsigned char*)pbits[0], 1920 * 1080 * 3);
    cout << "===============original shift=============" << endl;
    for (int i = 0;i < 8;i++) {
        for (int j = 0;j < 8;j++) {
            cout << (int)(unsigned char)pbits[0][position + i * 1920 * 3 + j * 3 ] << "  ";
        }
        cout << endl;
    }
    int dctbuf[64];
    cout << "===============original shift==============" << endl;
    DCT((unsigned char*)pbits[0], dctbuf, 1920, 1080, 0, 0);
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
            
            DCT((unsigned char*)pbits[0], dctbuf, 1920, 1080, i * 1920 * 8 * 3 + j * 8 * 3, 0);
            
            IDCT(Y, dctbuf, 1920, 1080, i * 1920 * 8 * 3 + j * 8 * 3, 0);
            DCT((unsigned char*)pbits[0], dctbuf, 1920, 1080, i * 1920 * 8 * 3 + j * 8 * 3, 1);
            IDCT(Cb, dctbuf, 1920, 1080, i * 1920 * 8 * 3 + j * 8 * 3, 1);
            DCT((unsigned char*)pbits[0], dctbuf, 1920, 1080, i * 1920 * 8 * 3 + j * 8 * 3, 2);
            IDCT(Cr, dctbuf, 1920, 1080, i * 1920 * 8 * 3 + j * 8 * 3,2);
            
            YCbCr_TO_RGB(pbits[0], Y, Cb, Cr, 1920, 1080, i * 1920 * 8 * 3 + j * 8 * 3, 0);
            YCbCr_TO_RGB(pbits[0], Y, Cb, Cr, 1920, 1080, i * 1920 * 8 * 3 + j * 8 * 3, 1);
            YCbCr_TO_RGB(pbits[0], Y, Cb, Cr, 1920, 1080, i * 1920 * 8 * 3 + j * 8 * 3, 2);
        }
    }
    Run_Inverse_Shift((unsigned char*)pbits[0], 1920 * 1080 * 3);
    cout << "===============original unshift=============" << endl;
    for (int i = 0;i < 8;i++) {
        for (int j = 0;j < 8;j++) {
            cout << (int)(unsigned char)pbits[0][position + i * 1920 * 3 + j * 3 ] << "  ";
        }
        cout << endl;
    }
    cout << "===============original unshift==============" << endl;
    */
    //set borden
    /*cout << "===============after transform=============" << endl;
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

    o.open("dct2.bmp", ios::binary | ios::out);
    o.write((char*)&filehead, sizeof(BITMAPFILEHEADER));
    o.write((char*)&infohead, sizeof(BITMAPINFOHEADER));
    o.write((char*)pbits[0], 1920 * 1080 * 3);
    o.close();
    */
    DWORD get_picture_thread_id;
    //CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)get_picture, 0, 0, &get_picture_thread_id);
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
        
    }
    closesocket(s);
    WSACleanup();
    return 0;
}
