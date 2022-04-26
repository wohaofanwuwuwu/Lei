#ifndef COMPRESSION_H
#define COMPRESSION_H
#include <Windows.h>
#define basic_block 8
#define PI (3.1415926)
#define frames_num 5
#define queue_size 12
#define compress_procedure_num 4

struct Compress_Procedures_Signal {
    HANDLE signals[compress_procedure_num];
    const char* names[compress_procedure_num] ;
};
extern Compress_Procedures_Signal compress_process;
extern Compress_Procedures_Signal read_process;

void interframe_compression(char** frames, int num_frame, int length, int width);
int paper_cut(int old_length, int new_length);
void DCT(unsigned char* buf, int* dct, int width, int height, int beg,int fenliang);
//void IDCT(double* buf, int* dctbuf, int width, int height, int beg,int fenliang);
void YCbCr_TO_RGB(char* rgbbuf, double* Y, double* Cb, double* Cr, int width, int height, int beg, int fenliang);
void DCT_OTHER_SIZE(char* buf, int width, int height);
void screenshot(char** pBits);
//void Run_Inverse_Shift(unsigned char* buf,int size);
void Run_Shift_YCbCr(unsigned char* buf, int size);
int around(double a);
int ZigZag(int* buf, char* out, int beg);
//int UnZigZag( char* buf, int* out, int beg);
void Init_Shift_Table();

extern int Light_Stride_Table[64];
extern int Chroma_Stride_Table[64];
extern int Null_Table[64];
extern int Shift_YCbCr[256];
extern int Inverse_Shift_YCbCr[256];


#endif