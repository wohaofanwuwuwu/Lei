#ifndef INTERFRAME_H
#define INTERFRAME_H
#define basic_block 8
#define frames_num 5
void interframe_compression(char** frames, int num_frame, int length, int width);
int paper_cut(int old_length, int new_length);
char** screenshot();
#endif