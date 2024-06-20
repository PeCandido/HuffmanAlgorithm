#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define SIZE_LIMIT 256

typedef struct node {
	char c;
    int frequency;
    struct node *left, *right;
} huff_node;

typedef struct {
	huff_node* nodes[SIZE_LIMIT];
    int size;
} queue;

queue* createQueue() {
	queue* q = (queue*) malloc(sizeof(queue));
	q->size = 0;
    return q;
}

huff_node* newNode(char c, int frequency) {
	huff_node* h_node = (huff_node*) malloc(sizeof(huff_node));
    h_node->c = c;
    h_node->frequency = frequency;
    h_node->left = NULL;
    h_node->right = NULL;
        
    return h_node;
}

void swapNodes(huff_node** x, huff_node** y) {
	huff_node* aux = *x;
    *x = *y;
    *y = aux;
}

void insertNode(queue* q, huff_node* h_node) {
	int i = q->size++;
    q->nodes[i] = h_node;

    while (i && (q->nodes[i]->frequency < q->nodes[(i - 1) / 2]->frequency || (q->nodes[i]->frequency == q->nodes[(i - 1) / 2]->frequency && q->nodes[i]->c < q->nodes[(i - 1) / 2]->c))) {
		swapNodes(&q->nodes[i], &q->nodes[(i - 1) / 2]);
		i = (i - 1) / 2;
	}
}

void heapify(queue* q, int i) {
	int smallest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < q->size && (q->nodes[left]->frequency < q->nodes[smallest]->frequency || (q->nodes[left]->frequency == q->nodes[smallest]->frequency && q->nodes[left]->c < q->nodes[smallest]->c))) {
		smallest = left;
	}

    if (right < q->size && (q->nodes[right]->frequency < q->nodes[smallest]->frequency || (q->nodes[right]->frequency == q->nodes[smallest]->frequency && q->nodes[right]->c < q->nodes[smallest]->c))) {
		smallest = right;
	}

    if (smallest != i) {
		swapNodes(&q->nodes[i], &q->nodes[smallest]);
        heapify(q, smallest);
    }
}

huff_node* extractMinimal(queue* q) {
	huff_node* temp = q->nodes[0];
    q->nodes[0] = q->nodes[--q->size];
    heapify(q, 0);
    return temp;
}

huff_node* treeConstructor(int* frequencies) {
	queue *q = createQueue();
    for (int i = 0; i < SIZE_LIMIT; i++) {
		if (frequencies[i] > 0) {
			insertNode(q, newNode((char)i, frequencies[i]));
		}
	}
	
	while (q->size > 1) {
		huff_node *left = extractMinimal(q);
        huff_node *right = extractMinimal(q);
        huff_node *parent = newNode(' ', left->frequency + right->frequency);
        parent->left = left;
        parent->right = right;
        insertNode(q, parent);
    }

    huff_node *root = extractMinimal(q);
    free(q);
    return root;
}

void codeConstructor(huff_node* h_node, char* code, int depth, char codes[SIZE_LIMIT][SIZE_LIMIT]) {
	if (!h_node->left && !h_node->right) {
		code[depth] = '\0';
        strcpy(codes[(unsigned char) h_node->c], code);
        return;
    }
    
    if (h_node->left) {
		code[depth] = '0';
        codeConstructor(h_node->left, code, depth + 1, codes);
    }
        
    if (h_node->right) {
		code[depth] = '1';
        codeConstructor(h_node->right, code, depth + 1, codes);
    }
}

void buildCode(huff_node* root, char codes[SIZE_LIMIT][SIZE_LIMIT]) {
	char code[SIZE_LIMIT];
    codeConstructor(root, code, 0, codes);
}

void printCodes(char codes[SIZE_LIMIT][SIZE_LIMIT], int *frequencies) {
	for (int i = 0; i < SIZE_LIMIT; i++) {
		if (codes[i][0] != '\0') {
			printf("%c com frequencia %d codificado como %s\n", i, frequencies[i], codes[i]);
        }
    }
}

