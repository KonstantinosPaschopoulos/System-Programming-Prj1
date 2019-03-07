#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mytypes.h"
#include "bitcoin_functions.h"
#include "command_functions.h"

int main(int argc, char **argv){
  int i, bitCoinValue, senderHashtableNumOfEntries, receiverHashtableNumOfEntries, bucketSize, flag;
  int bitcoinID;
  char user_input[MAX_INPUT], command[50], walletID[50];
  List *bitcoinList;
  wallet *walletList;
  FILE *bitCoinBalancesFile = NULL;
  FILE *transactionsFile = NULL;

  //Checking the user input
  if (argc != 13)
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
      if (bucketSize < (int)(sizeof(bucket) + sizeof(bucket_cell)))
      {
        printf("bucketSize should be more than %lu bytes\n", (sizeof(bucket) + sizeof(bucket_cell)));
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

  //After the balances have been read it's time to parse the transactions file
  table *senderHashtable = hash_init(senderHashtableNumOfEntries);
  table *receiverHashtable = hash_init(receiverHashtableNumOfEntries);

  //Parsing the initial transactions
  readTransactions(transactionsFile, walletList, senderHashtable, receiverHashtable, bucketSize);

  //After all the data structures are set, the program is ready to get input from the user
  flag = 1;
  while (flag != 0)
  {
    printf("\n");

    fgets(user_input, MAX_INPUT, stdin);
    if ((strlen(user_input) > 0) && (user_input[strlen(user_input) - 1] == '\n'))
    {
      user_input[strlen(user_input) - 1] = '\0';
    }

    //Using a switch statement to call the right function
    switch (get_command(user_input))
    {
      case 1:
        break;
      case 2:
        break;
      case 3:
        break;
      case 4:
        //findEarnings
        findEarnings(user_input, receiverHashtable);
        break;
      case 5:
        //findPayments
        findPayments(user_input, senderHashtable);
        break;
      case 6:
        //walletStatus
        // TODO better checks for numbers of strings
        sscanf(user_input, "%s %s", command, walletID);
        walletStatus(walletList, walletID);
        break;
      case 7:
        //bitCoinStatus
        sscanf(user_input, "%s %d", command, &bitcoinID);
        bitCoinStatus(bitcoinList, bitcoinID);
        break;
      case 8:
        //TraceCoin
        sscanf(user_input, "%s %d", command, &bitcoinID);
        traceCoin(bitcoinList, bitcoinID);
        break;
      case 9:
        //Exit
        exitProgram(bitcoinList, walletList, senderHashtable, receiverHashtable);
        flag = 0;
        break;
      default:
        printf("Invalid command\n");
        break;
    }
  }

  return 0;
}
