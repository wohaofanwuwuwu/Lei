#include <iostream>
#include <math.h>
#include<iomanip>
#include "compression.h"
using namespace std;
float DCT_COS[8][8] = {
                    0.35355338f, 0.35355338f, 0.35355338f, 0.35355338f, 0.35355338f, 0.35355338f, 0.35355338f, 0.35355338f,
                    0.49039263f, 0.41573480f, 0.27778509f, 0.09754512f, -0.09754516f, -0.27778518f, -0.41573483f, -0.49039266f,
                    0.46193978f, 0.19134171f, -0.19134176f, -0.46193978f, -0.46193978f, -0.19134156f, 0.19134180f, 0.46193978f,
                    0.41573480f, -0.09754516f, -0.49039266f, -0.27778500f, 0.27778521f, 0.49039263f, 0.09754504f, -0.41573489f,
                    0.35355338f, -0.35355338f, -0.35355332f, 0.35355350f, 0.35355338f, -0.35355362f, -0.35355327f, 0.35355341f,
                    0.27778509f, -0.49039266f, 0.09754521f, 0.41573468f, -0.41573489f, -0.09754511f, 0.49039266f, -0.27778542f,
                    0.19134171f, -0.46193978f, 0.46193978f, -0.19134195f, -0.19134149f, 0.46193966f, -0.46193987f, 0.19134195f,
                    0.09754512f, -0.27778500f, 0.41573468f, -0.49039260f, 0.49039271f, -0.41573480f, 0.27778557f, -0.09754577f
};
int Null_Table[64] = {
                    1,1,1,1,1,1,1,1,
                    1,1,1,1,1,1,1,1,
                    1,1,1,1,1,1,1,1,
                    1,1,1,1,1,1,1,1,
                    1,1,1,1,1,1,1,1,
                    1,1,1,1,1,1,1,1,
                    1,1,1,1,1,1,1,1,
                    1,1,1,1,1,1,1,1
};
int Light_Stride_Table[64] = {
                    1,11,10,16,24,40,51,61,
                    12,12,14,19,26,58,60,55,
                    14,13,16,24,40,57,69,56,
                    14,17,22,29,51,87,80,62,
                    18,22,37,56,68,109,103,77,
                    24,35,55,64,81,104,113,92,
                    49,64,78,87,103,121,120,101,
                    72,92,95,98,112,100,103,99
};
int Chroma_Stride_Table[64] = {
                    1,18,24,47,99,99,99,99,
                    18,21,26,66,99,99,99,99,
                    24,26,56,99,99,99,99,99,
                    47,66,99,99,99,99,99,99,
                    99,99,99,99,99,99,99,99,
                    99,99,99,99,99,99,99,99,
                    99,99,99,99,99,99,99,99,
                    99,99,99,99,99,99,99,99
};
int Shift_YCbCr[256];
int Inverse_Shift_YCbCr[256];
void Init_Shift_Table() {
    int beg = 128;
    for (int i = 0, j = 0;i < 256;i += 2, j++) {
        Shift_YCbCr[i] = beg+j ;
        Inverse_Shift_YCbCr[beg+j] = i  ;
    }
    for (int i = 1, j = 1;i < 256;i += 2, j++) {
        Shift_YCbCr[i] = beg-j ;
        Inverse_Shift_YCbCr[beg-j] = i;
    }

}
void Run_Shift_YCbCr(char * buf,int size) {
    for (int i = 0;i < size;i++) {
        buf[i] = (char)Shift_YCbCr[(int)(unsigned char)buf[i]];
    }
}
void Run_Inverse_Shift(char * buf,int size) {
    for (int i = 0;i < size;i++) {
        
        buf[i] = (char)Inverse_Shift_YCbCr[(int)(unsigned char)buf[i]];
    }
}
int around(double a)
{
    if (a >= 0)
    {
        return int(a + 0.5);
    }
    else
    {
        return int(a - 0.5);
    }
}
void RGB_TO_YCbCr(char *rgbbuf,double *ycbcr, int width, int height, int beg,int fenliang) {
    for (int i = 0;i < 8;i++) {
        for (int j = 0;j < 8;j++) {
            if (fenliang == 0) {//Y fenliang
                ycbcr[i*8+j] = 0.114 * rgbbuf[beg + i * width * 3 + j * 3]+ 0.587 *rgbbuf[beg + i * width * 3 + j * 3+1]+ 0.299 *rgbbuf[beg + i * width * 3 + j * 3+2];
            }
            else if (fenliang == 1) {//Cb
                ycbcr[i * 8 + j] = 0.5 * rgbbuf[beg + i * width * 3 + j * 3 ] + (-0.3313) * rgbbuf[beg + i * width * 3 + j * 3 +1] + (-0.1687) * rgbbuf[beg + i * width * 3 + j * 3 + 2]+128;
            }
            else {//Cr
                ycbcr[i * 8 + j] = (-0.0813) * rgbbuf[beg + i * width * 3 + j * 3] + (-0.4187) * rgbbuf[beg + i * width * 3 + j * 3 + 1] + (0.5) * rgbbuf[beg + i * width * 3 + j * 3 + 2]+128;
            }
            
        }
    }
    /*cout << "===========transform to ycbcr==========" << endl;
    for (int i = 0;i < 8;i++) {
        for (int j = 0;j < 8;j++) {
            cout << ycbcr[i * 8 + j] << "   ";
        }
        cout << endl;
    }
    cout<< "===========transform to ycbcr==========" << endl;*/
    /*test rgb to ycrcb ,put it in server.cpp
    for (int i = 0;i < 1080;i++) {
        for (int j = 0;j < 1920;j+=3) {
                Y = 0.114 * pbits[0][i * 1920 + j] + 0.587 * pbits[0][i * 1920 + j + 1] + 0.299 * pbits[0][i * 1920 + j + 2];
                Cb= 0.5 * pbits[0][i * 1920 + j] + (-0.3313) * pbits[0][i * 1920 + j + 1] + -0.1687 * pbits[0][i * 1920 + j + 2]+128;
                Cr = -0.0813 * pbits[0][i * 1920 + j] + -0.4187 * pbits[0][i * 1920 + j + 1] + 0.5 * pbits[0][i * 1920 + j + 2] + 128;
                pbits[0][i * 1920 + j+2] = (int)((Y + 1.402 * (Cr - 128))-0.5);
                pbits[0][i * 1920 + j+1] = (int)((Y - 0.34414 * (Cb - 128)-0.71414*(Cr-128)) - 0.5);
                pbits[0][i * 1920 + j] = (int)((Y + 1.772 * (Cb - 128)) - 0.5);
        }
    }
    */
}
void DCT(char* buf, double* dctbuf,int width,int height,int beg,int fenliang) {
    float DctMapTmp[8][8];
    double YCbCr[8*8];
    float t = 0;
    int i, j, k;

    RGB_TO_YCbCr(buf, YCbCr, width, height, beg, fenliang);
    
    //preprocess
    /* cout << "===========YCbCr===========" << endl;
    for (i = 0;i < 8;i++) {
        for (j = 0;j < 8;j++) {
            cout<<YCbCr[i * 8 + j]<<" ";
        }
        cout << endl;
    }
    cout << "=============YCbCr=========" << endl;
    
    Run_Shift_YCbCr(temp);
    /*cout << "===========Shift YCbCr===========" << endl;
    for (i = 0;i < 8;i++) {
        for (j = 0;j < 8;j++) {
            cout << YCbCr[i * 8 + j] << " ";
        }
        cout << endl;
    }
    cout << "=============Shift YCbCr=========" << endl;
    */
    for (i = 0;i < basic_block;i++) {
        for (j = 0;j < basic_block;j++) {
            t = 0;
            
            for (k = 0;k < basic_block;k++) {
                //t+=DCT_Mat[i][k]*DctMap[k][j];
                t += DCT_COS[i][k] * YCbCr[k * 8 + j];
                //if not transform to yuv,use below 
                // t += DCT_COS[i][k] * buf[beg+fenliang+k*width*3+3*j];
            }
                
            
            DctMapTmp[i][j] = t;
        }
    }
    for (i = 0;i < basic_block;i++) {
        for (j = 0;j < basic_block;j++) {
            t = 0;
            for (k = 0;k < basic_block;k++) {
                //t+=DctMapTmp[i][k]*DCT_Mat[j][k];
                t += DctMapTmp[i][k] * DCT_COS[j][k];
            }
            dctbuf[i*basic_block+j] = t;
        }
    }
    //简单 的量化 
    /*if (fenliang != 0) {
        for (int i = 7;i >=0;i--) {
            for (int j = 13 - i;j < 8;j++) {
                dctbuf[i * 8 + j] = 0;
            }
        }
    }*/
    int* table;
    if (fenliang == 0) {
        table = Light_Stride_Table;
    }
    else {
        table = Chroma_Stride_Table;
    }
     //   table = Null_Table;
    for (int i = 0;i < 8;i++) {
        for (int j = 0;j < 8;j++) {
            //dctbuf[i * 8 + j] = 0;
            if (dctbuf[i * 8 + j] >= 0) {
                dctbuf[i * 8 + j] = ((int)((dctbuf[i * 8 + j]*10)/table[i*8+j] + 0.5));
            }
            else {
                dctbuf[i * 8 + j] = ((int)((dctbuf[i * 8 + j]*10)/table[i*8+j] - 0.5));
            }
            
        }
    }
    /*cout << "=========DCT========" << endl;
    for (int i = 0;i < basic_block;i++)
    {
        for (int j = 0;j < basic_block;j++)
        {
            cout << dctbuf[i * basic_block + j] << " ";
        }
        cout << endl;
    }
    cout << "=========DCT========" << endl;*/
    //for test dct,please put it in server.cpp
    /*
    double* dctbuf = new double[64];
    cout << "original picture:" << endl;
    for (int i = 0;i < basic_block;i++) {
        for (int j = 0;j < basic_block;j++) {
            cout << (int)pbits[0][i * 1920 * 3 + j * 3] << " ";
        }
        cout << endl;
    }
    for (int fenliang = 0;fenliang < 3;fenliang++) {
        for (int i = 0;i < 1080 / 8;i++) {
            for (int j = 0;j < 1920 / 8;j++) {
                DCT(pbits[0], dctbuf, 1920, 1080, i*1920*8*3+j*8*3,fenliang);
                IDCT(pbits[0], dctbuf, 1920, 1080, i * 1920 * 8 * 3 + j * 8 * 3 ,fenliang);
                //cout << 1 << endl;;
            }
        }
    }
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
    */
}

