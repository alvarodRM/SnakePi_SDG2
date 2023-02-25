
#include "snakePi.h"

int flags = 0;

// Declaracion del objeto teclado
TipoTeclado teclado = {
	.columnas = {
		// EMPIEZA
		GPIO_KEYBOARD_COL_1,
		GPIO_KEYBOARD_COL_2,
		GPIO_KEYBOARD_COL_3,
		GPIO_KEYBOARD_COL_4
		//TERMINA
	},
	.filas = {
		// EMPIEZA
			GPIO_KEYBOARD_ROW_1,
			GPIO_KEYBOARD_ROW_2,
			GPIO_KEYBOARD_ROW_3,
			GPIO_KEYBOARD_ROW_4
		// TERMINA
	},
	.rutinas_ISR = {
		// EMPIEZA
			teclado_fila_1_isr,
			teclado_fila_2_isr,
			teclado_fila_3_isr,
			teclado_fila_4_isr
		// TERMINA
	},
	.teclaPulsada = {-1,-1},
	.debounceTime = {0,0,0,0},
};

// Declaracion del objeto display

TipoLedDisplay led_display = {
	.columnas = {
		//EMPIEZA
			GPIO_LED_DISPLAY_COL_1,
			GPIO_LED_DISPLAY_COL_2,
			GPIO_LED_DISPLAY_COL_3
			//GPIO_LED_DISPLAY_COL_4
		//TERMINA
	},
	.filas = {
		//EMPIEZA
			GPIO_LED_DISPLAY_ROW_1,
			GPIO_LED_DISPLAY_ROW_2,
			GPIO_LED_DISPLAY_ROW_3,
			GPIO_LED_DISPLAY_ROW_4,
			GPIO_LED_DISPLAY_ROW_5,
			GPIO_LED_DISPLAY_ROW_6,
			GPIO_LED_DISPLAY_ROW_7
		//TERMINA
	},
	.flags=0,
	.columna_actual=-1
};


//------------------------------------------------------
// FUNCIONES DE CONFIGURACION/INICIALIZACION
//------------------------------------------------------

// int ConfiguracionSistema (TipoSistema *p_sistema): procedimiento de configuracion
// e inicializacion del sistema.
// Realizará, entra otras, todas las operaciones necesarias para:
// configurar el uso de posibles librerías (e.g. Wiring Pi),
// configurar las interrupciones externas asociadas a los pines GPIO,
// configurar las interrupciones periódicas y sus correspondientes temporizadores,
// la inicializacion de los diferentes elementos de los que consta nuestro sistema,
// crear, si fuese necesario, los threads adicionales que pueda requerir el sistema
// como el thread de exploración del teclado del PC


