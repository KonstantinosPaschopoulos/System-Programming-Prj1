#include "mytypes.h"

int get_command(char *);
void findEarnings(char *, table *);
void findPayments(char *, table *);
int checkFind(char *, char *, int *, int*, int *, int *, int *, int *, int*, int *);
int checkInput(char *);
void walletStatus(wallet *, char *);
void bitCoinStatus(List *, int);
int countTransactions(tree_node *);
void traceCoin(List *, int);
void printTransactions(char *, tree_node *, int);
void exitProgram(List *, wallet *, table *, table *);
