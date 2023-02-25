#include "teclado_TL04.h"

TipoTeclado p_teclado;


char tecladoTL04[4][4] = {
	{'1', '2', '3', 'w'},
	{'4', '5', '6', 'D'},
	{'7', '8', '9', 'E'},
	{'A', '0', 'B', 'F'}
};

// Maquina de estados: lista de transiciones
// {EstadoOrigen, CondicionDeDisparo, EstadoFinal, AccionesSiTransicion }
fsm_trans_t fsm_trans_excitacion_columnas[] = {
	{ TECLADO_ESPERA_COLUMNA, CompruebaTimeoutColumna, TECLADO_ESPERA_COLUMNA, TecladoExcitaColumna },
	{-1, NULL, -1, NULL },
};

fsm_trans_t fsm_trans_deteccion_pulsaciones[] = {
	{ TECLADO_ESPERA_TECLA, CompruebaTeclaPulsada, TECLADO_ESPERA_TECLA, ProcesaTeclaPulsada},
	{-1, NULL, -1, NULL },
};

//------------------------------------------------------
// PROCEDIMIENTOS DE INICIALIZACION DE LOS OBJETOS ESPECIFICOS
//------------------------------------------------------

void InicializaTeclado(TipoTeclado *p_teclado) {
	//EMPIEZO

		teclado.flags=0;

		teclado.columna_actual=COLUMNA_1;
		//Inicializamos wiringPi
		wiringPiSetupGpio();

		if (wiringPiSetupGpio() < 0) {
			fprintf (stderr, "Unable to setup wiringPi: %s\n", strerror (errno)) ;
		}

		// Comenzamos excitacion por primera columna
		p_teclado->columna_actual = COLUMNA_1;

		// Inicialmente no hay tecla pulsada
		p_teclado->teclaPulsada.col = -1;
		p_teclado->teclaPulsada.row = -1;

		//EMPIEZA FILAS
		pinMode(GPIO_KEYBOARD_ROW_1, INPUT);
			pullUpDnControl (GPIO_KEYBOARD_ROW_1, PUD_DOWN );
			wiringPiISR(GPIO_KEYBOARD_ROW_1, INT_EDGE_RISING, teclado_fila_1_isr);
		pinMode(GPIO_KEYBOARD_ROW_2, INPUT);
			pullUpDnControl (GPIO_KEYBOARD_ROW_2, PUD_DOWN );
			wiringPiISR(GPIO_KEYBOARD_ROW_2, INT_EDGE_RISING, teclado_fila_2_isr);
		pinMode(GPIO_KEYBOARD_ROW_3, INPUT);
			pullUpDnControl (GPIO_KEYBOARD_ROW_3, PUD_DOWN );
			wiringPiISR(GPIO_KEYBOARD_ROW_3, INT_EDGE_RISING, teclado_fila_3_isr);
		pinMode(GPIO_KEYBOARD_ROW_4, INPUT);
			pullUpDnControl (GPIO_KEYBOARD_ROW_4, PUD_DOWN );
			wiringPiISR(GPIO_KEYBOARD_ROW_4, INT_EDGE_RISING, teclado_fila_4_isr);
		//TERMINA FILAS

		//EMPIEZA COLUMNAS
		pinMode(GPIO_KEYBOARD_COL_1, OUTPUT);
			digitalWrite (GPIO_KEYBOARD_COL_1, HIGH);
		pinMode(GPIO_KEYBOARD_COL_2, OUTPUT);
			digitalWrite (GPIO_KEYBOARD_COL_2, LOW);
		pinMode(GPIO_KEYBOARD_COL_3, OUTPUT);
			digitalWrite (GPIO_KEYBOARD_COL_3, LOW);
		pinMode(GPIO_KEYBOARD_COL_4, OUTPUT);
			digitalWrite (GPIO_KEYBOARD_COL_4, LOW);
		//TERMINA COLUMNAS

		p_teclado->tmr_duracion_columna = tmr_new (timer_duracion_columna_isr);
		tmr_startms((tmr_t*)(p_teclado->tmr_duracion_columna), TIMEOUT_COLUMNA_TECLADO);

		printf("\nSistema completado, TECLADO_TL04 listo\n");
		fflush(stdout);

	//TERMINO
}

//------------------------------------------------------
// OTROS PROCEDIMIENTOS PROPIOS DE LA LIBRERIA
//------------------------------------------------------

