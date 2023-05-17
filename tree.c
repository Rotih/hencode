#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"

tnode* first = NULL;

/* comparator for nodes when making linked list */
int comes_before(tnode* a, tnode* b) {
    if (a->frequency < b->frequency) {
        return 1;
    }
    if (a->frequency == b->frequency) {
        if (a->letter < b->letter) {
            return 1;
        }
    }
    return 0;
}

/* combines two nodes and makes them left and right leaves */
tnode* combine_nodes(tnode* a, tnode* b) {
    tnode* new_node = (tnode*)malloc(sizeof(tnode));
    new_node->frequency = a->frequency + b->frequency;
    new_node->letter = 'p';
    new_node->left = a;
    new_node->right = b;
    return new_node;
}

/* creates linked list based on histogram */
void create(int list[]) {
    int i;
    tnode* cur_node = NULL;
    tnode* new_node = NULL;
    for (i = 0; i < 256; i++) {
        if (list[i] > 0) {
            if (first == NULL) {
                new_node = (tnode*)malloc(sizeof(tnode));
                first = new_node;
                new_node->frequency = list[i];
                new_node->letter = (char)i;
                new_node->left = NULL;
                new_node->right = NULL;
                new_node->next = NULL;
            } else {
                cur_node = first;
                /* very long conditional sorry */
                while (((cur_node->next != NULL) 
                && (list[i] > cur_node->frequency) 
                    && (list[i] > cur_node->next->frequency)) 
                    || ((cur_node->next != NULL) 
                    && (list[i] == cur_node->next->frequency)
                    && (i > (int)(cur_node->next->letter)))) {
                        cur_node = cur_node->next;
                }
                new_node = (tnode*)malloc(sizeof(tnode));
                new_node->frequency = list[i];
                new_node->letter = (char)i;
                new_node->left = NULL;
                new_node->right = NULL;
                new_node->next = NULL;
                if (cur_node == first) {
                    if (list[i] >= first->frequency) {
                        new_node->next = cur_node->next;
                        cur_node->next = new_node;
                    } else {
                        new_node->next = first;
                        first = new_node;
                    }
                } else {
                    new_node->next = cur_node->next;
                    cur_node->next = new_node;
                }
            }
        }
    }
}

/* swaps contents of two tree nodes */
/* note: unused since selection sort isn't used */
void sort_swap(tnode* a, tnode* b) {
    unsigned int temp_f = a->frequency;
    char temp_c = a->letter;
    a->frequency = b->frequency;
    a->letter = b->letter;
    b->frequency = temp_f;
    b->letter = temp_c;
}

/* sorts the linked list */
/* NOTE: unused since create now actively sorts nodes */
void selection_sort() {
    tnode* start = first;
    tnode* current;
    tnode* smallest;
    while (start->next != NULL) {
        smallest = start;
        current = start->next;
        while (current != NULL) {
            if(comes_before(smallest, current) == 0) {
                smallest = current;
            }
            current = current->next;
            sort_swap(start, smallest);
            start = start->next;
        }
    }
}

/* inserts the new node into linked list */
void insert(tnode* combined) {
    tnode* cur = first->next->next;
    tnode* new = combined;
    int i = 0;
    if (combined->frequency <= cur->frequency) {
        new->next = cur;
        first = new;
    } else {
        while (cur->next != NULL) {
            if (((combined->frequency) <= (cur->next->frequency))
             && ((combined->frequency) > (cur->frequency))) {
                new->next = cur->next;
                cur->next = new;
                i = 1;
                break;
            }
            cur = cur->next;
        }
        if (i != 1) {
            if (combined->frequency > cur->frequency) {
                cur->next = new;
            } 
        }
        first = first->next->next;
    }
}

/* builds huff tree with use of helper methods */
tnode* build_tree(int size) {
    tnode* node1;
    tnode* node2;
    tnode* combined;
    while (size > 2) {
        node1 = first;
        node2 = first->next;
        combined = combine_nodes(node1, node2);
        insert(combined);
        size--;
    }
    if (size == 2) {
        node1 = first;
        node2 = first->next;
        return combine_nodes(node1, node2);
    } else {
        return first;
    }
}

/* creates the huff codes based on the tree and inserts them into a list */
void huff_encode(tnode* tree, char* code, int i, char* codes[]) {
    if (tree->left != NULL) {
        i++;
        code[i-1] = '0';
        huff_encode(tree->left, code, i, codes);
    }
    if (tree->right != NULL) {
        code[i-1] = '1';
        huff_encode(tree->right, code, i, codes);
    }
    if (tree->right == NULL && tree->left == NULL) {
        if (strlen(code) > i) {
            code[i] = '\0';
        }
        codes[(unsigned int)(tree->letter)] =
         (char*)malloc(sizeof(char) * strlen(code));
        strcpy(codes[(unsigned int)(tree->letter)], code);
    }
}

void free_tree(tnode* tree) {
    if (tree == NULL) {
        return;
    }
    free_tree(tree->left);
    free_tree(tree->right);
    free(tree);
}
