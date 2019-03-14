#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mytypes.h"
#include "bitcoin_functions.h"

int readBalances(FILE *bitCoinBalancesFile, List *bitcoinList, wallet *walletList, int bitCoinValue){
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
            printf("Two users have the same userID, exiting\n");
            return -1;
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
        if (enterBitcoin(atoi(pch), bitcoinList, bitCoinValue, walletList, walletID) == -1)
        {
          return -1;
        }
      }

      pch = strtok(NULL, " ");
    }
  }

  fclose(bitCoinBalancesFile);
  return 1;
}

int enterBitcoin(int id, List *bitcoinList, int bitCoinValue, wallet *walletList, char *walletID){
  bitcoin_node *curr = bitcoinList->nodes;
  wallet_node *curr_wall = walletList->nodes;
  leaf *curr_leaf;

  //Checking if the bitCoinId already exists
  while (curr != NULL)
  {
    if (curr->bitCoinID == id)
    {
      printf("Two users own the same bitcoin, exiting.\n");
      return -1;
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
  root->right = NULL;
  root->left = NULL;

  newBTC->bitCoinID = id;
  newBTC->next = NULL;

  //Adding a pointer to the tree
  newBTC->tree = root;

  //After I made sure the bitCoin is unique I enter it in the bitcoin list
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

  return 1;
}

table* hash_init(int num_entries, int bucketSize){
  int i;

  table *hash_table = (table*)malloc(sizeof(table));
  if (hash_table == NULL)
  {
    perror("Malloc failed");
    exit(0);
  }

  hash_table->size = num_entries;
  hash_table->bucket_size = bucketSize;
  strcpy(hash_table->greatestTransactionID, "");
  hash_table->latestTransaction = 0;

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

int readTransactions(FILE *transactionsFile, wallet *walletList, table *senderHashtable, table *receiverHashtable){
  int value, i;
  long int epoch;
  char whole_line[250], senderWalletID[50], receiverWalletID[50];
  char array[9][55];
  char* token;
  char delimiters[] = " -:";
  transaction_info info;

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
      if (i < 9)
      {
        strcpy(array[i], token);
      }
    }

    //Assigning the correct value to each variable
    strcpy(senderWalletID, array[1]);
    strcpy(receiverWalletID, array[2]);
    info.value = value = atoi(array[3]);
    info.day = atoi(array[4]);
    info.month = atoi(array[5]);
    info.year = atoi(array[6]);
    info.hours = atoi(array[7]);
    info.minutes = atoi(array[8]);
    strcpy(info.transactionID, array[0]);
    strcpy(info.sender, array[1]);

    //Checking if the transaction is valid
    if (checkTransaction(walletList, senderWalletID, receiverWalletID, value) == 0)
    {
      printf("Invalid transaction.\n");
      continue;
    }
    if (checkTransactionID(array[0], receiverHashtable) == 0)
    {
      //Exit if found one duplicate transactionID
      printf("The transactionID isn't unique, exiting.\n");
      return -1;
    }

    enterTransaction(senderWalletID, senderHashtable, receiverWalletID, receiverHashtable, walletList, info);

    //Since the transactionsFile is sorted by time and date I change the time of the latest transaction
    epoch = getUnixTime(info.year, info.month, info.day, info.hours, info.minutes);
    senderHashtable->latestTransaction = epoch;
    receiverHashtable->latestTransaction = epoch;
  }

  fclose(transactionsFile);
  return 1;
}

int checkTransactionID(char *transactionID, table *hash_t){
  //TODO check other buckets not just first
  int i, j;
  bucket *buc;
  transaction *trans;

  //Making sure that the transactionID is unique
  for (i = 0; i < hash_t->size; i++)
  {
    buc = hash_t->h_table[i];

    if (buc != NULL)
    {
      for (j = 0; j < buc->size; j++)
      {
        trans = buc->entries[j].transactions;
        while (trans != NULL)
        {
          if (strcmp(transactionID, trans->tree->info.transactionID) == 0)
          {
            return 0;
          }

          trans = trans->next;
        }
      }
    }
  }

  return 1;
}

int checkTransaction(wallet *walletList, char *senderWalletID, char *receiverWalletID, int value){
  wallet_node *curr = walletList->nodes;
  int sum = 0, flag = 0;
  leaf *coin;

  if (value == 0)
  {
    printf("There is no point in transfering 0$. ");
    return 0;
  }

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
        printf("The senderWalletID doesn't have enough money in its wallet. ");
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
  for (i = 0; i < (int)strlen(id); i++)
  {
    value += (int)id[i];
  }

  return (value % range);
}

void enterTransaction(char *senderWalletID, table *senderHashtable, char *receiverWalletID, table *receiverHashtable, wallet *walletList, transaction_info t_i){
  int buc, i, flag, remainder = t_i.value, place, old_value, bucketSize = senderHashtable->bucket_size;
  bucket *b, *prev, *temp;
  transaction *trans, *curr_trans, *transa;
  wallet_node *curr_wall;
  leaf *coin;

  //Updating the tables with the greatest transactionID yet
  if (strcmp(t_i.transactionID, senderHashtable->greatestTransactionID) > 0)
  {
    strcpy(senderHashtable->greatestTransactionID, t_i.transactionID);
    strcpy(receiverHashtable->greatestTransactionID, t_i.transactionID);
  }

  //Updating the senderHashtable first
  buc = hash_function(senderWalletID, senderHashtable->size);

  flag = 0;
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
      b->entries[i].transactions = NULL;
    }

    b->next = NULL;

    //Entering the walletID in the first cell
    b->entries[0].empty = 0;
    strcpy(b->entries[0].walletID, senderWalletID);
    b->entries[0].transactions = NULL;

    //Finaly updating the hash table
    senderHashtable->h_table[buc] = b;
    place = 0;
    flag = 1;
  }
  else
  {
    //The bucket exists and I need to find an empty space
    temp = senderHashtable->h_table[buc];

    //Iterate through the bucket(s) to find the walletID
    while ((temp != NULL) && (flag == 0))
    {
      for (i = 0; i < temp->size; i++)
      {
        if (temp->entries[i].empty == 0)
        {
          if (strcmp(temp->entries[i].walletID, senderWalletID) == 0)
          {
            //Found the correct place
            place = i;
            b = temp;

            flag = 1;
            break;
          }
        }
        else
        {
          //Found an empty cell to put the walletID and its transactions
          temp->entries[i].empty = 0;
          strcpy(temp->entries[i].walletID, senderWalletID);
          temp->entries[i].transactions = NULL;
          place = i;
          b = temp;

          flag = 1;
          break;
        }
      }

      prev = temp;
      temp = temp->next;
    }
  }

  //I need to add an overflow bucket
  if (flag == 0)
  {
    temp = (bucket*)malloc(sizeof(bucket));
    if (temp == NULL)
    {
      perror("Malloc failed");
      exit(0);
    }

    temp->entries = (bucket_cell*)malloc(((bucketSize - sizeof(bucket)) / sizeof(bucket_cell)) * sizeof(bucket_cell));
    if (temp->entries == NULL)
    {
      perror("Malloc failed");
      exit(0);
    }

    temp->size = (bucketSize - sizeof(bucket)) / sizeof(bucket_cell);  //Number of entries in every bucket

    for (i = 0; i < temp->size; i++)
    {
      temp->entries[i].empty = 1;
      b->entries[i].transactions = NULL;
    }

    temp->next = NULL;

    //Entering the walletID in the first cell
    temp->entries[0].empty = 0;
    strcpy(temp->entries[0].walletID, senderWalletID);
    temp->entries[0].transactions = NULL;

    prev->next = temp;  //Adding the overflow bucket after the last full bucket
    b = prev->next;
    place = 0;
  }

  //Allocating a new transaction node to add to the list
  trans = (transaction*)malloc(sizeof(transaction));
  if (trans == NULL)
  {
    perror("Malloc failed");
    exit(0);
  }
  trans->tree = NULL;
  transa = (transaction*)malloc(sizeof(transaction));
  if (transa == NULL)
  {
    perror("Malloc failed");
    exit(0);
  }
  transa->tree = NULL;

  //Searching through the senders wallet to find enough balance
  curr_wall = walletList->nodes;
  while (curr_wall != NULL)
  {
    if (strcmp(senderWalletID, curr_wall->walletID) == 0)
    {
      //Iterating through the availiable bitcoins to find enough balance to transfer
      coin = curr_wall->bitcoins;
      while (coin != NULL)
      {
        if (coin->balance->value <= 0)
        {
          coin = coin->next;
          continue;
        }

        old_value = coin->balance->value;

        //Updating the bitcoin's tree
        tree_node *new_balance = (tree_node*)malloc(sizeof(tree_node));
        if (new_balance == NULL)
        {
          perror("Malloc failed");
          exit(0);
        }
        strcpy(new_balance->walletID, receiverWalletID);
        new_balance->right = NULL;
        new_balance->left = NULL;
        tree_node *rest = (tree_node*)malloc(sizeof(tree_node));
        if (rest == NULL)
        {
          perror("Malloc failed");
          exit(0);
        }
        strcpy(rest->walletID, senderWalletID);
        rest->right = NULL;
        rest->left = NULL;

        if ((coin->balance->value - remainder) <= 0)
        {
          new_balance->value = coin->balance->value;
          rest->value = 0;
        }
        else
        {
          new_balance->value = remainder;
          rest->value = coin->balance->value - remainder;
        }

        //Updating the tree
        coin->balance->left = new_balance;
        //Since the left child is the one that takes part in the transaction,
        //only the left child has information about the transaction
        coin->balance->left->info = t_i;
        coin->balance->right = rest;

        //The transaction for the sender
        if (trans->tree == NULL)
        {
          trans->tree = coin->balance->left;
          trans->next = NULL;
          trans->next_bitcoin = NULL;
        }
        else
        {
          //In the case that more than one bitcoin is needed
          //to complete the transfer
          curr_trans = trans;
          while((curr_trans->next_bitcoin) != NULL)
          {
            curr_trans = curr_trans->next_bitcoin;
          }

          curr_trans->next_bitcoin = (transaction*)malloc(sizeof(transaction));
          if (curr_trans->next_bitcoin == NULL)
          {
            perror("Malloc failed");
            exit(0);
          }
          curr_trans->next_bitcoin->tree = coin->balance->left;
          curr_trans->next_bitcoin->next_bitcoin = NULL;
        }
        //The transaction for the receiver
        if (transa->tree == NULL)
        {
          transa->tree = coin->balance->left;
          transa->next = NULL;
          transa->next_bitcoin = NULL;
        }
        else
        {
          //In the case that more than one bitcoin is needed
          //to complete the transfer
          curr_trans = transa;
          while((curr_trans->next_bitcoin) != NULL)
          {
            curr_trans = curr_trans->next_bitcoin;
          }

          curr_trans->next_bitcoin = (transaction*)malloc(sizeof(transaction));
          if (curr_trans->next_bitcoin == NULL)
          {
            perror("Malloc failed");
            exit(0);
          }
          curr_trans->next_bitcoin->tree = coin->balance->left;
          curr_trans->next_bitcoin->next_bitcoin = NULL;
        }

        //Also update the wallets
        addBitcointoWallet(walletList, receiverWalletID, coin->bitcoin, coin->balance->left); //receiver
        coin->balance = coin->balance->right; //sender

        //If more money are needed in order to complete the transfer
        //the process is repeated
        remainder -= old_value;
        if (remainder <= 0)
        {
          break;
        }
        coin = coin->next;
      }

      break;
    }

    curr_wall = curr_wall->next;
  }

  //Adding the new transaction to the bucket b at the place that was found, at the end of the list
  if (b->entries[place].transactions == NULL)
  {
    b->entries[place].transactions = trans;
  }
  else
  {
    curr_trans = b->entries[place].transactions;
    while((curr_trans->next) != NULL)
    {
      curr_trans = curr_trans->next;
    }

    curr_trans->next = trans;
  }

  //Updating the receiverHashtable now
  buc = hash_function(receiverWalletID, receiverHashtable->size);

  flag = 0;
  if (receiverHashtable->h_table[buc] == NULL)
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
      b->entries[i].transactions = NULL;
    }

    b->next = NULL;

    //Entering the walletID in the first cell
    b->entries[0].empty = 0;
    strcpy(b->entries[0].walletID, receiverWalletID);
    b->entries[0].transactions = NULL;

    //Finaly updating the hash table
    receiverHashtable->h_table[buc] = b;
    place = 0;
    flag = 1;
  }
  else
  {
    //The bucket exists and I need to find an empty space
    temp = receiverHashtable->h_table[buc];

    //Iterate through the bucket(s) to find the walletID
    while ((temp != NULL) && (flag == 0))
    {
      for (i = 0; i < temp->size; i++)
      {
        if (temp->entries[i].empty == 0)
        {
          if (strcmp(temp->entries[i].walletID, receiverWalletID) == 0)
          {
            //Found the correct place
            place = i;
            b = temp;

            flag = 1;
            break;
          }
        }
        else
        {
          //Found an empty cell to put the walletID and its transactions
          temp->entries[i].empty = 0;
          strcpy(temp->entries[i].walletID, receiverWalletID);
          temp->entries[i].transactions = NULL;
          place = i;
          b = temp;

          flag = 1;
          break;
        }
      }

      prev = temp;
      temp = temp->next;
    }
  }

  //I need to add an overflow bucket
  if (flag == 0)
  {
    temp = (bucket*)malloc(sizeof(bucket));
    if (temp == NULL)
    {
      perror("Malloc failed");
      exit(0);
    }

    temp->entries = (bucket_cell*)malloc(((bucketSize - sizeof(bucket)) / sizeof(bucket_cell)) * sizeof(bucket_cell));
    if (temp->entries == NULL)
    {
      perror("Malloc failed");
      exit(0);
    }

    temp->size = (bucketSize - sizeof(bucket)) / sizeof(bucket_cell);  //Number of entries in every bucket

    for (i = 0; i < temp->size; i++)
    {
      temp->entries[i].empty = 1;
      temp->entries[i].transactions = NULL;
    }

    temp->next = NULL;

    //Entering the walletID in the first cell
    temp->entries[0].empty = 0;
    strcpy(temp->entries[0].walletID, receiverWalletID);
    temp->entries[0].transactions = NULL;

    prev->next = temp;  //Adding the overflow bucket after the last full bucket
    b = prev->next;
    place = 0;
  }

  //Adding the new transaction to the bucket b at the place that was found, at the end of the list
  if (b->entries[place].transactions == NULL)
  {
    b->entries[place].transactions = transa;
  }
  else
  {
    curr_trans = b->entries[place].transactions;
    while((curr_trans->next) != NULL)
    {
      curr_trans = curr_trans->next;
    }

    curr_trans->next = transa;
  }
}

void addBitcointoWallet(wallet *walletList, char *wID, bitcoin_node *id, tree_node *trans){
  wallet_node *curr_wall = walletList->nodes;
  leaf *new_leaf, *curr_leaf;

  //Find the correct wallet and add a new bitcoin
  while (curr_wall != NULL)
  {
    if (strcmp(curr_wall->walletID, wID) == 0)
    {
      new_leaf = (leaf*)malloc(sizeof(leaf));
      if (new_leaf == NULL)
      {
        perror("Malloc failed");
        exit(0);
      }
      new_leaf->bitcoin = id;
      new_leaf->balance = trans;
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

long int getUnixTime(int y, int mon, int d, int h, int min){
  struct tm t;
  time_t t_of_day;

  t.tm_year = y - 1900;
  t.tm_mon = mon - 1;
  t.tm_mday = d;
  t.tm_hour = h;
  t.tm_min = min;
  t.tm_sec = 0;
  t.tm_isdst = -1;
  t_of_day = mktime(&t);

  return (long)t_of_day;
}
