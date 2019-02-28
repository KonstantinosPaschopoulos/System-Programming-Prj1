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

bucket** hash_init(int num_entries){
  int i;

  bucket **hash_table = malloc(num_entries * sizeof(bucket*));
  if (hash_table == NULL)
  {
    perror("Malloc failed");
    exit(0);
  }

  for (i = 0; i < num_entries; i++)
  {
    hash_table[i] = NULL;
  }

  return hash_table;
}

void readTransactions(FILE *transactionsFile, wallet *walletList, bucket **senderHashtable, bucket **receiverHashtable){
  char whole_line[250];
  int transactionID, value;
  char senderWalletID[50], receiverWalletID[50];

  //Using fgets to read from the input until I reach the end of the file
  while (fgets(whole_line, 250, transactionsFile))
  {
    if ((strlen(whole_line) > 0) && (whole_line[strlen(whole_line) - 1] == '\n'))
    {
      whole_line[strlen(whole_line) - 1] = '\0';
    }


  }
}

int hash_function(char *id, int num_entries){
  int i, value = 0;

  //The hash function works by adding the ASCII values of each character
  for (i = 0; i < strlen(id); i++)
  {
    value += (int)id[i];
  }

  return (value % num_entries);
}

int bucket_hash(char *id, int bucketSize){
  int i, value = 0;

  for (i = 0; i < strlen(id); i++)
  {
    value += (int)id[i];
  }

  return (value % ((bucketSize - sizeof(bucket)) / sizeof(bucket_cell)));
}
