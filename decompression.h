#ifndef DECOMPRESSION_H
#define DECOMPRESSION_H
#include <Windows.h>
//#include "compression.h"
#define basic_block 8
#define PI (3.1415926)
#define frames_num 5
#define queue_size 12
#define decompress_procedure_num 4

struct Decompress_Procedures_Signal {
	HANDLE signals[decompress_procedure_num];
	const char* names[decompress_procedure_num];
};

void IDCT(double* buf, int* dctbuf, int width, int height, int beg, int fenliang);
void Run_Inverse_Shift(unsigned char* buf, int size);
int UnZigZag(char* buf, int* out, int beg);
void Init_Shift_Table();
int around(double a);
void YCbCr_TO_RGB(char* rgbbuf, double* Y, double* Cb, double* Cr, int width, int height, int beg, int fenliang);

extern int Light_Stride_Table[64];
extern int Chroma_Stride_Table[64];
extern int Null_Table[64];
extern int Shift_YCbCr[256];
extern int Inverse_Shift_YCbCr[256];
void decompress_interframe(char ** frames,int length,int width) {
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

#endif