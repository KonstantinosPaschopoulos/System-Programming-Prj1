#include <stdio.h>
#include "mytypes.h"

void readBalances(FILE *, List *, wallet *, int);
void enterBitcoin(int, List *, int, wallet *, char*);
table* hash_init(int);
void readTransactions(FILE *, wallet *, table *, table *, int);
int hash_function(char *, int);
void enterTransaction(char *, table *, char *, table *, wallet *, int);
