#include <iostream>
#include "Huffman.h"
#include <map>
using namespace std;
std::map<char, int> MapHuffman_Node;
/*
word[0]=ÑÚÂë£¬word[1-4]=int(data size),word[5-6]=short(Huffman tree offset of end)
word[7-end] =data

*/

Huffman_Tree* CreateHuffmanTree(char* buf, int size) {
	//counting
	Huffman_Node* node = new Huffman_Node[256];
	MinHeap heap;
	heap.top = new Huffman_Node[257];
	for (int i = 0;i < 256;i++) {
		node[i].zifu = i;
	}
	for (int i = 0;i < size;i++) {
		node[(int)((unsigned char)buf[i])].power++;//+1?
	}
	int count = 0;
	for (int i = 0;i < 256;i++) {
		if (node[i].power != 0) {
			count++;
			//cout << node[i].zifu <<"  power :"<<node[i].power<< endl;
			heap.push(node[i]);
			for (int j = 1;j < heap.position;j++) {
				//cout << heap.top[j].zifu << " ";
			}
			//cout << endl;
		}
	}
	Huffman_Tree* t=new Huffman_Tree;
	t->head = NULL;
	if (count != 0) {
		t->head = new Huffman_Node[2 * count - 1];
	}
	t->size = 2 * count - 1;
	int pos = 0;
	if (heap.empty()) {
		goto END;
	}
	else if(heap.only_one()) {
		Huffman_Node tmp = heap.pop();
		tmp.flag = 1;
		t->head[0] = tmp;
		goto END;
	}
	int c = 0;
	char linshi = '0';
	while (!heap.only_one()) {
		Huffman_Node compos;
		
		Huffman_Node min1 = heap.pop();
		//cout << "pop :" << min1.zifu <<"power :"<<min1.power<< endl<<"leave :" ;
		for (int j = 1;j < heap.position;j++) {
			//cout << heap.top[j].zifu << " ";
		}
		//cout << endl;
		min1.flag = 0;
		//min1.parent = pos + 2;
		if (min1.left != -1) {
			t->head[min1.left].parent = pos;
			t->head[min1.right].parent = pos;
		}
		t->head[pos] = min1;
		compos.left = pos++;
		Huffman_Node min2 = heap.pop();
		
		//cout << "pop :" << min2.zifu <<"power :"<<min2.power<< endl << "leave :";
		for (int j = 1;j < heap.position;j++) {
			//cout << heap.top[j].zifu << " ";
		}
		//cout << endl;
		//cout << "the " << c << " time construct huffman tree :min1:"<<min1.zifu<<" min2 :"<<min2.zifu<<endl;
		c++;
		min2.flag = 1;
		//min2.parent = pos + 1;
		if (min2.left != -1) {
			t->head[min2.left].parent = pos;
			t->head[min2.right].parent = pos;
		}
		t->head[pos] = min2;
		compos.right = pos++;
		compos.zifu = linshi++;
		compos.power = min1.power + min2.power;
		heap.push(compos);
		//cout << "push and :" << endl;
		for (int j = 1;j < heap.position;j++) {
			//cout << heap.top[j].zifu << " ";
		}
		//cout << endl;
	}
	{
		Huffman_Node compos=heap.pop();
		if (compos.left != -1) {
			t->head[compos.left].parent = pos;
			t->head[compos.right].parent = pos;
		}
		t->head[pos] = compos;
	}
	END:;
	//delete
	delete [] heap.top;
	delete [] node ;
	return t;
}
int HuffmanTree2DataPacket(char * result,Huffman_Tree* k,char *buf,int size) {
	//Huffman_Tree* k = CreateHuffmanTree(buf, size);

	for (int i = 0;i < k->size;i++) {
		if (k->head[i].left == -1)
			MapHuffman_Node[k->head[i].zifu] = i;
	}
	int num_word = 7;
	//
	int stack[2 * 256 - 1];
	int stack_top = 0;
	int stack_bottom = 0;
	stack[stack_top++] = k->size - 1;

	int L = k->head[stack[stack_bottom]].left;
	int R = k->head[stack[stack_bottom]].right;
	if (L != -1) {
		stack[stack_top++] = L;
		stack[stack_top++] = R;
	}
	else {
		result[num_word] = k->head[stack_bottom].zifu;
		result[num_word + 1] = 0;
		num_word += 2;
	}
	stack_bottom++;

	int weizhi = 0;
	while (stack_bottom < stack_top) {
		L = k->head[stack[stack_bottom]].left;
		R = k->head[stack[stack_bottom]].right;
		if (L != -1) {
			stack[stack_top++] = L;
			stack[stack_top] = R;
			result[num_word] = k->head[stack[stack_bottom]].zifu;
			
			result[num_word+1] = (stack_top - stack_bottom)/2;
			
			stack_top++;
			num_word += 2;
		}
		else {
			result[num_word] = k->head[stack[stack_bottom]].zifu;
			result[num_word + 1] = 0;
			
			num_word += 2;
		}
		stack_bottom++;
	}
	//

	result[5] = (short)num_word  & 0xff;
	result[6] = ((short)num_word >> 8) & 0xff;
	int up_to_eight = 0;

	for (int i = size-1;i >=0;i--) {
		int j;
		j = MapHuffman_Node[buf[i]];

		int num = k->size-1;
		while (j!=num) {
			result[num_word]=(result[num_word] << 1)| k->head[j].flag;
			
			up_to_eight++;
			if (up_to_eight == 8) {
				num_word++;
				up_to_eight = 0;
			}
			j=k->head[j].parent;
		}   
	}
	//cout << endl;
	result[1] = num_word & 0xff;
	result[2] = (num_word >> 8) & 0xff;
	result[3] = (num_word >> 16) & 0xff;
	result[4] = (num_word >> 24) & 0xff;
	result[0] = (unsigned char)up_to_eight;
	delete[] k->head;
	delete[] k;
	MapHuffman_Node.clear();
	return num_word;
}
int UnHuffman(char * result,char* buf) {
	char yanma=buf[0];
	cout <<"yanma :"<< (int)yanma<<" ";
	int size = *(int*)&buf[1];
	cout << "file size : " << size << " ";
	short offset = *(short*)&buf[5];

	int n = size;
	int count = 0;
	int count_eight = 8-(int)yanma;
	
	if (count_eight == 8) {
		n--;
		count_eight = 0;
	}
	Pair_Tree_Node* tree=(Pair_Tree_Node*)&buf[7];

	while (n >= offset) {
		int k = 0;
		int distance = 1;

		LOOP:
			if ((buf[n] & 0x1) == 0) {

				buf[n] = buf[n] >> 1;
				distance= (int)(unsigned char)tree[k].left.distance;
				
				count_eight++;
				if (count_eight >= 8) {
					count_eight = 0;
					n--;
				}
				if (distance == 0) {
					result[count++] = tree[k].left.zifu;
					//cout << endl;
					goto NEXT_WORD;
				}
			}
			else {
				
				buf[n] = buf[n] >> 1;
				distance= (int)(unsigned char)tree[k].right.distance;
				
				count_eight++;
				if (count_eight >= 8) {
					count_eight = 0;
					n--;
				}
				if (distance == 0) {
					result[count++] = tree[k].right.zifu;
					//cout << endl;
					////cout << "zifu :"<<result[count - 1] << endl;
					goto NEXT_WORD;
				}
			}
			k += distance;
			goto LOOP;
		NEXT_WORD:
			;
		
	}
	//cout << "unhuffman size: " << count << endl;
	return count;
}