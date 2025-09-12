/*****************************************************************************
  @file     Segurity.h
  @brief    Encargado de la seguridad del dispositivo
  @author   jpla
  @version  1.0 - coding
 ******************************************************************************/

#ifndef APP_SEGURITY_H_
#define APP_SEGURITY_H_

/*******************************************************************************
*                                ENCABEZADOS
******************************************************************************/
#include <stdint.h>
#include <stdbool.h>

/*******************************************************************************
*                               ENUMERACIONES
******************************************************************************/
enum permissions{
	MASTER,
	USER,
	SERVICE
};

/*******************************************************************************
*                                 MACROS
******************************************************************************/
#define ID_MAX_LENGTH 8
#define PIN_MAX_LENGTH 5

/*******************************************************************************
*                                 PROTOTIPOS
******************************************************************************/
// devuelve 1 si el usuario existe, cc 0
bool IDSentinel(char * pID);

// devuelve 1 si el ID y el PIN coinciden
bool Alohomora(char * pID, char * pPIN);

// devuelve el numero de usuario dentro de la lista
uint8_t UserNumber(char * pID);

#endif /* APP_SEGURITY_H_ */
