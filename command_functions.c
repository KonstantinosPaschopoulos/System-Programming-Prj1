#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mytypes.h"
#include "bitcoin_functions.h"
#include "command_functions.h"

int get_command(char *input){
  char copy_input[MAX_INPUT], semicolon_copy[MAX_INPUT];  //Using copies to be safe
  char * pch;
  char *semicolon_position;

  strcpy(copy_input, input);
  strcpy(semicolon_copy, input);

  //Get only the first word in the string using strtok
  pch = strtok(copy_input, " ");
  if (pch != NULL)
  {
    if (strcmp(pch, "/requestTransaction") == 0)
    {
      return 1;
    }
    else if (strcmp(pch, "/requestTransactions") == 0)
    {
      //Checking to see there is a semicolon in the rest of command
      semicolon_position = strchr(semicolon_copy, ';');
      if (semicolon_position == NULL)
      {
        //This is the second case of the /requestTransactions command
        return 3;
      }
      else
      {
        //This is the first case of the /requestTransactions command
        return 2;
      }
    }
    else if (strcmp(pch, "/findEarnings") == 0)
    {
      return 4;
    }
    else if (strcmp(pch, "/findPayments") == 0)
    {
      return 5;
    }
    else if (strcmp(pch, "/walletStatus") == 0)
    {
      return 6;
    }
    else if (strcmp(pch, "/bitCoinStatus") == 0)
    {
      return 7;
    }
    else if (strcmp(pch, "/traceCoin") == 0)
    {
      return 8;
    }
    else if (strcmp(pch, "/exit") == 0)
    {
      return 9;
    }
    else
    {
      return 0;
    }
  }

  return 0;
}

void walletStatus(wallet *walletList, char *walletID){
  wallet_node *curr;
  leaf *coin;
  int sum = 0, flag = 0;

  curr = walletList->nodes;
  while (curr != NULL)
  {
    //Finding the correct wallet
    if (strcmp(walletID, curr->walletID) == 0)
    {
      //Adding up all the bitcoins
      coin = curr->bitcoins;
      while (coin != NULL)
      {
        sum += coin->balance->value;

        coin = coin->next;
      }

      flag = 1;
      break;
    }

    curr = curr->next;
  }

  if (flag == 0)
  {
    printf("The wallet %s doesn't exist\n", walletID);
  }
  else
  {
    printf("The wallet %s currently has %d$\n", walletID, sum);
  }
}

void bitCoinStatus(List *bitcoinList, int bitcoinID){
  bitcoin_node *curr;
  tree_node *node;
  int flag = 0, transactions = 0, unspent;

  curr = bitcoinList->nodes;
  while (curr != NULL)
  {
    //Finding the correct bitCoin
    if (curr->bitCoinID == bitcoinID)
    {
      //To find how much of it has stayed unspent I will follow only
      //the right child of every node until I reach the end
      node = curr->tree;
      while (node != NULL)
      {
        unspent = node->value;

        node = node->right;
      }

      //Using a recursive function I count only how many left nodes exist in the tree
      transactions = countTransactions(curr->tree);

      flag = 1;
      break;
    }

    curr = curr->next;
  }

  if (flag == 0)
  {
    printf("The bitCoin %d doesn't exist\n", bitcoinID);
  }
  else
  {
    printf("%d %d %d\n", bitcoinID, transactions, unspent);
  }
}

int countTransactions(tree_node *node){
  int sum_left = 0, sum_right = 0;

  //In order to count how many transactions have happened
  //I count how many left children exist in the tree
  if (node == NULL)
  {
    //Return 0 when I reach an end
    return 0;
  }
  else
  {
    if (node->left != NULL)
    {
      sum_left = countTransactions(node->left);

      //Add 1 to the sum, for every left child
      sum_left++;
    }
    if (node->right != NULL)
    {
      sum_right = countTransactions(node->right);
    }

    return (sum_left + sum_right);
  }
}

void traceCoin(table *hashtable, int bitcoinID){
  /*
  int i, j;
  bucket *buc;
  transaction *trans, *bitcoins;


  //Iterate through the whole hashtable and find every transaction
  //that used the bitcoin print its information
  for (i = 0; i < hashtable->size; i++)
  {
    buc = hashtable->h_table[i];

    if (buc != NULL)
    {
      for (j = 0; j < buc->size; j++)
      {
        trans = buc->entries[j].transactions;
        while (trans != NULL)
        {
          //Search all the bitcoins that were used by this transaction
          bitcoins = trans;
          while (bitcoins != NULL)
          {
            if (bitcoins->tree->bitCoinID == bitcoinID)
            {

            }

            bitcoins = bitcoins->next_bitcoin;
          }

          // if (strcmp(transactionID, trans->info.transactionID) == 0)
          // {
          //   return 0;
          // }

          trans = trans->next;
        }
      }
    }
  }
  */
}

void exitProgram(List *bitcoinList, wallet *walletList, table *senderHashtable, table *receiverHashtable){

}