int ConfiguraInicializaSistema (TipoSistema *p_sistema) {
	//int result = 0;

	piLock(STD_IO_BUFFER_KEY);
	// sets up the wiringPi library
	if (wiringPiSetupGpio () < 0) {
		printf ("Unable to setup wiringPi\n");
		piUnlock(STD_IO_BUFFER_KEY);
		return -1;
	}

	InicializaTeclado(&(teclado));
	InicializaLedDisplay(&(led_display));

	printf("Configurando e inicializando el sistema...\n");
	fflush(stdout);

	// Lanzamos thread para exploracion del teclado convencional del PC
	/*result = piThreadCreate (thread_explora_teclado_PC);

	if (result != 0) {
		printf ("Thread didn't start!!!\n");
		piUnlock(STD_IO_BUFFER_KEY);
		return -1;
	}*/

	printf("Sistema listo...\n");
	printf("\n");
	printf("MEJORAS añadidas:\n");
	printf("1. Informar al usuario del número de manzanas conseguido al finalizar el juego\n");
	printf("2. Juego con velocidad incremental, la velocidad del movimiento va aumentando a medida que vamos consiguiendo manzanas, a las 5, 10 y 20 manzanas aumenta velocidad (4 tipos de velocidad)\n");
	printf("3. Se puede elegir antes de empezar si quieres jugar con 1 manzana o con 2 manzanas a la vez en la pantalla\n");
	printf("\n");
	printf("\nSELECCIONE NÚMERO DE MANZANAS Y PULSE ENTER (MÍNIMO 1- MÁXIMO 2)\n"); //Mejora manzanas
	scanf("%s", numero); //Mejora manzanas
	if(validarNumero(numero)==1 || validarNumero(numero)==2){
		if(validarNumero(numero)==1){
			numManzanas=1;
		}else{
			numManzanas=2;
		}
	printf("\n");
	printf("CONTROLES del teclado ordenador versiones 1 y 2:\n");
	printf("S ARRANCAR JUEGO, W ARRIBA, X ABAJO, A IZQUIERDA, D DERECHA, Q SALIR JUEGO\n");
	printf("\n");
	printf("CONTROLES del TECLADO_TL04 version 3:\n");
	printf("4 ARRANCAR JUEGO, 2 ARRIBA, W ABAJO, 1 IZQUIERDA, 3 DERECHA, 5 SALIR JUEGO\n");
	printf("\n");
	fflush(stdout);
	piUnlock(STD_IO_BUFFER_KEY);
	return 1;
	}else{
		while(validarNumero(numero)==0 || validarNumero(numero)==3){
		printf("\nERROR NUMERO DE MANZANAS NO VALIDO, SELECCIONE DE NUEVO 1 o 2 Y PULSE ENTER\n");
		scanf("%s",numero);
		}
		if(validarNumero(numero)==1){
			numManzanas=1;
		}else{
			numManzanas=2;
		}
		printf("\n");
		printf("CONTROLES del teclado ordenador versiones 1 y 2:\n");
		printf("S ARRANCAR JUEGO, W ARRIBA, X ABAJO, A IZQUIERDA, D DERECHA, Q SALIR JUEGO\n");
		printf("\n");
		printf("CONTROLES del TECLADO_TL04 version 3:\n");
		printf("4 ARRANCAR JUEGO, 2 ARRIBA, W ABAJO, 1 IZQUIERDA, 3 DERECHA, 5 SALIR JUEGO\n");
		printf("\n");
		fflush(stdout);
		piUnlock(STD_IO_BUFFER_KEY);
		return 1;
	}
}
int validarNumero(char numero[]){
	int i;
	int j = 0;
	int z = 0;
	int h = 0;
	for (i=0; i<strlen(numero); i++){
		h++;
	}
	if(h>1){
		return 3;
	}
	for(i=0; i<strlen(numero); i++){
		if(!((numero[i])==49 || (numero[i]==50))){
			return 0;
		}
	}
	for(i=0; i<strlen(numero); i++){
		if((numero[i])==49){
			j++;
		}
	}
	if(j==1){
		return 1;
	}
	for(i=0; i<strlen(numero); i++){
		if((numero[i])==50){
			z++;
		}
	}
	if(z==1){
		return 2;
	}
	return 3;
}

//INICIO RUTINA TEMPORIZACION
void temporizador_isr(union sigval value){
	piLock(SYSTEM_FLAGS_KEY);
	flags |= FLAG_TIMER_JUEGO;
	piUnlock(SYSTEM_FLAGS_KEY);
}
//FINALIZACION RUTINA TEMPORIZACION


//------------------------------------------------------
// FUNCIONES LIGADAS A THREADS ADICIONALES
//------------------------------------------------------

PI_THREAD (thread_explora_teclado_PC) {
	int teclaPulsada;

	while(1) {
		delay(10); // Wiring Pi function: pauses program execution for at least 10 ms

		piLock (STD_IO_BUFFER_KEY);

		if(kbhit()) {

			teclaPulsada = kbread();

			switch(teclaPulsada) {
				// A completar por el alumno...
				// ...
			case 's':
				//EMPIEZA
				piLock(SYSTEM_FLAGS_KEY);
				flags |= FLAG_BOTON;
				piUnlock(SYSTEM_FLAGS_KEY);
				//TERMINA
				printf("\n¡Tecla S pulsada! \n");
				fflush(stdout);
				break;
			case 'q':
				piLock(SYSTEM_FLAGS_KEY);
				flags |= FLAG_BOTON;
				piUnlock(SYSTEM_FLAGS_KEY);
				printf("\n¡SALIR DEL JUEGO! \n");
				exit(0);
				break;
				//EMPIEZA DERROS
			case 'w':
				piLock(SYSTEM_FLAGS_KEY);
				flags |= FLAG_MOV_ARRIBA;
				flags |= FLAG_BOTON;
				piUnlock(SYSTEM_FLAGS_KEY);
				printf("\n¡Tecla W pulsada! ARRIBA \n");
				fflush(stdout);
				break;
			case 'x':
				piLock(SYSTEM_FLAGS_KEY);
				flags |= FLAG_MOV_ABAJO;
				flags |= FLAG_BOTON;
				piUnlock(SYSTEM_FLAGS_KEY);
				printf("\n¡Tecla X pulsada! ABAJO \n");
				fflush(stdout);
				break;
			case 'a':
				piLock(SYSTEM_FLAGS_KEY);
				flags |= FLAG_MOV_IZQUIERDA;
				flags |= FLAG_BOTON;
				piUnlock(SYSTEM_FLAGS_KEY);
				printf("\n¡Tecla A pulsada! IZQUIERDA \n");
				fflush(stdout);
				break;
			case 'd':
				piLock(SYSTEM_FLAGS_KEY);
				flags |= FLAG_MOV_DERECHA;
				flags |= FLAG_BOTON;
				piUnlock(SYSTEM_FLAGS_KEY);
				printf("\n¡Tecla D pulsada! DERECHA \n");
				fflush(stdout);
				break;
			case 'z':
				piLock(SYSTEM_FLAGS_KEY);
				flags |= FLAG_BOTON;
				piUnlock(SYSTEM_FLAGS_KEY);
				break;
			//DERROS ACABA
			default:
				//printf("\n¡¡¡INVALID KEY!!!\n");
				break;
			}
		}
		piUnlock (STD_IO_BUFFER_KEY);
	}
}

