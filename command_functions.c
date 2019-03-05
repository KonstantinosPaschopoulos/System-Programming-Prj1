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
