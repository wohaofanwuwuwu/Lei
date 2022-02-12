#include "interframe.h"
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