// wait until next_activation (absolute time)
void delay_until (unsigned int next) {
	unsigned int now = millis();
	if (next > now) {
		delay (next - now);
	}
}

int main () {
	TipoSistema sistema;
	unsigned int next;

	fsm_trans_t snakePi[] = {
		{ WAIT_START, CompruebaBotonPulsado, WAIT_PUSH, InicializaJuego },
		{ WAIT_PUSH, CompruebaTimeoutActualizacionJuego, WAIT_PUSH, ActualizarJuego },
		{ WAIT_PUSH, CompruebaMovimientoArriba, WAIT_PUSH, MueveSerpienteArriba },
		{ WAIT_PUSH, CompruebaMovimientoAbajo, WAIT_PUSH, MueveSerpienteAbajo },
		{ WAIT_PUSH, CompruebaMovimientoIzquierda, WAIT_PUSH, MueveSerpienteIzquierda },
		{ WAIT_PUSH, CompruebaMovimientoDerecha, WAIT_PUSH, MueveSerpienteDerecha },
		{ WAIT_PUSH, CompruebaFinalJuego, WAIT_END, FinalJuego },
		{ WAIT_END, CompruebaBotonPulsado, WAIT_START, ReseteaJuego },
		{-1, NULL, -1, NULL },
	};

	// Configuracion e inicializacion del sistema

	printf ("\nComienzo\n");
	fflush(stdout);

		ConfiguraInicializaSistema(&sistema);

		//sistema.snakePi.p_pantalla = &(pantalla_inicial);
		sistema.snakePi.p_pantalla = &led_display.pantalla;
		sistema.snakePi.tmr_refresco_snake = tmr_new(temporizador_isr);

		fsm_t* snakePi_fsm = fsm_new (WAIT_START, snakePi, &(sistema.snakePi));
		fsm_t* TECLADO_TL04_ESPERA_COLUMNA_fsm = fsm_new (TECLADO_ESPERA_COLUMNA, fsm_trans_excitacion_columnas, &teclado);
		fsm_t* TECLADO_TL04_ESPERA_TECLA_fsm = fsm_new (TECLADO_ESPERA_TECLA, fsm_trans_deteccion_pulsaciones, &teclado);
		fsm_t* PANTALLA_LED_DISPLAY_fsm = fsm_new (DISPLAY_ESPERA_COLUMNA, fsm_trans_excitacion_display, &led_display);

	piLock(SYSTEM_FLAGS_KEY);
	flags = 0;
	piUnlock(SYSTEM_FLAGS_KEY);

	next = millis();
	while (1) {
		fsm_fire (snakePi_fsm);
		fsm_fire (TECLADO_TL04_ESPERA_COLUMNA_fsm);
		fsm_fire (TECLADO_TL04_ESPERA_TECLA_fsm);
		fsm_fire (PANTALLA_LED_DISPLAY_fsm);
		// A completar por el alumno...
		// ...
		next += CLK_MS;
		delay_until (next);
	}

	//EMPIEZA DESTROY DERROS
	tmr_destroy ((tmr_t*)(snakePi_fsm->user_data));

	fsm_destroy (snakePi_fsm);
	fsm_destroy (TECLADO_TL04_ESPERA_TECLA_fsm);
	fsm_destroy (TECLADO_TL04_ESPERA_COLUMNA_fsm);
	fsm_destroy (PANTALLA_LED_DISPLAY_fsm);

	//TERMINA DESTROY DERROS

	return 0;
}
