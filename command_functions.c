#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
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
    if (strcmp(pch, "requestTransaction") == 0)
    {
      return 1;
    }
    else if (strcmp(pch, "requestTransactions") == 0)
    {
      //Checking to see there is a semicolon in the rest of command
      semicolon_position = strchr(semicolon_copy, ';');
      if (semicolon_position == NULL)
      {
        //This is the second case of the requestTransactions command
        return 3;
      }
      else
      {
        //This is the first case of the requestTransactions command
        return 2;
      }
    }
    else if (strcmp(pch, "findEarnings") == 0)
    {
      return 4;
    }
    else if (strcmp(pch, "findPayments") == 0)
    {
      return 5;
    }
    else if (strcmp(pch, "walletStatus") == 0)
    {
      return 6;
    }
    else if (strcmp(pch, "bitCoinStatus") == 0)
    {
      return 7;
    }
    else if (strcmp(pch, "traceCoin") == 0)
    {
      return 8;
    }
    else if (strcmp(pch, "exit") == 0)
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

int requestTransaction(char *user_input, wallet *walletList, table *senderHashtable, table *receiverHashtable){
  char *token, *temp;
  char delimiters[] = " -:", input_copy[MAX_INPUT], receiverWalletID[50];
  transaction_info info;
  int i;
  char array[9][55];
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);

  strcpy(input_copy, user_input);

  //Using strtok to tokenize the input
  for (token = strtok(input_copy, delimiters), i = 0; token; token = strtok(NULL, delimiters), i++)
  {
    if (i < 9)
    {
      strcpy(array[i], token);
    }
  }

  strcpy(info.sender, array[1]);
  strcpy(receiverWalletID, array[2]);
  info.value = atoi(array[3]);

  if (i == 9)
  {
    //The user gave the time and date
    info.day = atoi(array[4]);
    info.month = atoi(array[5]);
    info.year = atoi(array[6]);
    info.hours = atoi(array[7]);
    info.minutes = atoi(array[8]);

    //TODO Checking if the given time and date are acceptable

  }
  else if (i == 4)
  {
    //Use current the current time and date
    info.day = tm.tm_mday;
    info.month = tm.tm_mon + 1;
    info.year = tm.tm_year + 1900;
    info.hours = tm.tm_hour;
    info.minutes = tm.tm_min;

    //TODO Checking if the given time and date are acceptable
  }
  else
  {
    printf("Wrong command syntax. Invalid transaction.\n");
    return -1;
  }

  //Checking if the transaction is valid
  if (checkTransaction(walletList, info.sender, receiverWalletID, info.value) == 0)
  {
    printf("Invalid transaction.\n");
    return -1;
  }

  //Generating a new unique transactionID
  temp = uniqueID(senderHashtable->greatestTransactionID);
  strcpy(info.transactionID, temp);

  //Adding the transaction to the system
  enterTransaction(info.sender, senderHashtable, receiverWalletID, receiverHashtable, walletList, info);
  printf("Done!\n");

  return 1;
}

void requestTransactions(char *user_input, wallet *walletList, table *senderHashtable, table *receiverHashtable){
  char whole_line[MAX_INPUT];
  char *token, *temp;
  char delimiters[] = " -:", input_copy[MAX_INPUT], receiverWalletID[50];
  transaction_info info;
  int i;
  char array[8][55];
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);

  //Calling requestTransaction to deal with the first transaction
  user_input[strlen(user_input) - 1] = '\0';
  if (requestTransaction(user_input, walletList, senderHashtable, receiverHashtable) == -1)
  {
    printf("Skipping the transaction!\n");
  }

  //Using fgets to get the rest of the user's commands
  while (fgets(whole_line, MAX_INPUT, stdin))
  {
    //The user must type exit to exit this command
    if (strcmp(whole_line, "exit\n") == 0)
    {
      printf("Exiting requestTransactions.\n");
      return;
    }

    if ((strlen(whole_line) > 0) && (whole_line[strlen(whole_line) - 1] == '\n'))
    {
      //Making sure the each line ends with a semicolon
      if (whole_line[strlen(whole_line) - 2] != ';')
      {
        printf("Wrong command syntax. Skipping the transaction!\n");
        continue;
      }
      whole_line[strlen(whole_line) - 2] = '\0';
    }

    strcpy(input_copy, whole_line);

    //Using strtok to tokenize the input
    for (token = strtok(input_copy, delimiters), i = 0; token; token = strtok(NULL, delimiters), i++)
    {
      if (i < 8)
      {
        strcpy(array[i], token);
      }
    }

    strcpy(info.sender, array[0]);
    strcpy(receiverWalletID, array[1]);
    info.value = atoi(array[2]);

    if (i == 8)
    {
      //The user gave the time and date
      info.day = atoi(array[3]);
      info.month = atoi(array[4]);
      info.year = atoi(array[5]);
      info.hours = atoi(array[6]);
      info.minutes = atoi(array[7]);

      //TODO Checking if the given time and date are acceptable

    }
    else if (i == 3)
    {
      //Use current the current time and date
      info.day = tm.tm_mday;
      info.month = tm.tm_mon + 1;
      info.year = tm.tm_year + 1900;
      info.hours = tm.tm_hour;
      info.minutes = tm.tm_min;

      //TODO Checking if the given time and date are acceptable
    }
    else
    {
      printf("Wrong command syntax. Skipping the transaction!\n");
      continue;
    }

    //Checking if the transaction is valid
    if (checkTransaction(walletList, info.sender, receiverWalletID, info.value) == 0)
    {
      printf("Invalid transaction. Skipping the transaction!\n");
      continue;
    }

    //Generating a new unique transactionID
    temp = uniqueID(senderHashtable->greatestTransactionID);
    strcpy(info.transactionID, temp);

    //Adding the transaction to the system
    enterTransaction(info.sender, senderHashtable, receiverWalletID, receiverHashtable, walletList, info);
    printf("Done!\n");
  }
}

