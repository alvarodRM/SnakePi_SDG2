
#include "ledDisplay.h"

tipo_pantalla pantalla_inicial = {
	.matriz = {
	{0,0,0,0,0,0,0},
	{0,1,1,0,1,0,0},
	{0,1,1,0,0,1,0},
	{0,0,0,0,0,1,0},
	{0,0,0,0,0,1,0},
	{0,1,1,0,0,1,0},
	{0,1,1,0,1,0,0},
	{0,0,0,0,0,0,0},
	}
};

tipo_pantalla pantalla_final = {
	.matriz = {
	{0,0,0,0,0,0,0},
	{0,0,1,0,0,1,0},
	{0,1,1,0,1,0,0},
	{0,0,0,0,1,0,0},
	{0,0,0,0,1,0,0},
	{0,1,1,0,1,0,0},
	{0,0,1,0,0,1,0},
	{0,0,0,0,0,0,0},
	}
};

// Maquina de estados: lista de transiciones
// {EstadoOrigen, CondicionDeDisparo, EstadoFinal, AccionesSiTransicion }
fsm_trans_t fsm_trans_excitacion_display[] = {
	{ DISPLAY_ESPERA_COLUMNA, CompruebaTimeoutColumnaDisplay, DISPLAY_ESPERA_COLUMNA, ActualizaExcitacionDisplay },
	{-1, NULL, -1, NULL },
};

//------------------------------------------------------
// PROCEDIMIENTOS DE INICIALIZACION DE LOS OBJETOS ESPECIFICOS
//------------------------------------------------------

void InicializaLedDisplay (TipoLedDisplay *led_display) {
		//EMPIEZA
			led_display->flags=0;
			led_display->columna_actual=-1;
			//Inicializamos wiringPi
			//wiringPiSetupGpio();


			//EMPIEZA FILAS
			pinMode(GPIO_LED_DISPLAY_ROW_1, OUTPUT);
				digitalWrite (GPIO_LED_DISPLAY_ROW_1, HIGH);

			pinMode(GPIO_LED_DISPLAY_ROW_2, OUTPUT);
				digitalWrite (GPIO_LED_DISPLAY_ROW_2, HIGH);

			pinMode(GPIO_LED_DISPLAY_ROW_3, OUTPUT);
				digitalWrite (GPIO_LED_DISPLAY_ROW_3, HIGH);

			pinMode(GPIO_LED_DISPLAY_ROW_4, OUTPUT);
				digitalWrite (GPIO_LED_DISPLAY_ROW_4, HIGH);

			pinMode(GPIO_LED_DISPLAY_ROW_5, OUTPUT);
				digitalWrite (GPIO_LED_DISPLAY_ROW_5, HIGH);

			pinMode(GPIO_LED_DISPLAY_ROW_6, OUTPUT);
				digitalWrite (GPIO_LED_DISPLAY_ROW_6, HIGH);

			pinMode(GPIO_LED_DISPLAY_ROW_7, OUTPUT);
				digitalWrite (GPIO_LED_DISPLAY_ROW_7, HIGH);

			//TERMINA FILAS

			//EMPIEZA COLUMNAS
			pinMode(GPIO_LED_DISPLAY_COL_1, OUTPUT);
				digitalWrite (GPIO_LED_DISPLAY_COL_1, LOW);

			pinMode(GPIO_LED_DISPLAY_COL_2, OUTPUT);
				digitalWrite (GPIO_LED_DISPLAY_COL_2, LOW);

			pinMode(GPIO_LED_DISPLAY_COL_3, OUTPUT);
				digitalWrite (GPIO_LED_DISPLAY_COL_3, LOW);

			/*pinMode(GPIO_LED_DISPLAY_COL_4, OUTPUT);
				digitalWrite (GPIO_LED_DISPLAY_COL_4, LOW);*/
			//TERMINA COLUMNAS


			led_display->tmr_refresco_display = tmr_new (timer_refresco_display_isr);
			tmr_startms((tmr_t*)(led_display->tmr_refresco_display), TIMEOUT_COLUMNA_DISPLAY);


			printf("\nSistema completado, ledDisplay listo\n");
			fflush(stdout);

		//TERMINO
}

//------------------------------------------------------
// OTROS PROCEDIMIENTOS PROPIOS DE LA LIBRERIA
//------------------------------------------------------

void ApagaFilas (TipoLedDisplay *led_display){
int i;
	for(i=0;i<NUM_FILAS_DISPLAY;i++) {
		digitalWrite(led_display->filas[i], HIGH);
	}

}

