#define STACK_CNT_256 3
#define STACK_CNT_512 4
#define OS_TASK_DEL_EN 1

#define SESIONES_TCP 5

//#define MAX_EVENTS 16


#use IO.LIB
#use LED.LIB
#use TCPCON.LIB
#use EVENTOS.LIB
#use UTILITIES.LIB
#use RTC.lib
#use MENU.LIB
#memmap xmem
#use ucos2.LIB
#use "dcrtcp.lib"

void Led_Red(){

	while(1){
		LED_RED_SET();
		OSTimeDlySec(1);
		LED_RED_RESET();
		OSTimeDlySec(1);
   	}
}


void ProgramaPrincipal(void* pdata){
	Event unEvento;
	struct tm FechaHora;
	char opcion_menu;
	char aux[2];
	int int_opcion_menu, int_id_evento, int_Analog_Value;
    enum tipoUI interfazAUsar;
    interfazAUsar = *(int*)pdata;

    // Defino Array de Eventos
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

		}
  }




main(){

	int consola;
	int tcp;
	int i;

	HW_init();
	OSInit();

	printf("Iniciando Socket\n");
    sock_init();
	printf("Socket Iniciado\n");

	tcp_reserveport(7);// enable SYN queuing on port 7.


	// Tarea 1 Prende LED
	OSTaskCreate(Led_Red, NULL, 256, 4);

	consola = CONSOLA;
	// Tarea 2 Mostrar Menu Para Consola
	OSTaskCreate(ProgramaPrincipal , &consola, 512, 3);


	tcp = TCP;

	OSTaskCreate(TCPCON_conexion, NULL, 512, 5);


	// Se crean N tareas para Mostrar Menu Por TCP
	for ( i=7 ; i<15; i++){                   //  // ver y agregar todas las funciones de tcp
		OSTaskCreate(TCPCON_conexion, NULL, 512, i);
		OSTaskCreate(ProgramaPrincipal, &tcp, 512, 6);

	}





//	OSTaskCreate(TCPCON_conexion, NULL, 512, 5);

//	OSTaskCreate(ProgramaPrincipal, &tcp, 512, 6);


	OSStart();
}