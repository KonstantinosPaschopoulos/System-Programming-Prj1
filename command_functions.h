#include "mytypes.h"

int get_command(char *);
int requestTransaction(char *, wallet *, table *, table *);
void requestTransactions(char *, wallet *, table *, table *);
void requestTransactionsFile(char *, wallet *, table *, table *);
void findEarnings(char *, table *);
char *uniqueID(char *);
void findPayments(char *, table *);
int checkFind(char *, char *, int *, int *, long int *, long int *);
int checkInput(char *);
void walletStatus(wallet *, char *);
void bitCoinStatus(List *, int);
int countTransactions(tree_node *);
void traceCoin(List *, int);
void printTransactions(char *, tree_node *, int);
void exitProgram(List *, wallet *, table *, table *);
void freeTree(tree_node *);
