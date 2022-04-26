#include <iostream>
#include <windows.h>
#include <fstream>
#include "Huffman.h"
#include "decompression.h"
#define basic_block 8
using namespace std;
char bitmap[6221824];
int lock = 0;
char** pBits;
int frames = 5;
int count_frame = 0;
struct Decompress_Procedure_Params {
    char** pbits_pos;
    int process_num;
    SOCKET* s;
};
Decompress_Procedures_Signal decompress_process;
Decompress_Procedures_Signal show_process;
void Decompress_Procedure(LPVOID* k);
void Play_Procedure(LPVOID* t);
void recv_picture(SOCKET& s,char ** pbits_pos, char* recvbuf, char* outbuf);
void setup_sockets();
void connect_socket(SOCKET &s,int port,const char * ip_addr);
bool Init_Semphores() {
    const char* decom_names = "12340";
    const char* read_names = "56780";
    for (int i = 0;i < decompress_procedure_num;i++) {
        decompress_process.signals[i] = 0;
        decompress_process.names[i] = decom_names + i;
        show_process.signals[i] = 0;
        show_process.names[i] = read_names + i;
    }
    for (int i = 0;i < decompress_procedure_num;i++) {
        decompress_process.signals[i] = CreateSemaphore(NULL, 1, 1, decompress_process.names[i]);
        show_process.signals[i] = CreateSemaphore(NULL, 0, 1, show_process.names[i]);
    }

    for (int i = 0;i < decompress_procedure_num;i++) {
        if (decompress_process.signals[i] == 0 || show_process.signals[i] == 0) {
            return false;
        }
    }
    return true;
}
void load_photo_to_file(char * buf,const char * picture_name) {
    int width = 1920;
    int height = 1080;
    BITMAPINFOHEADER infohead;
    infohead.biSize = sizeof(BITMAPINFOHEADER);
    infohead.biWidth = width;
    infohead.biHeight = height;
    infohead.biPlanes = 1;
    infohead.biClrUsed = 0;
    infohead.biBitCount = 24;
    infohead.biCompression = BI_RGB;
    infohead.biSizeImage = width * height * 3;//修改过要乘以biBitCount
    infohead.biXPelsPerMeter = 0;
    infohead.biYPelsPerMeter = 0;

    BITMAPFILEHEADER filehead;

    filehead.bfType = 0x4D42;  // "BM"
    filehead.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + infohead.biSizeImage;//有歧义
    filehead.bfReserved1 = 0;
    filehead.bfReserved2 = 0;
    filehead.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    BITMAPINFO amp;
    amp.bmiHeader = infohead;
    ofstream o;
    o.open(picture_name, ios::binary | ios::out);
    o.write((char*)&filehead, sizeof(BITMAPFILEHEADER));
    o.write((char*)&infohead, sizeof(BITMAPINFOHEADER));
    o.write((char*)buf, width * height * 3);
    o.close();
}
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wparam, LPARAM lparam)
{
    switch (uMsg)
    {
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_LBUTTONDOWN:
    {
        break;
    }
    case WM_KEYDOWN:
        //MessageBox(hwnd, TEXT("键盘按下"), TEXT("键盘消息"), MB_OK);
        break;
    case WM_PAINT:
    {
        
        InvalidateRect(hwnd, 0, false);
        HDC hddc;
        HDC mddc;
        PAINTSTRUCT pps;
        hddc = BeginPaint(hwnd, &pps);
        mddc = CreateCompatibleDC(hddc);
        //test bo fang qi
        /*HDC hhddc;
        HDC mmddc;
        hhddc = CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL);

        int width = GetDeviceCaps(hhddc, DESKTOPHORZRES);
        int height = GetDeviceCaps(hhddc, DESKTOPVERTRES);


        mmddc = CreateCompatibleDC(hhddc);
        HBITMAP hbmp = CreateCompatibleBitmap(hhddc, width, height);
        SelectObject(mmddc, hbmp);
        char* pBits = bitmap;
        */
        //
        int width = 1920;
        int height = 1080;
        BITMAPINFOHEADER infohead;
        infohead.biSize = sizeof(BITMAPINFOHEADER);
        infohead.biWidth = width;
        infohead.biHeight = height;
        infohead.biPlanes = 1;
        infohead.biClrUsed = 0;
        infohead.biBitCount = 24;
        infohead.biCompression = BI_RGB;
        infohead.biSizeImage = width * height * 3;//修改过要乘以biBitCount
        infohead.biXPelsPerMeter = 0;
        infohead.biYPelsPerMeter = 0;

        BITMAPFILEHEADER filehead;

        filehead.bfType = 0x4D42;  // "BM"
        filehead.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + infohead.biSizeImage;//有歧义
        filehead.bfReserved1 = 0;
        filehead.bfReserved2 = 0;
        filehead.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

        //test bo fang qi
        /*
        BitBlt(mmddc, 0, 0, width, height, hhddc, 0, 0, SRCCOPY);
        BITMAPINFO amp;
        amp.bmiHeader = infohead;
        GetDIBits(hhddc, hbmp, 0, height, pBits, (LPBITMAPINFO) & amp, DIB_RGB_COLORS);
        if (hbmp == NULL)
            cout << GetLastError() << endl;
        */
        /*ofstream o;

        o.open("k.bmp",ios::binary|ios::out);
        o.write((char *)&filehead,sizeof(BITMAPFILEHEADER));
        o.write((char *)&infohead,sizeof(BITMAPINFOHEADER ));
        o.write((char *)pBits,width*height*3);
        o.close();
        */
        //hbmp=(HBITMAP)LoadImage(0,"t.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
        BITMAPINFO amp;
        amp.bmiHeader = infohead;
        SetDIBitsToDevice(hddc, 0, 0, width, height, 0, 0, 0, height, pBits[count_frame], &amp, DIB_RGB_COLORS);
        count_frame = (count_frame + 1) % 60;
        cout << "paint" << endl;
        load_photo_to_file(pBits[count_frame], "test1.bmp");
        EndPaint(hwnd, &pps);
        //DeleteObject(hbmp);
        DeleteDC(mddc);

        break;
    }

    default:
        break;
    }

    return DefWindowProc(hwnd, uMsg, wparam, lparam);
}
LRESULT CALLBACK InterfaceProc(HWND hwnd, UINT uMsg, WPARAM wparam, LPARAM lparam) {
    switch (uMsg) {
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        break;
    }
    return DefWindowProc(hwnd, uMsg, wparam, lparam);
}
void bofangqi(WNDCLASS& classes, HINSTANCE& hInstance) {
    classes.cbClsExtra = 0;
    classes.cbWndExtra = 0;
    classes.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    classes.hCursor = LoadCursor(NULL, IDC_HAND);
    classes.hIcon = LoadIcon(NULL, IDI_WARNING);
    classes.hInstance = hInstance;
    classes.lpfnWndProc = WindowProc;
    classes.lpszClassName = TEXT("bofangqi");
    classes.lpszMenuName = NULL;
    classes.style = 0;
}//
void InterFace(WNDCLASS& classes, HINSTANCE& hInstance) {
    classes.cbClsExtra = 0;
    classes.cbWndExtra = 0;
    classes.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    classes.hCursor = LoadCursor(NULL, IDC_HAND);
    classes.hIcon = LoadIcon(NULL, IDI_WARNING);
    classes.hInstance = hInstance;
    classes.lpfnWndProc = InterfaceProc;
    classes.lpszClassName = TEXT("interface");
    classes.lpszMenuName = NULL;
    classes.style = 0;
}
void Dialog(WNDCLASS& classes, HINSTANCE& hInstance) {
    classes.cbClsExtra = 0;
    classes.cbWndExtra = 0;
    classes.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    classes.hCursor = LoadCursor(NULL, IDC_HAND);
    classes.hIcon = LoadIcon(NULL, IDI_WARNING);
    classes.hInstance = hInstance;
    classes.lpfnWndProc = InterfaceProc;
    classes.lpszClassName = TEXT("dialog");
    classes.lpszMenuName = NULL;
    classes.style = 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    Init_Shift_Table();
    Init_Semphores();
    setup_sockets();
    pBits = new char* [frames* queue_size];
    for (int i = 0;i <= queue_size*frames - 1;i++) {
        pBits[i] = new char[1920*1080*3];
        memset(pBits[i], 0, 1920 * 1080 * 3);
    }
    WNDCLASS main_interface;
    InterFace(main_interface, hInstance);
    WNDCLASS wc;
    bofangqi(wc, hInstance);
    WNDCLASS dialog;
    Dialog(dialog, hInstance);
    RegisterClass(&wc);
    RegisterClass(&dialog);
    RegisterClass(&main_interface);
    HWND main_window = CreateWindow(main_interface.lpszClassName, TEXT("main_window"), WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, 1200, 700,
        NULL, NULL, hInstance, NULL);
    ShowWindow(main_window, SW_SHOWNORMAL);

    HWND hwnd = CreateWindow(wc.lpszClassName, TEXT("TEXT WINDOW"), WS_CHILD | WS_VISIBLE,
        10, 10, 900, 600,
        main_window, NULL, hInstance, NULL);

    ShowWindow(hwnd, SW_SHOWNORMAL);

    UpdateWindow(hwnd);

    MSG msg;
    /*HWND dialog_box=CreateWindow(dialog.lpszClassName,TEXT("dialog box"),WS_CHILD|WS_OVERLAPPEDWINDOW,
                                    910, 10, 100, 500 ,
                                    main_window, NULL, hInstance, NULL);
        ShowWindow(dialog_box, SW_SHOWNORMAL);*/
    SOCKET s;
    connect_socket(s,6002,"192.168.56.1");
    //connect the rest duplicates
    SOCKET sk[3];
    connect_socket(sk[0], 6003, "192.168.56.1");
    
    connect_socket(sk[1], 6004, "192.168.56.1");
    
    connect_socket(sk[2], 6005, "192.168.56.1");
    Decompress_Procedure_Params* dcp[queue_size / 3];
    
    
    for (int i = 0;i < queue_size / 3;i++) {
        dcp[i] = new Decompress_Procedure_Params;
        dcp[i]->pbits_pos = pBits + i * 5 * 3;
        dcp[i]->process_num = i;
    }
    dcp[0]->s = &s;
    dcp[1]->s = &sk[0];
    dcp[2]->s = &sk[1];
    dcp[3]->s = &sk[2];
    //
    DWORD show_picture_thread_id;
    CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)Play_Procedure, (HWND*)&hwnd,0, &show_picture_thread_id);
    DWORD recv_picture_thread_id1;
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Decompress_Procedure, (LPVOID)dcp[0], 0, &recv_picture_thread_id1);
    DWORD recv_picture_thread_id2;
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Decompress_Procedure, (LPVOID)dcp[1], 0, &recv_picture_thread_id2);
    DWORD recv_picture_thread_id3;
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Decompress_Procedure, (LPVOID)dcp[2], 0, &recv_picture_thread_id3);
    DWORD recv_picture_thread_id4;
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Decompress_Procedure, (LPVOID)dcp[3], 0, &recv_picture_thread_id4);
    
    //}
        /*const char* names[5] = {
            "aa.bmp","b.bmp","c.bmp","d.bmp","e.bmp"
        };
        for (int i = 0;i < 5;i++) {
            load_photo_to_file(pBits[i], names[i]);
        }*/
    while (1)
    {
        //PostMessage(hwnd, WM_PAINT, 0, 0);
        Sleep(10);
        GetMessage(&msg, NULL, 0, 0);
        TranslateMessage(&msg);   //将传来的消息翻译
        DispatchMessage(&msg);
    }
}
void setup_sockets() {
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
void connect_socket(SOCKET & s,int port,const char * ip_addr) {
    

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == INVALID_SOCKET)
    {
        cout << "socket set up:  " << WSAGetLastError() << endl;
    }
    int nsendbuf = 6220804;
    int set_buf = setsockopt(s, SOL_SOCKET, SO_SNDBUF, (const char*)&nsendbuf, sizeof(int));
    if (set_buf != 0) {
        cout << "set buf error" << endl;
    }
    sockaddr_in saddress;
    memset(&saddress, 0, sizeof(saddress));
    saddress.sin_family = AF_INET;
    saddress.sin_port = htons(port);//46130
    saddress.sin_addr.s_addr = inet_addr(ip_addr);//两次转换可能出错103.46.128.49
    int connect_error;
    connect_error = connect(s, (sockaddr*)&saddress, sizeof(saddress));//第二个参数可能出错，3个参数可能错
    if (connect_error == -1)
    {
        int e = WSAGetLastError();
        std::cout << "connect error" << std::endl;
        std::cout << "connect:  " << e << std::endl;
        system("pause");
    }
    std::cout << "connect success" << std::endl;

}
void Decompress_Procedure(LPVOID * k) {
    Decompress_Procedure_Params* t=(Decompress_Procedure_Params*)k;
    char* recvbuf = new char[1920 * 1080 * 3 * 2];
    char* outbuf = new char[1920 * 1080 * 3 * 2];
    while (true) {
        WaitForSingleObject(decompress_process.signals[t->process_num],INFINITE);
        recv_picture(*(t->s), t->pbits_pos, recvbuf, outbuf);
        
        recv_picture(*(t->s), t->pbits_pos + 5, recvbuf, outbuf);
        
        recv_picture(*(t->s), t->pbits_pos + 10, recvbuf, outbuf);
        
        ReleaseSemaphore(show_process.signals[t->process_num],1,NULL);
    }
}
void Play_Procedure(LPVOID * t) {
    HWND* hwnd = (HWND*)t;
    while (true) {
        
        for (int i = 0;i < frames_num*queue_size;i++) {
            if (i % 15 == 0) {
                WaitForSingleObject(show_process.signals[i/15],INFINITE);
            }
            SendMessage(*hwnd, WM_PAINT, 0, 0);
            
            Sleep(10);
            if ((i + 1) % 15 == 0) {
                ReleaseSemaphore(decompress_process.signals[i/15],1,NULL);
            }
        }
        
    }
}
void recv_picture(SOCKET & s,char ** pbits_pos, char* recvbuf, char* outbuf) {
    int len;
    int recive = 0;//=0?
    recv(s, (char*)&len, 4, 0);
    len++;
    while (len > recive) {
        recive += recv(s, recvbuf + recive, len - recive, 0);
    }
    //memset(outbuf, 0, 1920 * 1080 * 3 * 2);
    UnHuffman(outbuf, recvbuf);

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

            YCbCr_TO_RGB(pbits_pos[0], Y, Cb, Cr, 1920, 1080, i * 1920 * 8 * 3 + j * 8 * 3, 0);
            YCbCr_TO_RGB(pbits_pos[0], Y, Cb, Cr, 1920, 1080, i * 1920 * 8 * 3 + j * 8 * 3, 1);
            YCbCr_TO_RGB(pbits_pos[0], Y, Cb, Cr, 1920, 1080, i * 1920 * 8 * 3 + j * 8 * 3, 2);
        }
    }
    Run_Inverse_Shift((unsigned char*)pbits_pos[0], 1920 * 1080 * 3);
    //SendMessage(*hwnd, WM_PAINT, 0, 0);//PostMessage(*hwnd, WM_PAINT, 0, 0);
    for (int i = 1;i < 5;i++) {
        recive = 0;
        recv(s, (char*)&len, 4, 0);
        while (len > recive) {
            recive += recv(s, pbits_pos[i] + recive, len - recive, 0);
        }
        //SendMessage(*hwnd, WM_PAINT, 0, 0);//PostMessage(*hwnd, WM_PAINT, 0, 0);
    }
    decompress_interframe(pbits_pos, 1920, 1080);
    
}
