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
                infohead.biSizeImage = width*height*3;//修改过要乘以biBitCount
                infohead.biXPelsPerMeter =0;
                infohead.biYPelsPerMeter =0;

                BITMAPFILEHEADER filehead;

                filehead.bfType = 0x4D42;  // "BM"
                filehead.bfSize =sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + infohead.biSizeImage;//有歧义
                filehead.bfReserved1 = 0;
                filehead.bfReserved2 = 0;
                filehead.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

                BITMAPINFO amp;
                amp.bmiHeader=infohead;
                /*char ** pBits=new char* [frames_num];
                for(int i=0;i<=frames_num-1;i++)
                pBits[i]=new char [width*height*3];*/
                
               int counting=0;
                while (1)
                {

                    BitBlt(mddc, 0, 0, width, height, hddc, 0, 0, SRCCOPY);
                    GetDIBits(mddc, hbmp, 0, height, pBits[counting], (LPBITMAPINFO) & amp, DIB_RGB_COLORS);
                    
                    if(counting==frames_num-1)
                    {
                        
                        break;
                    }
                    counting++;
                }
                
                    interframe_compression(pBits, frames_num , width, height);
                    
                    DeleteObject(hbmp);
                    

}
