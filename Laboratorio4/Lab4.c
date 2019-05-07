//#class auto 			// Change default storage class for local variables: on the stack

/* uCOS configuration */
#define OS_MAX_TASKS			6  		// Maximum number of tasks system can create (less stat and idle tasks)
#define OS_SEM_EN				1		// Enable semaphores
#define OS_TIME_DLY_HMSM_EN		1		// Habilitar la funcion de delay para pasar fecha y hora
#define OS_TASK_SUSPEND_EN		1		// Enable task suspend and resume
#define OS_MAX_EVENTS			8		// MAX_TCP_SOCKET_BUFFERS + 2 + 1 (1 semaphore is used in this app)
#define STACK_CNT_256			3
#define STACK_CNT_512			3		// LED blink task + main()
#define STACK_CNT_2K         	5		// TCP/IP needs a 2K stack
#define MAX_TCP_SOCKET_BUFFERS	5

// #define OS_TASK_DEL_EN 1
// #define MAX_EVENTS 16


#use IO.LIB
#use LED.LIB

#use EVENTOS.LIB
#use UTILITIES.LIB
#use RTC.lib

#memmap xmem
#use ucos2.LIB
//#use TCPCON.LIB
#use TCP_ALL.LIB
#use MENU.LIB



// Tarea para prender el led que indica que todo esta funcionando sin bloqueos
void Led_Red(){

	while(1){
		LED_RED_SET();
		OSTimeDlySec(1);
		LED_RED_RESET();
		OSTimeDlySec(1);
   	}
}

/*
// Tarea principal que interactua con el usuario por CONSOLA o TCP
void ProgramaPrincipal(void* pdata){
	Event unEvento;
	struct tm FechaHora;
	char opcion_menu;
	char aux[2];
	int int_opcion_menu, int_id_evento, int_Analog_Value;
    enum tipoUI interfazAUsar;
    interfazAUsar = *(int*)pdata;
    printf("INTERFAZ A USAR POR CONSOLA LEEE %d\n",interfazAUsar );

//    // Defino Array de Eventos
//    Event eventos[MAX_EVENTS];		// Lista de Eventos
//    EVENTOS_Eventos_init(&eventos);   // Inicializo array de eventos para cada llamada a programa principal, son n instancias, son n arrays;

    while(1){ // LOOP menu

    	// Muestra Menu y pide una opcion
		MENU_mostrarMenuPrincipal( interfazAUsar );
		MENU_obtenerOpcion( interfazAUsar, &opcion_menu );
		int_opcion_menu = atoi( &opcion_menu );

		// Evaluacion de la Opcion
		switch( int_opcion_menu ){
			case( OPCION_1 ):
				// CONFIGURAR FECHA HORA
				MENU_pedirFechaHora( &FechaHora, interfazAUsar );
				RTC_fijarFechaHora( &FechaHora );
				MENU_printFechaHora( &FechaHora, interfazAUsar );		// Imprimo la Fecha y hora modificadoS

				break;

			case( OPCION_2 ):
				// CONSULTAR FECHA HORA ACTUAL
				MENU_consultarHora( interfazAUsar );
				RTC_leerFechaHora( &FechaHora );	// Leo el RTC
				MENU_printFechaHora( &FechaHora, interfazAUsar ); // Imprimo la Fecha y hora
				break;

			case( OPCION_3 ):
				// AGREGAR EVENTO
				MENU_pedirDatosEvento( &unEvento, &FechaHora, interfazAUsar );
			//	EVENTOS_agregarEvento( &eventos, &unEvento ); // ver el tema del array de eventos, lo agregue en el mail lo saque del Eventos para poder manejar diferentes array , 1 por instancia, a no ser que sea todo el emis
				break;
			case ( OPCION_4 ):
				// ELIMINAR EVENTO SEGUN EL NUMERO DE EVENTO (ES DE 1 EN adelante segun posicion en el array))
				int_id_evento = MENU_eliminarEvento( interfazAUsar );
				printf("%d\n", int_id_evento);
			//	EVENTOS_eliminarEvento( int_id_evento );  // revisar despues de agregarEvento
				break;
			case( OPCION_5 ):
				// CONSULTAR EVENTO
				MENU_consultarEventos( interfazAUsar );
				EVENTOS_listarEventos();
				break;

			case( OPCION_6 ):
				// CONSULTA ANALOGICA
				MENU_pedirEntradaAnalogica( interfazAUsar, aux );
				int_opcion_menu = atoi(aux);
				int_Analog_Value = IO_getAnalogInput( int_opcion_menu ); //el valor que toma por parametro es un unsigned char
				MENU_mostrarEntradaAnalogica( interfazAUsar, &int_Analog_Value );
				break;
			case( OPCION_7 ):
				// SALIR
				printf("Salir");
				break;


			default:
				// OPCION INCORRECTA
				printf("\nOpcion DEFAULT: %d \n", int_opcion_menu);
				printf("Vuelva a ingresar\n");
			}

		OSTaskSuspend(OS_PRIO_SELF);
		}
  }

void Tarea3(void* pdata){
	char aux[2];
	while(1){
		printf("esta es la tercer tarea");
      gets(aux);

      	OSTaskResume(5);
        OSTaskResume(6);
	}
}
*/



main(){

	int consola;
	int tcp;
	int i;
	consola = CONSOLA;
	tcp = TCP;

	HW_init();
	OSInit();

	printf("Iniciando Socket\n");
    sock_init();
	printf("Socket Iniciado\n");

	tcp_reserveport(7);// enable SYN queuing on port 7.


	// Tarea 1 Prende LED
	OSTaskCreate(Led_Red, NULL, 256, 3);

	
	// Tarea 2 Mostrar Menu Para Consola
	//OSTaskCreate(ProgramaPrincipal , &consola, 512, 4); // ANDA OK

 // OSTaskCreate(Tarea3 , NULL, 512, 7); // ok

	
   // Tarea 3 TCP
	OSTaskCreate(TareaTCP, NULL, 2048, 4);


/*
	// Se crean N tareas para Mostrar Menu Por TCP
	for ( i=7 ; i<15; i++){                   //  // ver y agregar todas las funciones de tcp
		OSTaskCreate(TCPCON_conexion, NULL, 512, i);
		OSTaskCreate(ProgramaPrincipal, &tcp, 512, 6);

	}






//	OSTaskCreate(TCPCON_conexion, NULL, 512, 5);

//	OSTaskCreate(ProgramaPrincipal, &tcp, 512, 6);
*/

	OSStart();
}