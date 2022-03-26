#ifndef HUFFMAN_H
#define HUFFMAN_H
struct Huffman_Tree_Node {
	char zifu;
	char distance;
};
struct Pair_Tree_Node {
	Huffman_Tree_Node left;
	Huffman_Tree_Node right;
};
struct Huffman_Node {
	char zifu;
	int power;
	int left;
	int right;
	int parent;
	int flag;
	Huffman_Node() {
		power = 0;
		left = right = parent = flag = -1;
	}
};
struct MinHeap {
	Huffman_Node* top;
	int position;
	int maxsize;
	MinHeap() {
		maxsize = 256;
		position = 1;
	}
	void push(Huffman_Node node) {
		top[position] = node;
		float_up(top + position, position);
		position++;
	}
	Huffman_Node pop() {
		Huffman_Node temp = top[position - 1];
		top[position - 1] = top[1];
		top[1] = temp;
		position--;
		sink(top+1 , 1);
		
		return top[position];
	}
	void float_up(Huffman_Node* n, int pos) {
		while (top[pos].power < top[pos >> 1].power && pos != 1) {
			Huffman_Node temp = top[pos >> 1];
			top[pos >> 1] = top[pos];
			top[pos] = temp;
			pos = pos >> 1;
		}
	}
	void sink(Huffman_Node* n, int pos) {
			Huffman_Node temp = top[pos];
			int child = pos << 1;
			if (pos == position - 1) {
				return;
			}
			while (child < position) {
				if (child + 1 < position && top[child + 1].power < top[child].power) {
					child++;
				}
				if (top[child].power >= temp.power) {
					break;
				}
				else {
					top[pos] = top[child];
					pos = child;
					child = pos << 1;
				}
			}
			top[pos] = temp;
	}
	bool only_one() {
		return position == 2;
	}
	bool empty() {
		return position == 1;
	}
};
struct Huffman_Tree {
	Huffman_Node* head;
	int size;
};
int UnHuffman(char* result, char* buf);
int HuffmanTree2DataPacket(char* result, Huffman_Tree* t, char* buf, int size);
Huffman_Tree* CreateHuffmanTree(char* buf, int size);
#endif