void ActualizaExcitacionTecladoGPIO (int columna) {

	// EMPIEZO
	switch(columna){
			case COLUMNA_1:
				digitalWrite (GPIO_KEYBOARD_COL_4, LOW);
				digitalWrite (GPIO_KEYBOARD_COL_1, HIGH);
				digitalWrite (GPIO_KEYBOARD_COL_2, LOW);
				digitalWrite (GPIO_KEYBOARD_COL_3, LOW);
				break;
			case COLUMNA_2:
				digitalWrite (GPIO_KEYBOARD_COL_1, LOW);
				digitalWrite (GPIO_KEYBOARD_COL_2, HIGH);
				digitalWrite (GPIO_KEYBOARD_COL_3, LOW);
				digitalWrite (GPIO_KEYBOARD_COL_4, LOW);
				break;
			case COLUMNA_3:
				digitalWrite (GPIO_KEYBOARD_COL_2, LOW);
				digitalWrite (GPIO_KEYBOARD_COL_3, HIGH);
				digitalWrite (GPIO_KEYBOARD_COL_4, LOW);
				digitalWrite (GPIO_KEYBOARD_COL_1, LOW);
				break;
			case COLUMNA_4:
				digitalWrite (GPIO_KEYBOARD_COL_3, LOW);
				digitalWrite (GPIO_KEYBOARD_COL_4, HIGH);
				digitalWrite (GPIO_KEYBOARD_COL_1, LOW);
				digitalWrite (GPIO_KEYBOARD_COL_2, LOW);
				break;
			default:
				break;
		}
	//TERMINO
}

//------------------------------------------------------
// FUNCIONES DE ENTRADA O DE TRANSICION DE LA MAQUINA DE ESTADOS
//------------------------------------------------------

int CompruebaTimeoutColumna (fsm_t* this) {
	//	TipoTeclado *p_teclado;
	//	p_teclado = (TipoTeclado*)(this->user_data);
		int result = 0;

		piLock(KEYBOARD_KEY);
		result = teclado.flags & FLAG_TIMEOUT_COLUMNA_TECLADO;
		piUnlock(KEYBOARD_KEY);

		return result;
}

int CompruebaTeclaPulsada (fsm_t* this) {
	//	TipoTeclado *p_teclado;
	//	p_teclado = (TipoTeclado*)(this->user_data);

		int result = 0;

		piLock(KEYBOARD_KEY);
		result = teclado.flags & FLAG_TECLA_PULSADA;
		piUnlock(KEYBOARD_KEY);

		return result;
}

//--------------------------------------------------
// FUNCIONES DE SALIDA O DE ACCION DE LAS MAQUINAS DE ESTADOS
//---------------------------------------------------
void TecladoExcitaColumna (fsm_t* this) {
		TipoTeclado *p_teclado;
		p_teclado = (TipoTeclado*)(this->user_data);

		piLock(KEYBOARD_KEY);
		p_teclado->flags &= (~FLAG_TIMEOUT_COLUMNA_TECLADO);
		piUnlock(KEYBOARD_KEY);

		p_teclado->columna_actual+=1;
		if(p_teclado->columna_actual > 3){
			p_teclado->columna_actual = COLUMNA_1;
		}
		// Llamada a ActualizaExcitacionTecladoGPIO con columna a activar como argumento
		ActualizaExcitacionTecladoGPIO(p_teclado->columna_actual);

		tmr_startms((tmr_t*)(p_teclado->tmr_duracion_columna), TIMEOUT_COLUMNA_TECLADO);
}

