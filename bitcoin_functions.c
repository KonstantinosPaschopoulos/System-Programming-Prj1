#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mytypes.h"
#include "bitcoin_functions.h"

void readBalances(FILE *bitCoinBalancesFile, List *bitcoinList, wallet *walletList, int bitCoinValue){
  int name;
  char whole_line[250];
  char walletID[50];
  char * pch;
  wallet_node *curr_wallet = walletList->nodes;
  wallet_node *new_wallet;

  //Using fgets to read from the input until I reach the end of the file
  while (fgets(whole_line, 250, bitCoinBalancesFile))
  {
    if ((strlen(whole_line) > 0) && (whole_line[strlen(whole_line) - 1] == '\n'))
    {
      whole_line[strlen(whole_line) - 1] = '\0';
    }

    name = 0;

    //Using strtok to extract the userID and every
    //bitCoinID that the user owns
    pch = strtok(whole_line, " ");
    while (pch != NULL)
    {
      if (name == 0)
      {
        //Storing the userID
        name = 1;
        strcpy(walletID, pch);

        //Every existing user must have a wallet
        //even if they don't own any bitcoins
        while (curr_wallet != NULL)
        {
          if (strcmp(curr_wallet->walletID, walletID) == 0)
          {
            printf("Two users have the same name, exiting\n");
            exit(3);
          }

          curr_wallet = curr_wallet->next;
        }

        //Creating a new wallet
        new_wallet = (wallet_node*)malloc(sizeof(wallet_node));
        if (new_wallet == NULL)
        {
          perror("Malloc failed");
          exit(0);
        }
        strcpy(new_wallet->walletID, walletID);
        new_wallet->bitcoins = NULL;
        new_wallet->next = NULL;

        if (walletList->nodes == NULL)
        {
          //The list is empty
          walletList->nodes = new_wallet;
        }
        else
        {
          //Placing the new wallet at the end of the list
          curr_wallet = walletList->nodes;
          while ((curr_wallet->next) != NULL)
          {
            curr_wallet = curr_wallet->next;
          }

          curr_wallet->next = new_wallet;
        }
      }
      else
      {
        //Storing the bitcoin
        enterBitcoin(atoi(pch), bitcoinList, bitCoinValue, walletList, walletID);
      }

      pch = strtok(NULL, " ");
    }
  }

  fclose(bitCoinBalancesFile);
}

void enterBitcoin(int id, List *bitcoinList, int bitCoinValue, wallet *walletList, char *walletID){
  bitcoin_node *curr = bitcoinList->nodes;
  wallet_node *curr_wall = walletList->nodes;
  leaf *curr_leaf;

  //Checking if the bitCoinId already exists
  while (curr != NULL)
  {
    if (curr->bitCoinID == id)
    {
      printf("Two users own the same bitcoin, exiting.\n");
      exit(3);
    }

    curr = curr->next;
  }

  bitcoin_node *newBTC = (bitcoin_node*)malloc(sizeof(bitcoin_node));
  if (newBTC == NULL)
  {
    perror("Malloc failed");
    exit(0);
  }

  //Creating the tree
  tree_node *root = (tree_node*)malloc(sizeof(tree_node));
  if (root == NULL)
  {
    perror("Malloc failed");
    exit(0);
  }
  root->value = bitCoinValue;
  strcpy(root->walletID, walletID);

  newBTC->bitCoinID = id;
  newBTC->next = NULL;

  //Adding a pointer to the tree
  newBTC->tree = root;

  //After I made sure the bitCoin is unique
  //I enter it in the bitcoin list
  if (bitcoinList->nodes == NULL)
  {
    //The list is empty
    bitcoinList->nodes = newBTC;
  }
  else
  {
    //Placing the new bitCoin at the end of the list
    curr = bitcoinList->nodes;
    while ((curr->next) != NULL)
    {
      curr = curr->next;
    }

    curr->next = newBTC;
  }

  //Add the bitcoin to the correct owner's wallet
  while (curr_wall != NULL)
  {
    if (strcmp(curr_wall->walletID, walletID) == 0)
    {
      leaf *new_leaf = (leaf*)malloc(sizeof(leaf));
      if (new_leaf == NULL)
      {
        perror("Malloc failed");
        exit(0);
      }
      new_leaf->bitcoin = newBTC;
      new_leaf->balance = root;
      new_leaf->next = NULL;

      if (curr_wall->bitcoins == NULL)
      {
        curr_wall->bitcoins = new_leaf;
      }
      else
      {
        curr_leaf = curr_wall->bitcoins;
        while ((curr_leaf->next) != NULL)
        {
          curr_leaf = curr_leaf->next;
        }

        curr_leaf->next = new_leaf;
      }
    }

    curr_wall = curr_wall->next;
  }
}

table* hash_init(int num_entries){
  int i;

  table *hash_table = (table*)malloc(sizeof(table));
  if (hash_table == NULL)
  {
    perror("Malloc failed");
    exit(0);
  }

  hash_table->size = num_entries;

  hash_table->h_table = (bucket**)malloc(num_entries * sizeof(bucket*));
  if (hash_table->h_table == NULL)
  {
    perror("Malloc failed");
    exit(0);
  }

  for (i = 0; i < num_entries; i++)
  {
    hash_table->h_table[i] = NULL;
  }

  return hash_table;
}