void requestTransactionsFile(char *user_input, wallet *walletList, table *senderHashtable, table *receiverHashtable){
  FILE *input = NULL;
  char command[50], input_file[50], whole_line[MAX_INPUT], input_copy[MAX_INPUT], receiverWalletID[50];
  char *token, *temp;
  char delimiters[] = " -:";
  transaction_info info;
  int i;
  char array[8][55];
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);

  sscanf(user_input, "%s %s", command, input_file);

  input = fopen(input_file, "r");
  if (input == NULL)
  {
    perror("Could not open the input file");
    return;
  }

  //Using fgets to get the rest of the user's commands
  while (fgets(whole_line, MAX_INPUT, input))
  {
    if ((strlen(whole_line) > 0) && (whole_line[strlen(whole_line) - 1] == '\n'))
    {
      //Making sure the each line ends with a semicolon
      if (whole_line[strlen(whole_line) - 2] != ';')
      {
        printf("Wrong command syntax. Skipping the transaction!\n");
        continue;
      }
      whole_line[strlen(whole_line) - 2] = '\0';
    }

    strcpy(input_copy, whole_line);

    //Using strtok to tokenize the input
    for (token = strtok(input_copy, delimiters), i = 0; token; token = strtok(NULL, delimiters), i++)
    {
      if (i < 8)
      {
        strcpy(array[i], token);
      }
    }

    strcpy(info.sender, array[0]);
    strcpy(receiverWalletID, array[1]);
    info.value = atoi(array[2]);

    if (i == 8)
    {
      //The user gave the time and date
      info.day = atoi(array[3]);
      info.month = atoi(array[4]);
      info.year = atoi(array[5]);
      info.hours = atoi(array[6]);
      info.minutes = atoi(array[7]);

      //TODO Checking if the given time and date are acceptable

    }
    else if (i == 3)
    {
      //Use current the current time and date
      info.day = tm.tm_mday;
      info.month = tm.tm_mon + 1;
      info.year = tm.tm_year + 1900;
      info.hours = tm.tm_hour;
      info.minutes = tm.tm_min;

      //TODO Checking if the given time and date are acceptable
    }
    else
    {
      printf("Wrong command syntax. Skipping the transaction!\n");
      continue;
    }

    //Checking if the transaction is valid
    if (checkTransaction(walletList, info.sender, receiverWalletID, info.value) == 0)
    {
      printf("Invalid transaction. Skipping the transaction!\n");
      continue;
    }

    //Generating a new unique transactionID
    temp = uniqueID(senderHashtable->greatestTransactionID);
    strcpy(info.transactionID, temp);

    //Adding the transaction to the system
    enterTransaction(info.sender, senderHashtable, receiverWalletID, receiverHashtable, walletList, info);
    printf("Done!\n");
  }

  fclose(input);
}

char *uniqueID(char *latest){
  int i;
  char randomCharacters[] = "abcefghijklmonpqstuvwxyz1234567890ABCDEFGHIJKLMONPQSTYVWXYZ";
  char *newID = (char*)malloc(50 * sizeof(char));
  if (newID == NULL)
  {
    perror("Malloc failed");
    exit(0);
  }

  //Creating random strings until one of them is greater than the greatest transactionID
  do {
    for (i = 0; i < 50; i++)
    {
      newID[i] = randomCharacters[rand() % (sizeof(randomCharacters) - 1)];
    }
    newID[49] = '\0';
  } while (strcmp(newID, latest) < 1);

  return newID;
}

