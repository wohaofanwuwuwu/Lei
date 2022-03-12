#include <iostream>
using namespace std;
int ZigZagTable[64] =
{
	 0, 1,  8, 16,  9,  2,  3, 10,
	17, 24, 32, 25, 18, 11,  4,  5,
	12, 19, 26, 33, 40, 48, 41, 34,
	27, 20, 13,  6,  7, 14, 21, 28,
	35, 42, 49, 56, 57, 50, 43, 36,
	29, 22, 15, 23, 30, 37, 44, 51,
	58, 59, 52, 45, 38, 31, 39, 46,
	53, 60, 61, 54, 47, 55, 62, 63
};

int ZigZag(int *buf, char * out,int beg)
{
	short AC=(short)buf[0];
	out[beg] = AC & 0xff;
	out[beg+1] = (AC >> 8) & 0xff;
	int k = beg+2;
	for (int i = 1; i < 64; i++) {
		if (buf[ZigZagTable[i]] == 0) {
			out[k] = 0;
			k++;
			int count = 0;
			while (buf[ZigZagTable[i]] == 0) {
				count++;
				i++;
				if (i == 64) {
					out[k] = count;
					return k+1;
				}
			}
			out[k] = count;
			k++;
		}
		out[k] = buf[ZigZagTable[i]];
		k++;
	}
	return k;
}

int UnZigZag(char *buf, int * out,int beg)
{
	short AC;
	AC = *(short*)(buf+beg);
	out[0] = (int)AC;

	int k = beg+2;
	for (int i = 1; i < 64; i++) {

		if (buf[k] == 0) {
			k++;
			int count = (int)buf[k];
			for (int j = 0;j < count;j++,i++) {
				out[ZigZagTable[i]] = 0;

			}
			i--;
			k++;
		}
		else {
			out[ZigZagTable[i]] = (int)buf[k];
			k++;
		}
	}
	return k;

}