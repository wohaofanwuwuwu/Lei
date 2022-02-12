#include <iostream>
#include "interframe.h"
using namespace std;
enum big_small {
	big,
	small
};
big_small judge_big_small() {
		int val = 0x11223344;
		char* p = (char*)&val;
		if (*p == 0x44)//若低地址为低位0x44，则为小端模式
			return small;
		if (*p == 0x11)//若低地址为高位0x11，则为大端模式
			return big;
}
void interframe_compression(char ** frames,int num_frame,int length,int width) {
	char* first_frame = frames[0];
	int column = length / basic_block;
	int row = width / basic_block;
	char* bb_cur;
	char* bb_pre;
	big_small t= judge_big_small();
	char* temp_frame;
	
	for (int i = num_frame-1;i >= 1;i--) {
		int pointer = 4;
		int count = 0;
		int new_length = length;
		int new_width = width;
		temp_frame = new char[length * width*3];//maybe just new one
		
		for (int j = 0;j <= row - 1;j++) {
			for (int k = 0;k <= column - 1;k++) {
				//@cut
				
				int per_length=paper_cut(length, new_length);
				int per_width=paper_cut(width, new_width);
				int p = j * length * 3 * basic_block + k * basic_block * 3;
				
				bb_cur=&frames[i][p];
				bb_pre = &frames[i-1][p];
				bool same = true;
				for (int m = 0;m <= basic_block-1;m++) {
					for (int n = 0;n <= basic_block-1;n++) {
						if (bb_cur[length*3 * m + n] != bb_pre[length*3 * m + n]) {
							same = false;
							goto next_block;
						}
					}
				}
			next_block:
				if (!same) {
						count++;
						temp_frame[pointer++] = j & 0xff;
						
						temp_frame[pointer++] = (j >> 8) & 0xff;
						
						temp_frame[pointer++] = k & 0xff;
						temp_frame[pointer++] = (k >> 8) & 0xff;

						for (int m = 0;m <= basic_block-1;m++) {
							for (int n = 0;n <= basic_block-1;n++) {
								temp_frame[pointer++] = bb_cur[length*3 * m + n];
							}
						}
				}
			}
		}
		//cout << "frame: " << i << "count :" << count<<endl;
		temp_frame[0] = count & 0xff;
		temp_frame[1] = (count >> 8) & 0xff;
		temp_frame[2] = (count >> 16) & 0xff;
		temp_frame[3] = (count >> 24) & 0xff;
		delete [] frames[i];
		frames[i] = temp_frame;
	}
}
int paper_cut(int old_length, int new_length) {
	int per_length = (new_length - old_length) >= 0 ? (new_length - old_length) : (old_length - new_length);
	return per_length;
}