void encode(char *text, char codes[SIZE_LIMIT][SIZE_LIMIT], char *encodedText) {
	encodedText[0] = '\0';
	
    for (int i = 0; text[i]; i++) {
		strcat(encodedText, codes[(unsigned char) text[i]]);
    }
}

void decode(char* encodedText, huff_node* root, char* decodedText) {
	huff_node *current = root;
	int index = 0;
	
	for (int i = 0; encodedText[i]; i++) {
		if (encodedText[i] == '0') {
			current = current->left;
		} else {
			current = current->right;
		}
		
		if (!current->left && !current->right) {
			decodedText[index++] = current->c;
            current = root;
        }
    }
    
    decodedText[index] = '\0';
}

int countDiffChars(char *text) {
	int count = 0;
    int chars[SIZE_LIMIT] = {0};
    
    for (int i = 0; text[i]; i++) {
		if (!chars[(unsigned char) text[i]]) {
			chars[(unsigned char) text[i]] = 1;
            count++;
        }
    }
    
    return count;
}

void calcCompression(char *text, char *encodedText) {
	int originalBits = strlen(text) * 8;
    int encodedBits = strlen(encodedText);
    int chars = countDiffChars(text);
    int minimalBits = (int) ceil(log2(chars)) * strlen(text);

    double compression8Bits = (double)encodedBits / originalBits * 100;
    double compressionMinimalBits = (double)encodedBits / minimalBits * 100;

    printf(" %.2f%% %.2f%%\n", compression8Bits, compressionMinimalBits);
}

void freeTree(huff_node *h_node) {
	if (!h_node) {
		return;
	}
	
    freeTree(h_node->left);
    freeTree(h_node->right);
    free(h_node);
}

void clearFreq(int *freq) {
	for (int i = 0; i < SIZE_LIMIT; i++) {
		freq[i] = 0;
    }
}

int main() {
	huff_node* root = NULL;
	int frequencies[SIZE_LIMIT] = {0};
    char codes[SIZE_LIMIT][SIZE_LIMIT] = {0};
    char operation[SIZE_LIMIT];
    
    char character;
    int frequency;
    
    
    printf("Operation List: (write with lowercase)\n- add E freq\n- calc\n- print\n- cod STR\n- dec bits\n- clear\n- exit\n\n\n");
    
    while (1) {
		printf("-> ");
        scanf("%s", operation);
        
        if (strcmp(operation, "add") == 0) {
			scanf(" %c %d", &character, &frequency);
            frequencies[(unsigned char)character] = frequency;
            
        } else if (strcmp(operation, "calc") == 0) {
            if (root) {
				freeTree(root);
            }
            
            root = treeConstructor(frequencies);
            buildCode(root, codes);
            
        } else if (strcmp(operation, "print") == 0) {
			printCodes(codes, frequencies);
			
        } else if (strcmp(operation, "cod") == 0) {
			char text[SIZE_LIMIT], encodedText[SIZE_LIMIT];
            scanf("%s", text);
            encode(text, codes, encodedText);
            printf("%s", encodedText);
            calcCompression(text, encodedText);
            
        } else if (strcmp(operation, "dec") == 0) {
			char encodedText[SIZE_LIMIT], decodedText[SIZE_LIMIT];
            scanf("%s", encodedText);
            decode(encodedText, root, decodedText);
            printf("%s\n", decodedText);
            
        } else if (strcmp(operation, "clear") == 0) {
			clearFreq(frequencies);
            memset(codes, 0, sizeof(codes));
            
            if (root) {
				freeTree(root);
                root = NULL;
            }
            
        } else if (strcmp(operation, "exit") == 0) {
			break;
			
        } else {
			printf("Invalid operation.\n");
        }
     }
     
     if (root) {
		 freeTree(root);
     }
}