void DCT_OTHER_SIZE(char *buf,int width,int height) {
    double alpha,beta;
    
    int m = 0, n = 0, k = 0, l = 0;
    int NUM = 8;
    double* output = new double[NUM * NUM];
    cout << "===========original picture=============" << endl;
    for (int i = 0;i < NUM;i++) {
        for (int j = 0;j < NUM;j++) {
            cout << (int)buf[i * width * 3 + j * 3] << " ";
        }
        cout << endl;
    }
    cout << "===========original picture=============" << endl;
    for (k = 0;k < NUM;k++)
    {
        for (l = 0;l < NUM;l++)
        {
            if (k == 0)
            {
                alpha = sqrt(1.0 / NUM);
            }
            else
            {
                alpha = sqrt(2.0 / NUM);
            }
            if (l == 0)
            {
                beta = sqrt(1.0 / NUM);
            }
            else
            {
                beta = sqrt(2.0 / NUM);
            }
            double temp = 0.0;
            for (m = 0;m < NUM;m++)
            {
                for (n = 0;n < NUM;n++)
                {
                    temp += buf[m*width*3+3*n] * cos((2 * m + 1) * k * PI / (2.0 * NUM)) * cos((2 * n + 1) * l * PI / (2.0 * NUM));
                }
            }
            output[k*NUM+l] =alpha * beta * temp;
        }
    }
    cout << "==========DCT TRANSFORM==========" << endl;
    cout.precision(2);
    for (int i = 0;i < NUM;i++) {
        for (int j = 0;j < NUM;j++) {
            cout <<output[i*NUM+j]<< " ";
        }
        cout << endl;
    }
    cout << "==========DCT TRANSFORM==========" << endl;
}