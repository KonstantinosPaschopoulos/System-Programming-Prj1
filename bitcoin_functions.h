#include <stdio.h>
#include "mytypes.h"

void readBalances(FILE *, List *, wallet *, int);
void enterBitcoin(int, List *, int, wallet *, char*);
bucket** hash_init(int);
void readTransactions(FILE *, wallet *, bucket **, bucket **);
int hash_function(char *, int);
int bucket_hash(char *, int);