void ProcesaTeclaPulsada (fsm_t* this) {
	//EMPIEZA
	TipoTeclado *p_teclado;
	p_teclado = (TipoTeclado*)(this->user_data);

	piLock(KEYBOARD_KEY);
	p_teclado->flags &= (~FLAG_TECLA_PULSADA);
	piUnlock(KEYBOARD_KEY);

	switch(p_teclado->teclaPulsada.col){
		case COLUMNA_1:
				if(p_teclado->teclaPulsada.row == FILA_1){
					piLock(SYSTEM_FLAGS_KEY);
					flags |= FLAG_MOV_IZQUIERDA;
					piUnlock(SYSTEM_FLAGS_KEY);
					//printf("\n¡TECLADO_TL04 tecla 1 IZQUIERDA!\n");
					//fflush(stdout);
				}
				else if(p_teclado->teclaPulsada.row == FILA_2){
					piLock(SYSTEM_FLAGS_KEY);
					flags |= FLAG_BOTON;
					piUnlock(SYSTEM_FLAGS_KEY);
					//printf("\n¡TECLADO_TL04 tecla 4 ARRANCAR JUEGO!\n");
					//fflush(stdout);
				}
				break;
		case COLUMNA_2:
				if(p_teclado->teclaPulsada.row == FILA_1){
					piLock(SYSTEM_FLAGS_KEY);
					flags |= FLAG_MOV_ARRIBA;
					piUnlock(SYSTEM_FLAGS_KEY);
					//printf("\n¡TECLADO_TL04 tecla 2 ARRIBA!\n");
					//fflush(stdout);
				}
				else if(p_teclado->teclaPulsada.row == FILA_2){
					//printf("\n¡TECLADO_TL04 tecla 5 SALIR!\n");
					//fflush(stdout);
					exit(0);
				}
				break;
		case COLUMNA_3:
				if(p_teclado->teclaPulsada.row == FILA_1){
					piLock(SYSTEM_FLAGS_KEY);
					flags |= FLAG_MOV_DERECHA;
					piUnlock(SYSTEM_FLAGS_KEY);
					//printf("\n¡TECLADO_TL04 tecla 3 DERECHA!\n");
					//fflush(stdout);
				}
				break;
		case COLUMNA_4:
				if(p_teclado->teclaPulsada.row == FILA_1){
					piLock(SYSTEM_FLAGS_KEY);
					flags |= FLAG_MOV_ABAJO;
					piUnlock(SYSTEM_FLAGS_KEY);
					//printf("\n¡TECLADO_TL04 tecla W ABAJO!\n");
					//fflush(stdout);
				}
				break;
		default:
				//printf("\nERROR PULSE UNA TECLA QUE SI PERTENEZCA A LOS CONTROLES\n");
				//fflush(stdout);
				p_teclado->teclaPulsada.row = -1;
				p_teclado->teclaPulsada.col = -1;
				break;
	}
	// TERMINA
}


//------------------------------------------------------
// SUBRUTINAS DE ATENCION A LAS INTERRUPCIONES
//------------------------------------------------------

void teclado_fila_1_isr (void) {
	// EMPIEZO
	if (millis () < teclado.debounceTime[FILA_1]) {
			teclado.debounceTime[FILA_1] = millis () + DEBOUNCE_TIME ;
			return;
		}

	teclado.teclaPulsada.row = FILA_1;
	teclado.teclaPulsada.col = teclado.columna_actual;

	piLock (KEYBOARD_KEY);
	teclado.flags |= FLAG_TECLA_PULSADA;
	piUnlock (KEYBOARD_KEY);

	teclado.debounceTime[FILA_1] = millis () + DEBOUNCE_TIME ;
	// TERMINO
}

void teclado_fila_2_isr (void) {
	// EMPIEZA
	if (millis() < teclado.debounceTime[FILA_2]){
			teclado.debounceTime[FILA_2] = millis() + DEBOUNCE_TIME;
			return;
		}

	teclado.teclaPulsada.row = FILA_2;
	teclado.teclaPulsada.col = teclado.columna_actual;

	piLock(KEYBOARD_KEY);
	teclado.flags |= FLAG_TECLA_PULSADA;
	piUnlock(KEYBOARD_KEY);

	teclado.debounceTime[FILA_2] = millis() + DEBOUNCE_TIME;
	// TERMINO
}

void teclado_fila_3_isr (void) {
	// EMPIEZO
	if (millis() < teclado.debounceTime[FILA_3]){
			teclado.debounceTime[FILA_3] = millis() + DEBOUNCE_TIME;
			return;
		}

	teclado.teclaPulsada.row = FILA_3;
	teclado.teclaPulsada.col = teclado.columna_actual;

	piLock(KEYBOARD_KEY);
	teclado.flags |= FLAG_TECLA_PULSADA;
	piUnlock(KEYBOARD_KEY);

	teclado.debounceTime[FILA_3] = millis() + DEBOUNCE_TIME;
	// TERMINO
}

void teclado_fila_4_isr (void) {
	// EMPIEZO
	if (millis() < teclado.debounceTime[FILA_4]){
			teclado.debounceTime[FILA_4] = millis() + DEBOUNCE_TIME;
			return;
		}

	teclado.teclaPulsada.row = FILA_4;
	teclado.teclaPulsada.col = teclado.columna_actual;

	piLock(KEYBOARD_KEY);
	teclado.flags |= FLAG_TECLA_PULSADA;
	piUnlock(KEYBOARD_KEY);

	teclado.debounceTime[FILA_4] = millis() + DEBOUNCE_TIME;
	// TERMINO
}

void timer_duracion_columna_isr (union sigval value) {
	//EMPIEZA DERROS
	piLock(KEYBOARD_KEY);
	teclado.flags |= FLAG_TIMEOUT_COLUMNA_TECLADO;
	piUnlock(KEYBOARD_KEY);
	//TERMINA DERROS
}
