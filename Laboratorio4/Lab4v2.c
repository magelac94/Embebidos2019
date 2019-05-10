//#class auto 			// Change default storage class for local variables: on the stack
#define DEBUG			// activo para imprimir estado de conexion TCP
//#define NOANDA        //Activo si quiero probar el caso que no anda

/* uCOS configuration */
#define OS_MAX_TASKS			2  		// Maximum number of tasks system can create (less stat and idle tasks)
#define OS_TASK_SUSPEND_EN		1		// Habilitar suspender y resumir tareas
#define OS_TIME_DLY_HMSM_EN		1		// Habilitar la funcion de delay para pasar fecha y hora
#define STACK_CNT_256			2		// Led_Red + idle
#define STACK_CNT_512			2		// main()
#define STACK_CNT_2K			1		// 1 Tarea TCP

/* TCP/IP configuration */
#define TCPCONFIG 0
#define USE_ETHERNET		1
#define MY_IP_ADDRESS "10.10.0.10"
#define MY_NETMASK "255.255.255.0"
#define PORT 7
#define MY_GATEWAY "10.10.0.1"
#define TAMANIO_BUFFER_LE 512        // Este es el tamanio que le damos a nuestros buffers para leer y enviar al socket
//#define TCP_MODE_ASCII 1
/* Incluimos las librerias luego de los define para sobre escribir los macros deseados */
#use IO.LIB
#use LED.LIB
#use EVENTOS.LIB
#use UTILITIES.LIB
#use RTC.lib


#memmap xmem
#use "ucos2.lib"
#use "dcrtcp.lib"
#use MENU.LIB



// Tarea 1 - Prende y apaga el led. Cuando espera se suspende.
// Por eso le daremos una prioridad alta. Es nuestro test para ver que todo corre.
// Le asignaremos un stack de 256.
void Led_Red(void* pdata){

	while(1){
		LED_RED_SET();
		OSTimeDlySec(1);
		LED_RED_RESET();
		OSTimeDlySec(1);
	}
}

