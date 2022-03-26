#include <iostream>
#include <windows.h>
#include <fstream>
#include "Huffman.h"
//#include "decompression.h"
#define basic_block 8
using namespace std;
char bitmap[6221824];
char** pBits;
int frames = 5;
int count_frame = 0;
SOCKET s;
void connect_socket();
void decompress_interframe(char ** frames,int frames_num,int length,int width) {
	for (int i = 1;i <= frames_num - 1;i++) {
		int num = *(int*)frames[i];
		int sum = length * width * 3;
		char* temp_frame = new char[sum];
		for (int j = 0;j <= sum - 1;j++) {
			temp_frame[j] = frames[i - 1][j];
		}

		int pointer = 4;
		for (int j = 0;j <= num - 1;j++) {
			short row = *(short*)&frames[i][pointer];
			pointer += 2;
			short colmun = *(short*)&frames[i][pointer];
			pointer += 2;
			int beg = row * 3 * basic_block + colmun * basic_block * 3;
			for (int m = 0;m <= basic_block - 1;m++) {
				for (int n = 0;n <= basic_block - 1;n++) {
					temp_frame[beg + m * 3 * length + n]=frames[i][pointer++];
				}
			}
		}
		delete[] frames[i];
		frames[i] = temp_frame;
	}
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
        //MessageBox(hwnd, TEXT("���̰���"), TEXT("������Ϣ"), MB_OK);
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
        int width = 1080;
        int height = 1920;
        BITMAPINFOHEADER infohead;
        infohead.biSize = sizeof(BITMAPINFOHEADER);
        infohead.biWidth = width;
        infohead.biHeight = height;
        infohead.biPlanes = 1;
        infohead.biClrUsed = 0;
        infohead.biBitCount = 24;
        infohead.biCompression = BI_RGB;
        infohead.biSizeImage = width * height * 3;//�޸Ĺ�Ҫ����biBitCount
        infohead.biXPelsPerMeter = 0;
        infohead.biYPelsPerMeter = 0;

        BITMAPFILEHEADER filehead;

        filehead.bfType = 0x4D42;  // "BM"
        filehead.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + infohead.biSizeImage;//������
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
        count_frame=(count_frame+1)%5;
        cout << "paint" << endl;

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
    default :
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
void recv_picture();
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    pBits = new char* [frames];
    for (int i = 0;i <= frames-1;i++) {
        pBits[i] = new char[6221824];
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
    connect_socket();
    DWORD recv_picture_thread_id;
    //CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)recv_picture, 0, 0, &recv_picture_thread_id);
    char * recvbuf=new char [1920*1080*3*2];
    char * tmp =new char [1920*1080*3*2];
    int * len=new int ;
    int recv_len=0;
    while(true){
        int recive=0;
        recv(s,(char*)len,4,0);
        while(len>recive)
        recive+=recv(s,recvbuf+recive,len-recive,0);
        UnHuffman(tmp,recvbuf);

        int k = 0;
        int temp[64];
        double Y[8 * 8];
        double Cb[8 * 8];
        double Cr[8 * 8];
        for (int i = 0;i < 1080/8;i ++) {
            for (int j = 0;j < 1920/8;j ++) {
            //cout << i << " " << j << endl;
            k=UnZigZag(outbuf, temp, k);

            IDCT(Y, temp, 1920, 1080, 0, 0);

            k=UnZigZag(outbuf, temp, k);

            IDCT(Cb, temp, 1920, 1080, 0, 1);

            k=UnZigZag(outbuf, temp, k);

            IDCT(Cr, temp, 1920, 1080, 0, 2);

            YCbCr_TO_RGB(pbits[0], Y, Cb, Cr, 1920, 1080, i * 1920 * 8 * 3 + j * 8 * 3, 0);
            YCbCr_TO_RGB(pbits[0], Y, Cb, Cr, 1920, 1080, i * 1920 * 8 * 3 + j * 8 * 3, 1);
            YCbCr_TO_RGB(pbits[0], Y, Cb, Cr, 1920, 1080, i * 1920 * 8 * 3 + j * 8 * 3, 2);
        }
    }
    Run_Inverse_Shift((unsigned char*)pbits[0], 1920 * 1080 * 3);

        for(int i=0;i<4;i++){
            recive=0;
            recv(s,(char*)len,4,0);
            while(len>recive)
            recive+=recv(s,recvbuf+recive,len-recive,0);
        }
    }


    while (1)
    {
        //maybe over 6220804
        //cout<<recive<<endl;
        /*for (int i = 0;i <= frames - 1;i++) {
            //int recive = recv(s, pBits[i], 6220804, 0);//maybe over 6220804
            //cout<<recive<<endl;
            PostMessage(hwnd, WM_PAINT, 0, 0);
            Sleep(10);
            GetMessage(&msg, NULL, 0, 0);
            TranslateMessage(&msg);   //����������Ϣ����
            DispatchMessage(&msg);
        }*/
        //decompress_interframe(pBits,frames,1920,1080);
    }
}
void connect_socket() {
    WORD wversionrequested = MAKEWORD(2, 2);
    WSADATA wsadata;
    int werror = 1;
    werror = WSAStartup(wversionrequested, &wsadata);
    if(werror)
    {
      if(WSAGetLastError()==10004) cout<<"��������ֹ"<<endl;
      else if(WSAGetLastError()==10013) cout<<"���ʱ��ܾ�"<<endl;
      else if(WSAGetLastError()==10014) cout<<"��ַ����"<<endl;
      else if(WSAGetLastError()==10022) cout<<"��������"<<endl;
      else if(WSAGetLastError()==10024) cout<<"��̫���sockets"<<endl;
      else if(WSAGetLastError()==10036) cout<<"һ���������ڽ���"<<endl;
      else if(WSAGetLastError()==10037) cout<<"һ���������ڽ���"<<endl;
    }

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == INVALID_SOCKET)
    {
        cout<<"socket set up:  "<<WSAGetLastError()<<endl;
    }
    int nsendbuf = 6220804;
    int set_buf=setsockopt(s, SOL_SOCKET, SO_SNDBUF, (const char *)&nsendbuf, sizeof(int));
    if (set_buf != 0) {
        cout << "set buf error" << endl;
    }
    sockaddr_in saddress;
    memset(&saddress, 0, sizeof(saddress));
    saddress.sin_family = AF_INET;
    saddress.sin_port = htons(46130);//
    saddress.sin_addr.s_addr = inet_addr("103.46.128.49");//����ת�����ܳ���
    int connect_error;
    connect_error = connect(s, (sockaddr*)&saddress, sizeof(saddress));//�ڶ����������ܳ���3���������ܴ�
    if (connect_error == -1)
    {
        std::cout << "connect error" << std::endl;
        std::cout << "connect:  " << WSAGetLastError() << std::endl;
        system("pause");
    }
    std::cout << "connect success" << std::endl;

}
void recv_picture(){

}
