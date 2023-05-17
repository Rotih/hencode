typedef struct tnode {
    char letter;
    unsigned int frequency;
    struct tnode* left;
    struct tnode* right;
    struct tnode* next;
} tnode;

extern tnode* first;

int comes_before(tnode* a, tnode* b);

tnode* combine_nodes(tnode* a, tnode* b);

void create(int list[]);

void sort_swap(tnode* a, tnode* b);

void selection_sort();

void insert(tnode* combined);

tnode* build_tree(int size);

void huff_encode(tnode* tree, char* code, int i, char* codes[]);

void free_tree(tnode* tree);