#ifndef MYTYPES_H
#define MYTYPES_H

typedef struct bitcoin_node{
  int bitCoinID;
  struct tree_node *tree;  //A pointer to the bitcoin's tree
  struct bitcoin_node *next;
} bitcoin_node;

typedef struct List{
  bitcoin_node *nodes;
} List;

typedef struct wallet_node{
  char walletID[50];
  struct leaf *bitcoins; //A list of every bitcoin that the user owns
  struct wallet_node *next;
} wallet_node;

typedef struct leaf{
  struct tree_node *bitcoin;
  struct leaf *next;
} leaf;

typedef struct wallet{
    wallet_node *nodes;
} wallet;

typedef struct tree_node{
  char walletID[50];
  int value;
  struct tree_node *left;
  struct tree_node *right;
} tree_node;

#endif
