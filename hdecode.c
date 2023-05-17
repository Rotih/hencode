#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "tree.h"

int main(int argc, char* argv[]) {
    int list[256] = {0};
    tnode* tree;
    tnode* temp;
    /* numchars stores total unique chars, this is needed for decode loop */
    int infile, outfile, i = 0, numchars = 0;
    uint8_t* size = (uint8_t*)malloc(sizeof(uint8_t));
    /* size_i stores the integer value of the pointer size */
    int size_i;
    char ch;
    uint32_t freq;
    /* after obtaining frequency, i move it to freqcopy */
    /* since freqcopy is uint8_t, it remove null bits */
    /* i think this works, but there's probably an easier way */
    uint8_t freqcopy;
    uint8_t c;
    int index = 0;

    if (argc < 2 || argc > 3) {
        printf("Usage: hdecode [ ( infile | - ) [ outfile ] ]\n");
        return 0;
    }
    if (argc == 3) {
        infile = open(argv[1], O_RDONLY);
        if (infile < 0) {
            close(infile);
            return 0;
        }
        outfile = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0666);
        if (outfile < 0) {
            close(infile);
            close(outfile);
            return 0;
        }
    }
    /* lseek(infile, 0, SEEK_SET); */
    read(infile, size, 1);
    size_i = *size + 1;
    if (size_i < 1) {
        if (argc > 2) {
            close(infile);
            close(outfile);
        } else {
            if (argc == 2) {
                close(infile);
                close(outfile);
            }
        }
        return 0;
    }

    /* reads all chars and frequency and makes table */
    while (i < size_i) {
        read(infile, &ch, sizeof(uint8_t));
        read(infile, &freq, sizeof(uint32_t));
        /* flips so null bits are at the end */
        freq = ntohl(freq);
        freqcopy = (uint8_t)freq;
        list[(int)ch] = freqcopy;
        i++;
        numchars += freq;
    }

    /* create linked list -> to build tree -> to build huffcodes */
    create(list);
    tree = build_tree(size_i);

    /* initialize vars to use in while loop */
    index = 0;
    read(infile, &c, sizeof(c));
    temp = tree;

    /* DECODE LOOP */
    while (numchars > 0) {
        /* if leaf node, write letter */
        if (temp->left == NULL && temp->right == NULL) {
            write(outfile, &(temp->letter), sizeof(uint8_t));
            temp = tree; 
            numchars--;
        } else {
            /* checks next possible direction of tree using encoding */
            if ((c & (0x80 >> index)) == 0) {
                temp = temp->left;
            } else {
                temp = temp->right;
            }
            index++;
            if (index == 8) {
                read(infile, &c, sizeof(c));
                index = 0;
            }
        }
    }
    close(infile);
    close(outfile);
    free(size);
    free_tree(tree);
    return 0;
}