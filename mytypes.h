#ifndef MYTYPES_H
#define MYTYPES_H

typedef struct bitcoin_node{
  int bitCoinID;
  //pointer to bitcoin tree
  struct bitcoin_node *next;
} bitcoin_node;

typedef struct Wallet_node{
  char userID[50];
  //pointer to bitcoins in bitcoin list
  struct Wallet_node *next;
} Wallet_node;

typedef struct List{
  bitcoin_node *nodes;
} List;

#endif