void findEarnings(char *user_input, table *hash_table){
  char walletID[50];
  int time1, day1, month1, year1, pos, i, comm;
  int time2, day2, month2, year2;
  int time;
  transaction *trans;
  int sum;
  bucket *buc;

  comm = checkFind(user_input, walletID, &time1, &day1, &month1, &year1, &time2, &day2, &month2, &year2);

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
            time = (trans->tree->info.hours * 100) + trans->tree->info.minutes;
            //Depending on the syntax of the command, different stuff will have to be done
            if (comm == 1)
            {
              if ((time < time1) || (time > time2))
              {
                trans = trans->next;
                continue;
              }
            }
            else if (comm == 2)
            {
              if (trans->tree->info.year < year1 || trans->tree->info.year > year2)
              {
                trans = trans->next;
                continue;
              }
              if (trans->tree->info.month < month1 || trans->tree->info.month > month2)
              {
                trans = trans->next;
                continue;
              }
              if (trans->tree->info.day < day1 || trans->tree->info.day > day2)
              {
                trans = trans->next;
                continue;
              }
            }
            else if (comm == 3)
            {
              if ((time < time1) || (time > time2))
              {
                trans = trans->next;
                continue;
              }
              if (trans->tree->info.year < year1 || trans->tree->info.year > year2)
              {
                trans = trans->next;
                continue;
              }
              if (trans->tree->info.month < month1 || trans->tree->info.month > month2)
              {
                trans = trans->next;
                continue;
              }
              if (trans->tree->info.day < day1 || trans->tree->info.day > day2)
              {
                trans = trans->next;
                continue;
              }
            }
            sum += trans->tree->info.value;

            trans = trans->next;
          }
          printf("The wallet %s has received %d$\n", walletID, sum);


          //Now print all the transactions that are needed
          trans = buc->entries[i].transactions;
          while (trans != NULL)
          {
            time = (trans->tree->info.hours * 100) + trans->tree->info.minutes;

            if (comm == 1)
            {
              if ((time < time1) || (time > time2))
              {
                trans = trans->next;
                continue;
              }
            }
            else if (comm == 2)
            {
              if (trans->tree->info.year < year1 || trans->tree->info.year > year2)
              {
                trans = trans->next;
                continue;
              }
              if (trans->tree->info.month < month1 || trans->tree->info.month > month2)
              {
                trans = trans->next;
                continue;
              }
              if (trans->tree->info.day < day1 || trans->tree->info.day > day2)
              {
                trans = trans->next;
                continue;
              }
            }
            else if (comm == 3)
            {
              if ((time < time1) || (time > time2))
              {
                trans = trans->next;
                continue;
              }
              if (trans->tree->info.year < year1 || trans->tree->info.year > year2)
              {
                trans = trans->next;
                continue;
              }
              if (trans->tree->info.month < month1 || trans->tree->info.month > month2)
              {
                trans = trans->next;
                continue;
              }
              if (trans->tree->info.day < day1 || trans->tree->info.day > day2)
              {
                trans = trans->next;
                continue;
              }
            }

            printf("%s %s %s %d %d-%d-%d %d:%d\n", trans->tree->info.transactionID,
                      trans->tree->info.sender, trans->tree->walletID,
                      trans->tree->info.value, trans->tree->info.day, trans->tree->info.month,
                      trans->tree->info.year, trans->tree->info.hours, trans->tree->info.minutes);

            trans = trans->next;
          }

          return;
        }
      }
    }

    buc = buc->next;
  }

  printf("The wallet %s either doesn't exist or it hasn't receiven any money\n", walletID);
}