void ExcitaColumnas(int columna) {

	switch(columna) {
			//EMPIEZA DERROS
				case 0:
					//led_display->columna_actual=0;
					digitalWrite (GPIO_LED_DISPLAY_COL_1, LOW);
					digitalWrite (GPIO_LED_DISPLAY_COL_2, LOW);
					digitalWrite (GPIO_LED_DISPLAY_COL_3, LOW);
					break;
				case 1:
					//led_display->columna_actual=1;
					digitalWrite (GPIO_LED_DISPLAY_COL_1, HIGH);
					digitalWrite (GPIO_LED_DISPLAY_COL_2, LOW);
					digitalWrite (GPIO_LED_DISPLAY_COL_3, LOW);
					break;
				case 2:
					//led_display->columna_actual=2;
					digitalWrite (GPIO_LED_DISPLAY_COL_1, LOW);
					digitalWrite (GPIO_LED_DISPLAY_COL_2, HIGH);
					digitalWrite (GPIO_LED_DISPLAY_COL_3, LOW);
					break;
				case 3:
					//led_display->columna_actual=3;
					digitalWrite (GPIO_LED_DISPLAY_COL_1, HIGH);
					digitalWrite (GPIO_LED_DISPLAY_COL_2, HIGH);
					digitalWrite (GPIO_LED_DISPLAY_COL_3, LOW);
					break;
				case 4:
					//led_display->columna_actual=4;
					digitalWrite (GPIO_LED_DISPLAY_COL_1, LOW);
					digitalWrite (GPIO_LED_DISPLAY_COL_2, LOW);
					digitalWrite (GPIO_LED_DISPLAY_COL_3, HIGH);
					break;
				case 5:
					//led_display->columna_actual=5;
					digitalWrite (GPIO_LED_DISPLAY_COL_1, HIGH);
					digitalWrite (GPIO_LED_DISPLAY_COL_2, LOW);
					digitalWrite (GPIO_LED_DISPLAY_COL_3, HIGH);
					break;
				case 6:
					//led_display->columna_actual=6;
					digitalWrite (GPIO_LED_DISPLAY_COL_1, LOW);
					digitalWrite (GPIO_LED_DISPLAY_COL_2, HIGH);
					digitalWrite (GPIO_LED_DISPLAY_COL_3, HIGH);
					break;
				case 7:
					//led_display->columna_actual=7;
					digitalWrite (GPIO_LED_DISPLAY_COL_1, HIGH);
					digitalWrite (GPIO_LED_DISPLAY_COL_2, HIGH);
					digitalWrite (GPIO_LED_DISPLAY_COL_3, HIGH);
					break;
				default:
					break;
			}
		//TERMINA DERROS
}

void ActualizaLedDisplay (TipoLedDisplay *led_display) {
	//EMPIEZA DERROS


	led_display->columna_actual+=1;
	if(led_display->columna_actual >= NUM_COLUMNAS_DISPLAY){
		led_display->columna_actual = 0;
	}
	ExcitaColumnas(led_display->columna_actual);

	int i=0;

	for(i=0; i<NUM_FILAS_DISPLAY; i++){
			if(led_display->pantalla.matriz[led_display->columna_actual][i]== 1){
				digitalWrite(led_display->filas[i], LOW);
			}else{
				digitalWrite(led_display->filas[i], HIGH);
			}

	}
	//led_display->tmr_refresco_display = tmr_new (timer_refresco_display_isr);
	//tmr_startms((tmr_t*)(led_display->tmr_refresco_display), TIMEOUT_COLUMNA_DISPLAY);
	//piUnlock(MATRIX_KEY);
	//TERMINA DERROS
}

void PintaPantallaPorTerminal (tipo_pantalla *p_pantalla) {
#ifdef _SIN_PSEUDOWIRINGPI__
	int i=0, j=0;

	printf("\n[PANTALLA]\n");
	fflush(stdout);
	for(j=0;j<NUM_FILAS_DISPLAY;j++) {
		for(i=0;i<NUM_COLUMNAS_DISPLAY;i++) {
			printf("%d", p_pantalla->matriz[i][j]);
			fflush(stdout);
		}
		printf("\n");
		fflush(stdout);
	}
	fflush(stdout);
#endif
}

//------------------------------------------------------
// FUNCIONES DE ENTRADA O DE TRANSICION DE LA MAQUINA DE ESTADOS
//------------------------------------------------------

int CompruebaTimeoutColumnaDisplay (fsm_t* this) {
	int result = 0;
	//TipoLedDisplay *p_ledDisplay;
	//p_ledDisplay = (TipoLedDisplay*)(this->user_data);

	piLock(MATRIX_KEY);
	result = led_display.flags & FLAG_TIMEOUT_COLUMNA_DISPLAY;
	piUnlock(MATRIX_KEY);

	return result;
}

//------------------------------------------------------
// FUNCIONES DE SALIDA O DE ACCION DE LA MAQUINA DE ESTADOS
//------------------------------------------------------

void ActualizaExcitacionDisplay (fsm_t* this) {
	TipoLedDisplay *p_ledDisplay;
	p_ledDisplay = (TipoLedDisplay*)(this->user_data);

	piLock(MATRIX_KEY);
	p_ledDisplay->flags &= (~FLAG_TIMEOUT_COLUMNA_DISPLAY);
	piUnlock(MATRIX_KEY);


	// Llamada a ActualizaExcitacionTecladoGPIO con columna a activar como argumento
	//ExcitaColumnas(p_ledDisplay->columna_actual);

	ActualizaLedDisplay(p_ledDisplay);
	tmr_startms((tmr_t*)(p_ledDisplay->tmr_refresco_display), TIMEOUT_COLUMNA_DISPLAY);

	//p_ledDisplay->tmr_refresco_display = tmr_new (timer_refresco_display_isr);

}

//------------------------------------------------------
// SUBRUTINAS DE ATENCION A LAS INTERRUPCIONES
//------------------------------------------------------

void timer_refresco_display_isr (union sigval value) {
	piLock(MATRIX_KEY);
	led_display.flags |= FLAG_TIMEOUT_COLUMNA_DISPLAY;
	piUnlock(MATRIX_KEY);
}
