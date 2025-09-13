/*****************************************************************************
  @file     Segurity.h
  @brief    Encargado de la seguridad del dispositivo
  @author   jpla
  @version  1.0 - coding
 ******************************************************************************/

/*******************************************************************************
*                                ENCABEZADOS
******************************************************************************/
#include "Segurity.h"
#include <string.h>

/*******************************************************************************
*                                 OBJETOS
******************************************************************************/
typedef struct {
	char ID[ID_MAX_LENGTH];
	char PIN[PIN_MAX_LENGTH+1]; // contando el terminador se puede 4 o 5 digitos (5 o 6 chars)
	uint8_t permissions;
} user_t;

/*******************************************************************************
*                                 VARIABLES
******************************************************************************/
static user_t users[10] = {
    { "12345678", "12345", USER },
	{ "37159500", "00005", USER },
    // otros usuarios...
}; // 10 hay que cambiarlo por el numero de users


/*******************************************************************************
*                                FUNCIONES
******************************************************************************/

bool IDSentinel(char * pID)
{
	uint8_t counter, i;
	for(i = 0, counter = 0 ; i <= 10 ; i++) // 10 hay que cambiarlo por el numero de users
	{
		for(int j = 0 ; j <= ID_MAX_LENGTH - 1 ; j++)
		{
			if(users[i].ID[j] == pID[j])
			{
				counter++;
				if(counter == ID_MAX_LENGTH)
				{
					return 1;
				}
			}
		}
		counter = 0;
	}
	return 0;
}

bool Alohomora(char * pID, char * pPIN)
{
	// llamo a userNumber para conocer que numero de user es el del ID
	uint8_t userNumber = UserNumber(pID);
	if(userNumber--)
	{
		uint8_t counter = 0;
		while(users[userNumber].PIN[counter] == pPIN[counter] && users[userNumber].PIN[counter] != 0)
		{
			counter ++;
		}
		if(counter == strlen(users[userNumber].PIN))
		{
			return true;
		}
	}
	return false;
}

uint8_t UserNumber(char * pID)
{
	uint8_t counter, i;
	for(i = 0, counter = 0; i <= 10 ; i++) // 10 hay que cambiarlo por el numero de users
	{
		for(int j = 0 ; j <= ID_MAX_LENGTH - 1 ; j++)
		{
			if(users[i].ID[j] != pID[j])
			{
				counter = 0;
				break;
			}
			else
			{
				counter++;
				if(counter == ID_MAX_LENGTH)
				{
					return i + 1;
				}
			}
		}
		counter = 0;
	}
	return false;
}