void findPayments(char *user_input, table *hash_table){
  char walletID[50];
  int time1, day1, month1, year1, pos, i, comm;
  int time2, day2, month2, year2;
  int time;
  transaction *trans;
  int sum;
  bucket *buc;

  comm = checkFind(user_input, walletID, &time1, &day1, &month1, &year1, &time2, &day2, &month2, &year2);

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
            time = (trans->tree->info.hours * 100) + trans->tree->info.minutes;
            //Depending on the syntax of the command, different stuff will have to be done
            if (comm == 1)
            {
              if ((time < time1) || (time > time2))
              {
                trans = trans->next;
                continue;
              }
            }
            else if (comm == 2)
            {
              if (trans->tree->info.year < year1 || trans->tree->info.year > year2)
              {
                trans = trans->next;
                continue;
              }
              if (trans->tree->info.month < month1 || trans->tree->info.month > month2)
              {
                trans = trans->next;
                continue;
              }
              if (trans->tree->info.day < day1 || trans->tree->info.day > day2)
              {
                trans = trans->next;
                continue;
              }
            }
            else if (comm == 3)
            {
              if ((time < time1) || (time > time2))
              {
                trans = trans->next;
                continue;
              }
              if (trans->tree->info.year < year1 || trans->tree->info.year > year2)
              {
                trans = trans->next;
                continue;
              }
              if (trans->tree->info.month < month1 || trans->tree->info.month > month2)
              {
                trans = trans->next;
                continue;
              }
              if (trans->tree->info.day < day1 || trans->tree->info.day > day2)
              {
                trans = trans->next;
                continue;
              }
            }
            sum += trans->tree->info.value;

            trans = trans->next;
          }
          printf("The wallet %s has paid %d$\n", walletID, sum);


          //Now print all the transactions that are needed
          trans = buc->entries[i].transactions;
          while (trans != NULL)
          {
            time = (trans->tree->info.hours * 100) + trans->tree->info.minutes;

            if (comm == 1)
            {
              if ((time < time1) || (time > time2))
              {
                trans = trans->next;
                continue;
              }
            }
            else if (comm == 2)
            {
              if (trans->tree->info.year < year1 || trans->tree->info.year > year2)
              {
                trans = trans->next;
                continue;
              }
              if (trans->tree->info.month < month1 || trans->tree->info.month > month2)
              {
                trans = trans->next;
                continue;
              }
              if (trans->tree->info.day < day1 || trans->tree->info.day > day2)
              {
                trans = trans->next;
                continue;
              }
            }
            else if (comm == 3)
            {
              if ((time < time1) || (time > time2))
              {
                trans = trans->next;
                continue;
              }
              if (trans->tree->info.year < year1 || trans->tree->info.year > year2)
              {
                trans = trans->next;
                continue;
              }
              if (trans->tree->info.month < month1 || trans->tree->info.month > month2)
              {
                trans = trans->next;
                continue;
              }
              if (trans->tree->info.day < day1 || trans->tree->info.day > day2)
              {
                trans = trans->next;
                continue;
              }
            }

            printf("%s %s %s %d %d-%d-%d %d:%d\n", trans->tree->info.transactionID,
                      trans->tree->info.sender, trans->tree->walletID,
                      trans->tree->info.value, trans->tree->info.day, trans->tree->info.month,
                      trans->tree->info.year, trans->tree->info.hours, trans->tree->info.minutes);

            trans = trans->next;
          }

          return;
        }
      }
    }

    buc = buc->next;
  }

  printf("The wallet %s either doesn't exist or it hasn't made any payments\n", walletID);
}

int checkFind(char *input, char *walletID, int *time1, int *day1, int*month1, int *year1, int *time2, int *day2, int*month2, int *year2){
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
    *time1 = (atoi(array[2]) * 100) + atoi(array[3]);
    *time2 = (atoi(array[4]) * 100) + atoi(array[5]);
    return 1;
  }
  else if (i == 8)
  {
    //Gave dates
    *day1 = atoi(array[2]);
    *month1 = atoi(array[3]);
    *year1 = atoi(array[4]);
    *day2 = atoi(array[5]);
    *month2 = atoi(array[6]);
    *year2 = atoi(array[7]);
    return 2;
  }
  else if (i == 12)
  {
    //Gave everything
    *time1 = (atoi(array[2]) * 100) + atoi(array[3]);
    *day1 = atoi(array[4]);
    *month1 = atoi(array[5]);
    *year1 = atoi(array[6]);
    *time2 = (atoi(array[7]) * 100) + atoi(array[8]);
    *day2 = atoi(array[9]);
    *month2 = atoi(array[10]);
    *year2 = atoi(array[11]);
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

int checkInput(char *input){
  char input_copy[MAX_INPUT];
  char *token;
  int i, j, flag = 0;

  strcpy(input_copy, input);

  //Count how many arguments are in the command
  for (token = strtok(input_copy, " "), i = 0; token; token = strtok(NULL, " "), i++)
  {
    //Also check if the second argument is a number
    if (i == 1)
    {
      for (j = 0; j < (int)strlen(token); j++)
      {
        if(token[j] > '9' || token[j] < '0')
        {
          flag = 1;
          break;
        }
      }
    }
  }

  if (i != 2)
  {
    return -1;
  }

  if (flag == 0)
  {
    return 1;
  }
  else
  {
    return 2;
  }
}

void exitProgram(List *bitcoinList, wallet *walletList, table *senderHashtable, table *receiverHashtable){

}
