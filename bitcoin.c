#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mytypes.h"
#include "bitcoin_functions.h"

int main(int argc, char **argv){
  int i, bitCoinValue, senderHashtableNumOfEntries, receiverHashtableNumOfEntries, bucketSize;
  List *bitcoinList;
  wallet *walletList;
  FILE *bitCoinBalancesFile = NULL;
  FILE *transactionsFile = NULL;

  //Checking the user input
  if (argc > 13)
  {
    printf("Wrong arguments\n");
    exit(2);
  }

  for (i = 1; i < argc; i++)
  {
    if (strcmp(argv[i], "-v") == 0)
    {
      bitCoinValue = atoi(argv[i + 1]);
      i++;
    }
    else if (strcmp(argv[i], "-h1") == 0)
    {
      senderHashtableNumOfEntries = atoi(argv[i + 1]);
      if (senderHashtableNumOfEntries < 1)
      {
        printf("senderHashtableNumOfEntries should be a positive number\n");
        exit(2);
      }

      i++;
    }
    else if (strcmp(argv[i], "-h2") == 0)
    {
      receiverHashtableNumOfEntries = atoi(argv[i + 1]);
      if (receiverHashtableNumOfEntries < 1)
      {
        printf("receiverHashtableNumOfEntries should be a positive number\n");
        exit(2);
      }

      i++;
    }
    else if (strcmp(argv[i], "-b") == 0)
    {
      bucketSize = atoi(argv[i + 1]);
      if (bucketSize < sizeof(bucket))
      {
        printf("bucketSize should be more than %lu bytes\n", sizeof(bucket));
        exit(2);
      }

      i++;
    }
    else if (strcmp(argv[i], "-a") == 0)
    {
      //Opening the balances file
      bitCoinBalancesFile = fopen(argv[i + 1], "r");
      if (bitCoinBalancesFile == NULL)
      {
        perror("Could not open the balances file");
        exit(0);
      }
      i++;
    }
    else if (strcmp(argv[i], "-t") == 0)
    {
      //Opening the transactions file
      transactionsFile = fopen(argv[i + 1], "r");
      if (transactionsFile == NULL)
      {
        perror("Could not open the transactions file");
        exit(0);
      }
      i++;
    }
    else
    {
      printf("Only use the following flags: -a, -t, -v, -h1, -h2 or -b\n");
      exit(2);
    }
  }

  //Allocating a list that holds every unique bitCoinID
  //and points to the bitcoin's tree
  bitcoinList = (List*)malloc(sizeof(List));
  if (bitcoinList == NULL)
  {
    perror("Malloc failed");
    exit(0);
  }
  bitcoinList->nodes = NULL;

  //Also allocating a list to hold every wallet
  walletList = (wallet*)malloc(sizeof(wallet));
  if (walletList == NULL)
  {
    perror("Malloc failed");
    exit(0);
  }
  walletList->nodes = NULL;

  //Parsing the balances file
  readBalances(bitCoinBalancesFile, bitcoinList, walletList, bitCoinValue);

  //After the balances have been read
  //it's time to parse the transactions file
  table *senderHashtable = hash_init(senderHashtableNumOfEntries);
  table *receiverHashtable = hash_init(receiverHashtableNumOfEntries);

  readTransactions(transactionsFile, walletList, senderHashtable, receiverHashtable);

  /*
  wallet_node *test = walletList->nodes;
  leaf *test_b;
  while (test != NULL)
  {
    printf("%s", test->walletID);
    test_b = test->bitcoins;
    while (test_b != NULL)
    {
      printf(" %d(%d) ", test_b->bitcoin->bitCoinID, test_b->balance->value);
      test_b = test_b->next;
    }

    printf("\n");

    test = test->next;
  }
  */

  return 0;
}