// Tarea 2 - Menu por consola
void interfaz_consola(void* pdata){
	Event unEvento;
	struct tm FechaHora;
	char opcion_menu;
	char aux[2];
	char buffer[50];
	int int_opcion_menu, int_id_evento, int_Analog_Value;
    enum tipoUI interfazAUsar;
    interfazAUsar = *(int*)pdata;
    printf("INTERFAZ A USAR POR CONSOLA LEEE %d\n",interfazAUsar );

//    // Defino Array de Eventos
//    Event eventos[MAX_EVENTS];		// Lista de Eventos
//    EVENTOS_Eventos_init(&eventos);   // Inicializo array de eventos para cada llamada a programa principal, son n instancias, son n arrays;

    while(1){ // LOOP menu

    	// Muestra Menu y pide una opcion
		MENU_mostrarMenuPrincipal( CONSOLA, buffer, NULL );
		MENU_obtenerOpcion( CONSOLA, &opcion_menu, NULL );
		int_opcion_menu = atoi( &opcion_menu );

		// Evaluacion de la Opcion
		switch( int_opcion_menu ){
			case( OPCION_1 ):
				// CONFIGURAR FECHA HORA
				MENU_pedirFechaHora( &FechaHora, CONSOLA, NULL, buffer );
				RTC_fijarFechaHora( &FechaHora );
				MENU_printFechaHora( &FechaHora, CONSOLA );		// Imprimo la Fecha y hora modificadoS

				break;

			case( OPCION_2 ):
				// CONSULTAR FECHA HORA ACTUAL
				MENU_consultarHora( CONSOLA, NULL );
				RTC_leerFechaHora( &FechaHora );	// Leo el RTC
				MENU_printFechaHora( &FechaHora, CONSOLA ); // Imprimo la Fecha y hora
				break;

			case( OPCION_3 ):
				// AGREGAR EVENTO
				MENU_pedirDatosEvento( &unEvento, &FechaHora, CONSOLA, NULL, buffer );
			//	EVENTOS_agregarEvento( &eventos, &unEvento ); // ver el tema del array de eventos, lo agregue en el mail lo saque del Eventos para poder manejar diferentes array , 1 por instancia, a no ser que sea todo el emis
				break;
			case ( OPCION_4 ):
				// ELIMINAR EVENTO SEGUN EL NUMERO DE EVENTO (ES DE 1 EN adelante segun posicion en el array))
				int_id_evento = MENU_eliminarEvento( CONSOLA, NULL, buffer );
				printf("%d\n", int_id_evento);
			//	EVENTOS_eliminarEvento( int_id_evento );  // revisar despues de agregarEvento
				break;
			case( OPCION_5 ):
				// CONSULTAR EVENTO
				MENU_consultarEventos( CONSOLA , NULL);
				EVENTOS_listarEventos();
				break;

			case( OPCION_6 ):
				// CONSULTA ANALOGICA
				MENU_pedirEntradaAnalogica( CONSOLA, aux, NULL, buffer );
				int_opcion_menu = atoi(aux);
				int_Analog_Value = IO_getAnalogInput( int_opcion_menu ); //el valor que toma por parametro es un unsigned char
				MENU_mostrarEntradaAnalogica( CONSOLA, &int_Analog_Value, NULL );
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

		//OSTaskSuspend(OS_PRIO_SELF);
		}
}

// Tarea 3 - Menu por TCP
/* Esta tarea escucha por una conexion entrante por el puerto 7.
Cuando hay una conexion activa, muestra el menu por la interfaz tcp.
Debemos asignarle un stack de 2K por el socket tcp.
*/
void interfaz_tcp(void* pdata){
	struct tm FechaHora;
	char buffer_recepcion[TAMANIO_BUFFER_LE], buffer_envio[TAMANIO_BUFFER_LE];
	static tcp_Socket un_tcp_socket;
	auto int bytes_leidos, bytes_enviados, i, int_opcion_menu;

	while(1) {
		// Ponemos el socket en estado de escucha
		tcp_listen(&un_tcp_socket, 7, 0, 0, NULL, 0);	// Ponemos a escuchar el puerto

		// Inicializamos los buffers para sacar la basura
		for ( i = 0; i < TAMANIO_BUFFER_LE; i++ ){
			buffer_recepcion[i] = ' ';
			buffer_envio[i] = ' ';
		}

      // Le damos un tiempo para que el socket quede pronto
		OSTimeDlySec(5);
#ifdef DEBUG
		printf("\nDEBUG: Escuchando en puerto 7 por conexiones\n");
#endif
		// Esperamos por la conexion y si no esta establecida y pronta, suspende por 500ms.
		while( !sock_established(&un_tcp_socket) && sock_bytesready(&un_tcp_socket)==-1 ) {
			tcp_tick(NULL);
			OSTimeDlyHMSM(0,0,0,500);
		}

		// Un delay para que se establezca la conexion
		OSTimeDlyHMSM(0,0,0,500);
#ifdef DEBUG
		printf("\nDEBUG: Conexion establecida\n");
#endif

		do {

			// Se muestra menu y se espera opcion
			MENU_mostrarMenuPrincipal( TCP, buffer_envio, &un_tcp_socket );
			MENU_obtenerOpcion( TCP, buffer_recepcion, &un_tcp_socket );


         	int_opcion_menu = atoi( buffer_recepcion );
         	printf("%u\n", int_opcion_menu );

         	switch( int_opcion_menu ){
				case( OPCION_1 ):
					// CONFIGURAR FECHA HORA
					printf("\nOPCION 1\n");
					MENU_pedirFechaHora( &FechaHora, TCP, &un_tcp_socket, buffer_recepcion );
					//RTC_fijarFechaHora( &FechaHora );
					break;
				default:
					// OPCION INCORRECTA
					printf("\nOpcion DEFAULT: %d \n", int_opcion_menu);
					printf("Vuelva a ingresar\n");
			}







		} while(tcp_tick(&un_tcp_socket));
#ifdef DEBUG
		printf("\nDEBUG: Conexion finalizada....\n");
#endif

	}
}


main(){

	auto INT8U Error;

	// Inicializa el hardware de la placa
	HW_init();

	// Inicializa la estructura de datos interna del sistema operativo uC/OS-II
	OSInit();

	// Iniciamos el stack TCP/IP
#ifdef DEBUG
	printf("\nDEBUG: Iniciando Sockets\n");
#endif
	sock_init();
#ifdef DEBUG
	printf("\nDEBUG: Socket Iniciados\n");
#endif

	// Habilita encolado de SYN en puerto 7
	tcp_reserveport(7);

	// Deshabilitamos el scheduling mientras se crean las tareas
	OSSchedLock();

	//Creacion de tareas
	Error = OSTaskCreate(Led_Red, NULL, 256, 3 );
	Error = OSTaskCreate(interfaz_tcp, NULL, 2048, 4 );
	Error = OSTaskCreate(interfaz_consola,NULL,512, 5);

	// Re-habilitamos scheduling
	OSSchedUnlock();

	// Iniciamos el sistema operativo comenzando por la tarea en estado "ready" de nayor prioridad
	OSStart();
}