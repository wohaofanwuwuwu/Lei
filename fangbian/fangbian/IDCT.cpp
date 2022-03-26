#include <iostream>
#include <math.h>
#include "compression.h"
using namespace std;
float IDCT_COS[basic_block][basic_block] = {
                    0.35355338f, 0.49039263f, 0.46193978f, 0.41573480f, 0.35355338f, 0.27778509f, 0.19134171f, 0.09754512f,
                    0.35355338f, 0.41573480f, 0.19134171f, -0.09754516f, -0.35355338f, -0.49039266f, -0.46193978f, -0.27778500f,
                    0.35355338f, 0.27778509f, -0.19134176f, -0.49039266f, -0.35355332f, 0.09754521f, 0.46193978f, 0.41573468f,
                    0.35355338f, 0.09754512f, -0.46193978f, -0.27778500f, 0.35355350f, 0.41573468f, -0.19134195f, -0.49039260f,
                    0.35355338f, -0.09754516f, -0.46193978f, 0.27778521f, 0.35355338f, -0.41573489f, -0.19134149f, 0.49039271f,
                    0.35355338f, -0.27778518f, -0.19134156f, 0.49039263f, -0.35355362f, -0.09754511f, 0.46193966f, -0.41573480f,
                    0.35355338f, -0.41573483f, 0.19134180f, 0.09754504f, -0.35355327f, 0.49039266f, -0.46193987f, 0.27778557f,
                    0.35355338f, -0.49039266f, 0.46193978f, -0.41573489f, 0.35355341f, -0.27778542f, 0.19134195f, -0.09754577f
};
void YCbCr_TO_RGB(char* rgbbuf, double* Y,  double* Cb, double* Cr,int width, int height, int beg, int fenliang) {
    double wucha;
    for (int i = 0;i < 8;i++) {
        for (int j = 0;j < 8;j++) {
            if (fenliang == 0) {//B fenliang
                rgbbuf[beg + fenliang + i * width * 3 + 3 * j]=(int)around((Y[i*8+j] + 1.772 * (Cb[i * 8 + j] - 128)));
            }
            else if (fenliang == 1) {//G
                rgbbuf[beg + fenliang + i * width * 3 + 3 * j]=(int)around((Y[i * 8 + j] - 0.34414 * (Cb[i * 8 + j] - 128) - 0.71414 * (Cr[i * 8 + j] - 128)));
            }
            else {//R
                rgbbuf[beg + fenliang + i * width * 3 + 3 * j]=(int)around((Y[i * 8 + j] + 1.402 * (Cr[i * 8 + j] - 128)) );
            }

        }
    }
}
void IDCT(double* buf, int * dctbuf, int width, int height, int beg,int fenliang) {
    float DctMapTmp[basic_block][basic_block];
    float t = 0;
    int i, j, k;
    int* table;
    if (fenliang == 0) {
        table = Light_Stride_Table;
    }
    else {
        table = Chroma_Stride_Table;
    }
    //table = Null_Table;
    for (i = 0;i < basic_block;i++) {
        for (j = 0;j < basic_block;j++) {
            t = 0;
            for (k = 0;k < basic_block;k++) {
                //t+=DCT_Mat[k][i]*DctMap[k][j];
                t += IDCT_COS[i][k] * ((dctbuf[k * basic_block + j]*table[k*basic_block+j])/1.0);
            }
            DctMapTmp[i][j] = t;
        }
    }
    for (i = 0;i < basic_block;i++) {
        for (j = 0;j < basic_block;j++) {
            t = 0;
            for (k = 0;k < basic_block;k++) {
                //t+=DctMapTmp[i][k]*DCT_Mat[k][j]
                t += DctMapTmp[i][k] * IDCT_COS[j][k];
            }
            buf[i*8+j] = t;
            if (t < 0) {
                //buf[beg+fenliang+i * width * 3 + 3 * j] = (int)(t-0.5);

            }
            else {
                //buf[beg+fenliang+i * width * 3 + 3 * j] = (int)(t+0.5);
            }

        }
    }

    /*cout << "=========IDCT========" << endl;
    for (int i = 0;i < basic_block;i++)
    {
        for (int j = 0;j < basic_block;j++)
        {
            cout << (int)buf[i*8+j] << " ";
        }
        cout << endl;
    }
    cout << "=========IDCT========" << endl;

    Run_Inverse_Shift(temp);

    /*cout << "=========inverse========" << endl;

    for (int i = 0;i < basic_block;i++)
    {
        for (int j = 0;j < basic_block;j++)
        {
            cout << (int)buf[i * 8 + j] << " ";
        }
        cout << endl;
    }*/
}
