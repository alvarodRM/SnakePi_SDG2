#ifndef _SNAKEPI_H_
#define _SNAKEPI_H_

#include "systemLib.h"
#include "teclado_TL04.h"
#include "ledDisplay.h"
#include "snakePiLib.h"
#include <stdio.h> //Mejora vidas
#include "pseudoWiringPi.h"

int vidas;
int numManzanas;
char numero[5];

typedef struct {
	tipo_snakePi snakePi;
	int debug;
} TipoSistema;

//------------------------------------------------------
// FUNCIONES DE CONFIGURACION/INICIALIZACION
//------------------------------------------------------

int ConfiguraInicializaSistema (TipoSistema *p_sistema);
int validarNumero(char numero[]);

//------------------------------------------------------
// FUNCIONES LIGADAS A THREADS ADICIONALES
//------------------------------------------------------

PI_THREAD(thread_explora_teclado_PC);

#endif /* SNAKEPI_H_ */
