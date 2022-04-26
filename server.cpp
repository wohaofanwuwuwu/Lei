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
    SOCKET* duplicate[3];
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
        tail->next =client;//
        tail =tail->next;
        tail->next = NULL;
        //tail->Thread = client->Thread;
        //tail->ThreadId = client->ThreadId;
        //tail->socket = client->socket;
        //copy duplicates

        num++;
    }
    void delete_client(Client * client) {
        client->last->next = client->next;
        client->next->last = client->last;
        delete client;
        num--;
    }
};
struct Compress_Procedure {
    char** pbits_pos;
    int process_num;
};
char** pbits;
Clients clients;
int lock = 1;
Compress_Procedures_Signal compress_process;
Compress_Procedures_Signal read_process;
bool Init_Semphores(){
    const char* com_names ="1234";
    const char* read_names = "5678";
    for (int i = 0;i < compress_procedure_num;i++) {
        compress_process.signals[i] = 0;
        compress_process.names[i] = com_names + i;
        read_process.signals[i] = 0;
        read_process.names[i] = read_names + i;
    }
    for (int i = 0;i < compress_procedure_num;i++) {
        compress_process.signals[i] = CreateSemaphore(NULL, 0, 1, compress_process.names[i]);
        read_process.signals[i]= CreateSemaphore(NULL, 1, 1, read_process.names[i]);
    }

    for (int i = 0;i < compress_procedure_num;i++) {
        if (compress_process.signals[i] == 0 || read_process.signals[i] == 0) {
            return false;
        }
    }
    return true;
}
void setup_socket() {
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
}
void connect_socket(SOCKET& s,int port) {
    
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
    saddress.sin_port = htons(port);
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

int get_picture(char* outbuf, char* data,char** pbits_pos) {

    int dctbuf[64];
    //screenshot(pbits);
    Run_Shift_YCbCr((unsigned char*)pbits_pos[0], 1920 * 1080 * 3);
    int pos = 0;
    for (int i = 0;i < 1080 / 8;i++) {
        for (int j = 0;j < 1920 / 8;j++) {
            DCT((unsigned char*)pbits_pos[0], dctbuf, 1920, 1080, i * 8 * 1920 * 3 + j * 8 * 3, 0);
            pos = ZigZag(dctbuf, outbuf, pos);
            DCT((unsigned char*)pbits_pos[0], dctbuf, 1920, 1080, i * 8 * 1920 * 3 + j * 8 * 3, 1);
            pos = ZigZag(dctbuf, outbuf, pos);
            DCT((unsigned char*)pbits_pos[0], dctbuf, 1920, 1080, i * 8 * 1920 * 3 + j * 8 * 3, 2);
            pos = ZigZag(dctbuf, outbuf, pos);
            //IDCT(pbits[0], dctbuf, 1920, 1080, i * 1920 * 8 * 3 + j * 8 * 3 , fenliang
        }
    }

    //cout << "DCT and zigzag size: " << pos << endl;
    Huffman_Tree* t = CreateHuffmanTree(outbuf, pos);
    int size = HuffmanTree2DataPacket(data, t, outbuf, pos);
    //interframe_compression(pbits, 5, 1920, 1080);

    //cout << "first picture size = " << size << endl;
   /* memset(outbuf, 0, 1920 * 1080 * 3);
    UnHuffman(outbuf, data);

    int k = 0;
    int temp[64];
    double Y[8 * 8];
    double Cb[8 * 8];
    double Cr[8 * 8];
    for (int i = 0;i < 1080 / 8;i++) {
        for (int j = 0;j < 1920 / 8;j++) {
            k = UnZigZag(outbuf, temp, k);

            IDCT(Y, temp, 1920, 1080, 0, 0);

            k = UnZigZag(outbuf, temp, k);

            IDCT(Cb, temp, 1920, 1080, 0, 1);

            k = UnZigZag(outbuf, temp, k);

            IDCT(Cr, temp, 1920, 1080, 0, 2);

            YCbCr_TO_RGB(pbits[0], Y, Cb, Cr, 1920, 1080, i * 1920 * 8 * 3 + j * 8 * 3, 0);
            YCbCr_TO_RGB(pbits[0], Y, Cb, Cr, 1920, 1080, i * 1920 * 8 * 3 + j * 8 * 3, 1);
            YCbCr_TO_RGB(pbits[0], Y, Cb, Cr, 1920, 1080, i * 1920 * 8 * 3 + j * 8 * 3, 2);
        }
    }
    Run_Inverse_Shift((unsigned char*)pbits[0], 1920 * 1080 * 3);
    decompress_interframe(pbits, 1920, 1080);*/
    //memset(outbuf, 0, 1920 * 1080 * 3 * 2);
    //int after_huffman=UnHuffman(outbuf, data);
    //cout <<"Huffman later "<< after_huffman << endl;
    return size;
}
void send_picture(void *pthread_struct) {
    Compress_Procedure* t = (Compress_Procedure*)pthread_struct;
    char* outbuf = new char[1920 * 1080 * 3 * 2];
    char* datapacket = new char[1920 * 1080 * 3 * 2];
    while (true) {
        WaitForSingleObject(compress_process.signals[t->process_num], INFINITE);
        //cout << "procedure " << t->process_num << " was waked up " << endl;
        //@TODO fix the parameters
        Client* p;
        int size = get_picture(outbuf, datapacket, t->pbits_pos);
        p = clients.head->next;
        while (p != NULL) {
            SOCKET sock;
            if (t->process_num == 0) {
                sock = *(p->socket);
            }
            else {
                sock = *(p->duplicate[t->process_num - 1]);
            }

            send(sock, (char*)&size, sizeof(int), 0);
            send(sock, datapacket, size + 1, 0);
            for (int i = 1;i < 5;i++) {
                int len = (*(int*)(*(t->pbits_pos + i)));
                len = len * 68 + 4;
                //cout << "interframes "<<i<<" size " << len << endl;
                send(sock, (char*)&len, sizeof(int), 0);
                send(sock, *(t->pbits_pos + i), len, 0);
            }
            p = p->next;
        }
        int size2 = get_picture(outbuf, datapacket, t->pbits_pos+5);
        p = clients.head->next;
        while (p != NULL) {
            SOCKET sock;
            if (t->process_num == 0) {
                sock = *(p->socket);
            }
            else {
                sock = *(p->duplicate[t->process_num - 1]);
            }

            send(sock, (char*)&size2, sizeof(int), 0);
            send(sock, datapacket, size2 + 1, 0);
            for (int i = 1;i < 5;i++) {
                int len = (*(int*)(*(t->pbits_pos + 5 + i)));
                len = len * 68 + 4;
                //cout << "interframes "<<i<<" size " << len << endl;
                send(sock, (char*)&len, sizeof(int), 0);
                send(sock, *(t->pbits_pos + 5 + i), len, 0);
            }
            p = p->next;
        }
        int size3 = get_picture(outbuf, datapacket, t->pbits_pos+10);
        p = clients.head->next;
        while (p != NULL) {
            SOCKET sock;
            if (t->process_num == 0) {
                sock = *(p->socket);
            }
            else {
                sock = *(p->duplicate[t->process_num - 1]);
            }

            send(sock, (char*)&size3, sizeof(int), 0);
            send(sock, datapacket, size3 + 1, 0);
            for (int i = 1;i < 5;i++) {
                int len = (*(int*)(*(t->pbits_pos + 10 + i)));
                len = len * 68 + 4;
                //cout << "interframes "<<i<<" size " << len << endl;
                send(sock, (char*)&len, sizeof(int), 0);
                send(sock, *(t->pbits_pos + 10 + i), len, 0);
            }
            p = p->next;
        }
        ReleaseSemaphore(read_process.signals[t->process_num], 1, NULL);
    }
}
int main()
{
    setup_socket();
    SOCKET s;
    connect_socket(s,6002);
    SOCKET thread2_sock;
    connect_socket(thread2_sock, 6003);
    SOCKET thread3_sock;
    connect_socket(thread3_sock, 6004);
    SOCKET thread4_sock;
    connect_socket(thread4_sock, 6005);
    sockaddr_in* newAddr=new sockaddr_in;
    int len = sizeof(SOCKADDR);
    //pbits = screenshot();
    Init_Semphores();
    Init_Shift_Table();
    pbits = new char* [queue_size*frames_num];
    for (int i = 0;i <= queue_size*frames_num - 1;i++) {
        pbits[i] = new char[1920 * 1080 * 3];
    }
    Compress_Procedure *cp[queue_size / 3];
    for (int i = 0;i < queue_size / 3;i++) {
        cp[i] = new Compress_Procedure;
        cp[i]->pbits_pos = pbits + i * 5 * 3;
        cp[i]->process_num = i;
    }
    DWORD shot;
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)screenshot, (LPVOID)pbits, 0, &shot);
    //DWORD get_picture_thread_id;
    //CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)get_picture, 0, 0, &get_picture_thread_id);
    DWORD send_picture_thread_id_1;
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)send_picture, cp[0], 0, &send_picture_thread_id_1);
    DWORD send_picture_thread_id_2;
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)send_picture, cp[1], 0, &send_picture_thread_id_2);
    DWORD send_picture_thread_id_3;
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)send_picture, cp[2], 0, &send_picture_thread_id_3);
    DWORD send_picture_thread_id_4;
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)send_picture, cp[3], 0, &send_picture_thread_id_4);

    while (true) {
        SOCKET* newsocket = new SOCKET;
        *newsocket = accept(s, (SOCKADDR*)newAddr, &len);
        if (*newsocket == INVALID_SOCKET) {
            cout << "newsocket error" << endl;
        }
        //create duplicates
        SOCKET* newsocket2 = new SOCKET;
        *newsocket2 = accept(thread2_sock, (SOCKADDR*)newAddr, &len);
        if (*newsocket2 == INVALID_SOCKET) {
            cout << "newsocket error" << endl;
        }

        SOCKET* newsocket3 = new SOCKET;
        *newsocket3 = accept(thread3_sock, (SOCKADDR*)newAddr, &len);
        if (*newsocket3 == INVALID_SOCKET) {
            cout << "newsocket error" << endl;
        }

        SOCKET* newsocket4 = new SOCKET;
        *newsocket4 = accept(thread4_sock, (SOCKADDR*)newAddr, &len);
        if (*newsocket4 == INVALID_SOCKET) {
            cout << "newsocket error" << endl;
        }
        //
        Client* client=new Client;
        client->socket = newsocket;
        client->duplicate[0] = newsocket2;
        client->duplicate[1] = newsocket3;
        client->duplicate[2] = newsocket4;
        cout << "get client" << endl;
        //client->Thread=CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)send_picture,newsocket, 0, &client->ThreadId);
        clients.add_client(client);
        
    }

    closesocket(s);
    WSACleanup();
    return 0;
}
