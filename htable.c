#include <stdio.h>
#include <stdlib.h>
#include "tree.h"

int main(int argc, char* argv[]) {
    int list[256] = {0};
    char* codes[256];
    int i, size = 0;
    char cur;
    tnode* tree;
    char* code = (char*)malloc(sizeof(char) * 256);
    FILE* nf;
    if (argc == 0 || argc == 1) {
        return 0;
    }
    nf = fopen(argv[1], "r"); 
    if (nf == NULL) {
        return 0;
    }
    while ((cur = fgetc(nf)) != EOF) {
        list[(int)cur] += 1;
    }
    for (i = 0; i < 256; i++) {
        if (list[i] > 0) {
           size++; 
        }
    }
    create(list);
    tree = build_tree(size);
    if(!tree){
        return 0;
    }
    huff_encode(tree, code, 0, codes);
    for (i = 0; i < 256; i++) {
        if (list[i] > 0) {
            printf("0x%02x: %s\n", i, codes[i]);
        }
    }
    fclose(nf);
    free_tree(tree);
    return 0;
}