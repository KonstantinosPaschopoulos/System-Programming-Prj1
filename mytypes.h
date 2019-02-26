#ifndef MYTYPES_H
#define MYTYPES_H

typedef struct Node{
  int bitCoinID;
  //pointer to bitcoin tree
  struct Node *next;
} Node;

typedef struct List{
  Node *nodes;
} List;

#endif
