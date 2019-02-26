#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mytypes.h"
#include "bitcoin_functions.h"

int main(int argc, char **argv){
  int i, bitCoinValue, senderHashtable1NumOfEntries, receiverHashtable1NumOfEntries, bucketSize;
  List *bitcoinList;
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
      senderHashtable1NumOfEntries = atoi(argv[i + 1]);
      i++;
    }
    else if (strcmp(argv[i], "-h2") == 0)
    {
      receiverHashtable1NumOfEntries = atoi(argv[i + 1]);
      i++;
    }
    else if (strcmp(argv[i], "-b") == 0)
    {
      bucketSize = atoi(argv[i + 1]);
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

      //Allocating a list that holds every unique bitCoinID
      //and points to the bitcoin's tree
      bitcoinList = (List*)malloc(sizeof(List));
      if (bitcoinList == NULL)
      {
        perror("Malloc failed");
        exit(0);
      }
      bitcoinList->nodes = NULL;

      //Parsing the balances file
      readBalances(bitCoinBalancesFile, bitcoinList);

      fclose(bitCoinBalancesFile);
    }
    else if (strcmp(argv[i], "-t") == 0)
    {

    }
    else
    {
      printf("Only use the following flags: -a, -t, -v, -h1, -h2 or -b\n");
      exit(2);
    }
  }

  return 0;
}
