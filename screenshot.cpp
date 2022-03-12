#include <iostream>
#include <windows.h>
#include "compression.h"
#include <fstream>
using namespace std;
char** screenshot()
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
                char ** pBits=new char* [frames_num];
                for(int i=0;i<=frames_num-1;i++)
                pBits[i]=new char [width*height*3];
                
               int counting=0;
                while (1)
                {

                    BitBlt(mddc, 0, 0, width, height, hddc, 0, 0, SRCCOPY);
                    GetDIBits(mddc, hbmp, 0, height, pBits[counting], (LPBITMAPINFO) & amp, DIB_RGB_COLORS);
                    //cout<<(unsigned int)pBits[counting][2258]<<endl;
                    //cout << height << endl;
                    if(counting==frames_num-1)
                    {
                        /*    ofstream o;

                        o.open("1.bmp",ios::binary|ios::out);
                        o.write((char *)&filehead,sizeof(BITMAPFILEHEADER));
                        o.write((char *)&infohead,sizeof(BITMAPINFOHEADER ));
                        o.write((char *)pBits[0],width*height*3);
                        o.close();
                        cout<<"one picture"<<endl;
                    o.open("2.bmp",ios::binary|ios::out);
                    o.write((char *)&filehead,sizeof(BITMAPFILEHEADER));
                    o.write((char *)&infohead,sizeof(BITMAPINFOHEADER ));
                    o.write((char *)pBits[1],width*height*3);
                    o.close();
                    o.open("3.bmp",ios::binary|ios::out);
                    o.write((char *)&filehead,sizeof(BITMAPFILEHEADER));
                    o.write((char *)&infohead,sizeof(BITMAPINFOHEADER ));
                    o.write((char *)pBits[2],width*height*3);
                    o.close();
                    o.open("4.bmp",ios::binary|ios::out);
                    o.write((char *)&filehead,sizeof(BITMAPFILEHEADER));
                    o.write((char *)&infohead,sizeof(BITMAPINFOHEADER ));
                    o.write((char *)pBits[3],width*height*3);
                    o.close();
                    o.open("5.bmp",ios::binary|ios::out);
                    o.write((char *)&filehead,sizeof(BITMAPFILEHEADER));
                    o.write((char *)&infohead,sizeof(BITMAPINFOHEADER ));
                    o.write((char *)pBits[4],width*height*3);
                    o.close();*/
                    break;
                    }
                    counting++;
                    }
                
                    interframe_compression(pBits, frames_num , width, height);
                    //cout << strlen((char*)*pBits) << endl;
                    //cout << *(int*)pBits[1] << endl;
                    DeleteObject(hbmp);
                    //test decompress
                    //decompress_interframe(pBits, 5, width, height);
                    /*while (1)
                    {
                        if (counting == 4)
                        {
                            ofstream o;

                            o.open("6.bmp", ios::binary | ios::out);
                            o.write((char*)&filehead, sizeof(BITMAPFILEHEADER));
                            o.write((char*)&infohead, sizeof(BITMAPINFOHEADER));
                            o.write((char*)pBits[0], width * height * 3);
                            o.close();
                            o.open("7.bmp", ios::binary | ios::out);
                            o.write((char*)&filehead, sizeof(BITMAPFILEHEADER));
                            o.write((char*)&infohead, sizeof(BITMAPINFOHEADER));
                            o.write((char*)pBits[1], width * height * 3);
                            o.close();
                            o.open("8.bmp", ios::binary | ios::out);
                            o.write((char*)&filehead, sizeof(BITMAPFILEHEADER));
                            o.write((char*)&infohead, sizeof(BITMAPINFOHEADER));
                            o.write((char*)pBits[2], width * height * 3);
                            o.close();
                            o.open("9.bmp", ios::binary | ios::out);
                            o.write((char*)&filehead, sizeof(BITMAPFILEHEADER));
                            o.write((char*)&infohead, sizeof(BITMAPINFOHEADER));
                            o.write((char*)pBits[3], width * height * 3);
                            o.close();
                            o.open("10.bmp", ios::binary | ios::out);
                            o.write((char*)&filehead, sizeof(BITMAPFILEHEADER));
                            o.write((char*)&infohead, sizeof(BITMAPINFOHEADER));
                            o.write((char*)pBits[4], width * height * 3);
                            o.close();
                            break;
                        }
                        counting++;
                    }*/
                return pBits;
}