void readTransactions(FILE *transactionsFile, wallet *walletList, table *senderHashtable, table *receiverHashtable, int bucketSize){
  int value, day, month, year, hours, minutes, i;
  char whole_line[250], senderWalletID[50], receiverWalletID[50], transactionID[50];
  char array[9][55];
  char* token;
  char delimiters[] = " -:";

  //Using fgets to read from the input until I reach the end of the file
  while (fgets(whole_line, 250, transactionsFile))
  {
    if ((strlen(whole_line) > 0) && (whole_line[strlen(whole_line) - 1] == '\n'))
    {
      whole_line[strlen(whole_line) - 1] = '\0';
    }

    //Using strtok to tokenize every line before the final extraction
    for (token = strtok(whole_line, delimiters), i = 0; token; token = strtok(NULL, delimiters), i++)
    {
      strcpy(array[i], token);
    }

    //Assigning the correct value to each variable
    strcpy(transactionID, array[0]);
    strcpy(senderWalletID, array[1]);
    strcpy(receiverWalletID, array[2]);
    value = atoi(array[3]);
    day = atoi(array[4]);
    month = atoi(array[5]);
    year = atoi(array[6]);
    hours = atoi(array[7]);
    minutes = atoi(array[8]);

    //Checking if the transaction is valid
    if (checkTransaction(walletList, senderWalletID, receiverWalletID, value) == 0)
    {
      printf("Invalid transaction.\n");
      continue;
    }

    //delete this
    minutes = minutes * hours * year * month * day * value;

    enterTransaction(senderWalletID, senderHashtable, receiverWalletID, receiverHashtable, walletList, bucketSize, value);
  }
}

int checkTransaction(wallet *walletList, char *senderWalletID, char *receiverWalletID, int value){
  wallet_node *curr = walletList->nodes;
  int sum = 0, flag = 0;
  leaf *coin;

  if (strcmp(senderWalletID, receiverWalletID) == 0)
  {
    printf("The senderWalletID and receiverWalletID can't be the same. ");
    return 0;
  }

  while (curr != NULL)
  {
    if (strcmp(receiverWalletID, curr->walletID) == 0)
    {
      flag = 1;
      break;
    }

    curr = curr->next;
  }

  if (flag == 0)
  {
    printf("The receiverWalletID doesn't seem to exist. ");
    return 0;
  }

  curr = walletList->nodes;
  while (curr != NULL)
  {
    if (strcmp(senderWalletID, curr->walletID) == 0)
    {
      //Found the senders wallet, now I need to see if
      //it has enough money to transfer
      coin = curr->bitcoins;
      while (coin != NULL)
      {
        sum += coin->balance->value;

        coin = coin->next;
      }

      if (sum >= value)
      {
        return 1;
      }
      else
      {
        printf("The senderWalletID doesn't have enough money in it's wallet. ");
        return 0;
      }
    }

    curr = curr->next;
  }

  printf("The senderWalletID doesn't seem to exist. ");
  return 0;
}

int hash_function(char *id, int range){
  int i, value = 0;

  //The hash function works by adding the ASCII values of each character
  for (i = 0; i < strlen(id); i++)
  {
    value += (int)id[i];
  }

  return (value % range);
}

void enterTransaction(char *senderWalletID, table *senderHashtable, char *receiverWalletID, table *receiverHashtable, wallet *walletList, int bucketSize, int value){
  int buc, i, flag;
  bucket *b, *prev;
  transaction *trans;

  //Updating the senderHashtable first
  buc = hash_function(senderWalletID, senderHashtable->size);

  if (senderHashtable->h_table[buc] == NULL)
  {
    //The bucket needs to be allocated first
    b = (bucket*)malloc(sizeof(bucket));
    if (b == NULL)
    {
      perror("Malloc failed");
      exit(0);
    }

    b->entries = (bucket_cell*)malloc(((bucketSize - sizeof(bucket)) / sizeof(bucket_cell)) * sizeof(bucket_cell));
    if (b->entries == NULL)
    {
      perror("Malloc failed");
      exit(0);
    }

    b->size = (bucketSize - sizeof(bucket)) / sizeof(bucket_cell);  //Number of entries in every bucket

    for (i = 0; i < b->size; i++)
    {
      b->entries[i].empty = 1;
    }

    //Entering the walletID in the first cell
    b->entries[0].empty = 0;
    strcpy(b->entries[0].walletID, senderWalletID);

    //Adding the transaction info
    trans = (transaction*)malloc(sizeof(transaction));
    if (trans == NULL)
    {
      perror("Malloc failed");
      exit(0);
    }

    //-- TODO Edit tree
    //Search through the senders wallet to find enough balance

    //Go to the correct node and split it

    //Add the node to the transaction

    //Edit the wallet

    //Repeat if it needs more than one coins

    //Finaly updating the hash table
    senderHashtable->h_table[buc] = b;
  }
  else
  {
    //The bucket exists and I need to find an empty space
    b = senderHashtable->h_table[buc];

    //Iterate through the bucket(s) to find the walletID
    flag = 0;
    while (b != NULL)
    {
      for (i = 0; i < b->size; i++)
      {
        if (b->entries[i].empty == 0)
        {
          if (strcmp(b->entries[0].walletID, senderWalletID) == 0)
          {
            //Adding the transaction to the correct walletID

            flag = 1;
            break;
          }
        }
        else
        {
          //Found an empty cell to put the walletID and its transactions
          b->entries[i].empty = 0;
          strcpy(b->entries[i].walletID, senderWalletID);

          //Adding the transaction info

          flag = 1;
          break;
        }
      }

      prev = b;
      b = b->next;
    }

    if (flag == 0)
    {
      //I need to add an overflow bucket

    }
  }
}
