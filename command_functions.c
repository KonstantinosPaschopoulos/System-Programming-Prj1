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

void findEarnings(char *user_input, table *hash_table){
  char walletID[50];
  int minutes, hours, day, month, year, pos, i, comm;
  transaction *trans;
  int sum;
  bucket *buc;

  comm = checkFind(user_input, walletID, &minutes, &hours, &day, &month, &year);

  if (comm == -1)
  {
    printf("Invalid command\n");
    return;
  }

  //Using the hash function to go to the correct bucket
  pos = hash_function(walletID, hash_table->size);
  buc = hash_table->h_table[pos];

  //Iterate through the bucket and the overflow buckets, if they exist
  while (buc != NULL)
  {
    for (i = 0; i < buc->size; i++)
    {
      if (buc->entries[i].empty == 0)
      {
        //Finding the correct entry
        if (strcmp(buc->entries[i].walletID, walletID) == 0)
        {
          //Calculate how much money have been transfered to the wallet in total
          trans = buc->entries[i].transactions;
          sum = 0;
          while (trans != NULL)
          {
            //Depending on the syntax of the command, different stuff will have to be done
            if (comm == 0)
            {
              sum += trans->tree->info.value;
            }

            trans = trans->next;
          }
          printf("The wallet %s has received %d$\n", walletID, sum);


          //Now print all the transactions that are needed

          return;
        }
      }
      else
      {
        //If an empty cell was found, before finding the walletID, it means it doesn't exist
        printf("The wallet %s either doesn't exist or it hasn't received any money\n", walletID);
        return;
      }
    }

    buc = buc->next;
  }
}

void findPayments(char *user_input, table *hash_table){
  char walletID[50];
  int minutes, hours, day, month, year, pos, i, comm;
  transaction *trans;
  int sum;
  bucket *buc;

  comm = checkFind(user_input, walletID, &minutes, &hours, &day, &month, &year);

  if (comm == -1)
  {
    printf("Invalid command\n");
    return;
  }

  //Using the hash function to go to the correct bucket
  pos = hash_function(walletID, hash_table->size);
  buc = hash_table->h_table[pos];

  //Iterate through the bucket and the overflow buckets, if they exist
  while (buc != NULL)
  {
    for (i = 0; i < buc->size; i++)
    {
      if (buc->entries[i].empty == 0)
      {
        //Finding the correct entry
        if (strcmp(buc->entries[i].walletID, walletID) == 0)
        {
          //Calculate how much money have been transfered to the wallet in total
          trans = buc->entries[i].transactions;
          sum = 0;
          while (trans != NULL)
          {
            //Depending on the syntax of the command, different stuff will have to be done
            if (comm == 0)
            {
              sum += trans->tree->info.value;
            }

            trans = trans->next;
          }
          printf("The wallet %s has paid %d$\n", walletID, sum);


          //Now print all the transactions that are needed

          return;
        }
      }
    }

    buc = buc->next;
  }

  printf("The wallet %s either doesn't exist or it hasn't made any payments\n", walletID);
}

int checkFind(char *input, char *walletID, int *minutes, int *hours, int *day, int*month, int *year){
  char input_copy[MAX_INPUT];
  char array[12][55];
  char* token;
  char delimiters[] = " -:";
  int i, flag = 0;

  strcpy(input_copy, input);

  for (i = 0; i < (int)strlen(input_copy); i++)
  {
    if ((input_copy[i] == ':') || (input_copy[i] == '-'))
    {
      flag++;
    }
  }
  if ((flag % 2) != 0)
  {
    return -1;
  }

  for (token = strtok(input_copy, delimiters), i = 0; token; token = strtok(NULL, delimiters), i++)
  {
    if (i < 12)
    {
      strcpy(array[i], token);
    }
    else
    {
      return -1;
    }
  }

  strcpy(walletID, array[1]);

  if (i == 2)
  {
    //Gave only the command
    return 0;
  }
  else if (i == 6)
  {
    //Gave times

    return 1;
  }
  else if (i == 8)
  {
    //Gave dates
    return 2;
  }
  else if (i == 12)
  {
    //Gave everything
    return 3;
  }


  return -1;
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

void traceCoin(List *bitcoinList, int bitcoinID){
  bitcoin_node *curr;
  int flag = 0;

  curr = bitcoinList->nodes;
  while (curr != NULL)
  {
    //Finding the correct bitCoin
    if (curr->bitCoinID == bitcoinID)
    {
      //Using a recursive function to print the history of this bitcoin
      printTransactions(curr->tree->walletID, curr->tree, 0);

      flag = 1;
      break;
    }

    curr = curr->next;
  }

  if (flag == 0)
  {
    printf("The bitCoin %d doesn't exist\n", bitcoinID);
  }
}

void printTransactions(char *from, tree_node *root, int yes){
  if (root == NULL)
  {
    return;
  }
  else
  {
    if (yes == 1)
    {
      printf("%s %s %s %d %d-%d-%d %d:%d\n", root->info.transactionID, from, root->walletID,
                root->info.value, root->info.day, root->info.month, root->info.year,
              root->info.hours, root->info.minutes);
    }

    //Go to the children
    printTransactions(root->walletID, root->right, 0);
    printTransactions(root->walletID, root->left, 1);
  }
}

void exitProgram(List *bitcoinList, wallet *walletList, table *senderHashtable, table *receiverHashtable){

}
