#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "tree.h"

/* sets bit at given index to value */
uint8_t set(uint8_t num, int index, int value) {
    /* sets index to set as 1 */
    /* shifts bit to correct index */
    uint8_t to_set = 0x80 >> index;
    if (value == 0) {
        num &= ~to_set;
    }
    if (value == 1) {
        num |= to_set;
    }
    return num;
}

int main(int argc, char* argv[]) {
    /* list[] stores frequency of each char */
    int list[256] = {0};
    /* code is used when creating the huffcode */
    char* code = (char*)malloc(sizeof(char) * 256);
    /* cur_byte is used as a temp variable to store huffcode when encoding */ 
    char* cur_byte = NULL;
    /* codes stores huffcodes for all characters */
    char* codes[256];
    /* i and j used in loops, cur_index used when encoding */
    int i, j, cur_index;
    /* size stores the number of unique chars */
    uint8_t size = 0;
    /* cur temporarily holds the current char when reading infile */
    char cur;
    /* tree stores the full huffman tree */
    tnode* tree;
    /* variables for input file and output file */
    int infile, outfile;
    /* cur_num stores the encoding to write into output file */ 
    uint8_t cur_num = 0;
    /* ch stores the char for writing header */
    uint8_t ch;
    /* freq stores the frequency for writing header */
    uint32_t freq;

    /* prints error message if invalid number of arguments */
    if (argc < 2 || argc > 3) {
        printf("Usage: hencode infile [ outfile ]\n");
        return 0;
    }
    infile = open(argv[1], O_RDONLY); 
    if (infile < 0)  {
        close(infile);
        return 0;
    }
    if (argc == 3) {
        outfile = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0666);
    } 
    if (outfile < 0) {
        close(infile);
        close(outfile);
        return 0;
    }
    /* creates frequency list */
    while (read(infile, &cur, 1) > 0) {
        list[(int)cur] += 1;
        cur = 0;
    }
    
    /* increments size when unique char is found (freq > 0) */
    for (i = 0; i < 256; i++) {
        if (list[i] > 0) {
           size++; 
        }
    }

    /* if no unique chars, close */
    if (size == 0) {
        close(outfile);
        close(infile);
        return 0;
    }
    
    create(list);
    tree = build_tree(size);
    huff_encode(tree, code, 0, codes);
    /* write header */
    size--;
    write(outfile, &size, 1);
    for (i = 0; i < 256; i++) {
        if (list[i] > 0) {
            ch = i;
            freq = htonl(list[i]);
            write(outfile, &ch, 1);
            write(outfile, &freq, 4);
        }
    }

    /* set/reset values to use in while loop */
    i = 0;
    j = 0;
    cur_index = 0;
    lseek(infile, 0, SEEK_SET);

    /* while char exists */
    while (read(infile, &j, 1) > 0) {
        /* cur_byte stores the code from array of char pointers */
        cur_byte = codes[j];
        /* if not null//if there is a code for the given char/index */
        while (cur_byte[i] != '\0') {
            if (cur_byte[i] == '0') {
                /* set current index to 0 */
                cur_num = set(cur_num, cur_index, 0);
            }
            if (cur_byte[i] == '1') {
                /* set current index to 1 */
                cur_num = set(cur_num, cur_index, 1);
            }
            cur_index++;
            if (cur_index == 8) {
                /* once 8 bits are reached, write to file */
                write(outfile, &cur_num, sizeof(uint8_t));
                cur_index = 0;
                cur_num = 0;
            }
            i++;
        }
        i = 0;
        j = 0;
    }
    /* write any remaining */
    if (cur_index > 0) {
        write(outfile, &cur_num, sizeof(uint8_t));
    }
    close(infile);
    close(outfile);
    free(code);
    free_tree(tree);
    return 0;
}
