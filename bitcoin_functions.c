#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mytypes.h"
#include "bitcoin_functions.h"

void readBalances(FILE *bitCoinBalancesFile, List *bitcoinList){
  int name;
  char whole_line[250];
  char * pch;

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
      }
      else
      {
        //Storing the bitcoin
        enterBitcoin(atoi(pch), bitcoinList);
      }

      pch = strtok(NULL, " ");
    }
  }
}

void enterBitcoin(int id, List *bitcoinList){
  bitcoin_node *curr = bitcoinList->nodes;

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

  //--Create the tree
  newBTC->bitCoinID = id;
  //--Add pointer to the tree

  //After I made sure the bitCoin is unique
  //I enter it in the main memory
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
}
