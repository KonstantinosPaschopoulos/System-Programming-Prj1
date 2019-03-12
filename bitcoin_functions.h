#include <stdio.h>
#include "mytypes.h"

void readBalances(FILE *, List *, wallet *, int);
void enterBitcoin(int, List *, int, wallet *, char*);
table* hash_init(int, int);
void readTransactions(FILE *, wallet *, table *, table *);
int checkTransactionID(char *, table *);
int checkTransaction(wallet *, char *, char *, int);
int hash_function(char *, int);
void enterTransaction(char *, table *, char *, table *, wallet *, transaction_info);
void addBitcointoWallet(wallet *, char *, bitcoin_node *, tree_node *);
long int getUnixTime(int, int, int, int, int);
