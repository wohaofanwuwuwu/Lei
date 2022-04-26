#include <iostream>
#include <windows.h>
#include "compression.h"
#include <fstream>

using namespace std;
void screenshot(char ** pBits)
{
    //mddc = CreateCompatibleDC(hddc);
    HDC hddc;
    HDC mddc;
    hddc=CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL);

    int width=GetDeviceCaps(hddc, DESKTOPHORZRES);
    int height=GetDeviceCaps(hddc, DESKTOPVERTRES);


    mddc = CreateCompatibleDC(hddc);
    HBITMAP hbmp=CreateCompatibleBitmap(hddc,width,height);
    SelectObject(mddc,hbmp);
    //BitBlt(mddc,0,0,width,height,hddc,0,0,SRCCOPY);
    //HBITMAP h=(HBITMAP)SelectObject(mddc,hbmp);
    BITMAPINFOHEADER infohead;

    infohead.biSize = sizeof(BITMAPINFOHEADER);
    infohead.biWidth = width;
    infohead.biHeight = height;
    infohead.biPlanes = 1;
    infohead.biClrUsed = 0;
    infohead.biBitCount = 24;
    infohead.biCompression = BI_RGB;
    infohead.biSizeImage = width*height*3;//�޸Ĺ�Ҫ����biBitCount
    infohead.biXPelsPerMeter =0;
    infohead.biYPelsPerMeter =0;

    BITMAPFILEHEADER filehead;

    filehead.bfType = 0x4D42;  // "BM"
    filehead.bfSize =sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + infohead.biSizeImage;//������
    filehead.bfReserved1 = 0;
    filehead.bfReserved2 = 0;
    filehead.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    BITMAPINFO amp;
    amp.bmiHeader=infohead;
    /*char ** pBits=new char* [frames_num];
    for(int i=0;i<=frames_num-1;i++)
    pBits[i]=new char [width*height*3];*/
    while (true) {
        for (int i = 0;i < queue_size;i++) {
            //cout << "read clock " << i << endl;
            if (i % 3 == 0) {
                WaitForSingleObject(read_process.signals[i/3],INFINITE);//wait for first decompress pthread complete
            }
            for (int counting = 0;counting <= frames_num - 1;counting++) {
                BitBlt(mddc, 0, 0, width, height, hddc, 0, 0, SRCCOPY);
                GetDIBits(mddc, hbmp, 0, height, (pBits+5*i)[counting], (LPBITMAPINFO) & amp, DIB_RGB_COLORS);
            }
            //interframe compression pBits address = 5*i???
            interframe_compression(pBits + 5 * i, frames_num, width, height);
            if ((i+1) % 3 == 0) {
                ReleaseSemaphore(compress_process.signals[i/3],1,NULL);//launch decompress
            }
        }
        DeleteObject(hbmp);
        
    }
    
}
