//#class auto 			// Change default storage class for local variables: on the stack
#define DEBUG			// activo para imprimir estado de conexion TCP
//#define NOANDA        //Activo si quiero probar el caso que no anda

/* uCOS configuration */
#define OS_MAX_TASKS			7  		// Maximum number of tasks system can create (less stat and idle tasks)
#define OS_TASK_SUSPEND_EN		1		// Habilitar suspender y resumir tareas
#define OS_TIME_DLY_HMSM_EN		1		// Habilitar la funcion de delay para pasar fecha y hora
#define OS_SEM_EN				1		// Habilitar semaforos
#define OS_SEM_QUERY_EN			1		// Enable semaphore querying
#define OS_MAX_EVENTS			9		// MAX_TCP_SOCKET_BUFFERS + 2 + 2 (2 semaforos son usados)
#define STACK_CNT_256			2		// Led_Red + idle
#define STACK_CNT_512			1		// main()
#define STACK_CNT_2K			5		// 5 Tareas TCP
#define STACK_CNT_4K			2		// 5 Tareas TCP

/* TCP/IP configuration */
#define TCPCONFIG 0
#define USE_ETHERNET		1
#define MY_IP_ADDRESS "10.10.0.10"
#define MY_NETMASK "255.255.255.0"
#define PORT 7
#define MY_GATEWAY "10.10.0.1"
#define MAX_TCP_SOCKET_BUFFERS	5					// Determina la cantidad maxima de sockets con buffer preasignado
#define TCP_BUF_SIZE			1024				// Tamanio del buffer TCP
#define TAMANIO_BUFFER_LE 		512      			// Este es el tamanio que le damos a nuestros buffers para leer y enviar al socket
#define PUERTO7					7
#define PUERTO8					8

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

/* Definimos punteros a los semaforos, colas y MBox que vayamos a usar*/
OS_EVENT* pSemaforoRTC;
OS_EVENT* pSemaforoAtoI;

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
	char buffer[TAMANIO_BUFFER_LE];
	int int_opcion_menu, int_id_evento, int_Analog_Value, i;
	auto INT8U Error;

	// Inicializamos buffer para sacar la basura
	for ( i = 0; i < TAMANIO_BUFFER_LE; i++ ){
			buffer[i] = ' ';
	}

    while(1){ // LOOP menu
    	// Muestra Menu y pide una opcion
		MENU_mostrarMenuPrincipal( CONSOLA, buffer, NULL );
		MENU_obtenerOpcion( CONSOLA, buffer, NULL );

		// Nos aseguramos que tenemos el recurso para nosotros
		OSSemPend(pSemaforoAtoI, 0, &Error);
		int_opcion_menu = atoi( buffer );
		// Devolvemos el recurso
		OSSemPost(pSemaforoAtoI);

		// Evaluacion de la Opcion
		switch( int_opcion_menu ){
			case( OPCION_1 ):
				// CONFIGURAR FECHA HORA
				MENU_pedirFechaHora( CONSOLA, &FechaHora, buffer, NULL );
				RTC_fijarFechaHora( &FechaHora );
				MENU_printFechaHora( CONSOLA, &FechaHora, buffer, NULL );		// Imprimo la Fecha y hora modificadoS
				break;

			case( OPCION_2 ):
				// CONSULTAR FECHA HORA ACTUAL
				MENU_consultarHora( CONSOLA, buffer, NULL );
				RTC_leerFechaHora( &FechaHora );	// Leo el RTC
				MENU_printFechaHora( CONSOLA, &FechaHora, buffer, NULL ); // Imprimo la Fecha y hora
				break;

			case( OPCION_3 ):
				// AGREGAR EVENTO
				MENU_pedirDatosEvento( CONSOLA, &unEvento, &FechaHora, buffer, NULL );
				EVENTOS_agregarEvento( &unEvento );
				break;
			case ( OPCION_4 ):
				// ELIMINAR EVENTO SEGUN EL NUMERO DE EVENTO (ES DE 1 EN adelante segun posicion en el array))
				int_id_evento = MENU_eliminarEvento( CONSOLA, buffer, NULL );
				printf("%d\n", int_id_evento);
			//	EVENTOS_eliminarEvento( int_id_evento );  // revisar despues de agregarEvento
				break;
			case( OPCION_5 ):
				// CONSULTAR EVENTO
				//MENU_consultarEventos( CONSOLA , NULL, buffer);
				MENU_printEvento( CONSOLA, buffer, NULL );
				break;

			case( OPCION_6 ):
				// CONSULTA ANALOGICA
				MENU_pedirEntradaAnalogica( CONSOLA, buffer, NULL );

				// Nos aseguramos que tenemos el recurso para nosotros
				OSSemPend(pSemaforoAtoI, 0, &Error);
				int_opcion_menu = atoi( buffer );
				// Devolvemos el recurso
				OSSemPost(pSemaforoAtoI);

				int_Analog_Value = IO_getAnalogInput( int_opcion_menu ); //el valor que toma por parametro es un unsigned char
				MENU_mostrarEntradaAnalogica( CONSOLA, &int_Analog_Value, buffer, NULL );
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



// Tarea 3 - Menu por TCP
/* Esta tarea escucha por una conexion entrante por el puerto 7.
Cuando hay una conexion activa, muestra el menu por la interfaz tcp.
Debemos asignarle un stack de 2K por el socket tcp.
*/
void interfaz_tcp(void* pdata){

	Event unEvento;
	struct tm FechaHora;
	char buffer[TAMANIO_BUFFER_LE];
	static tcp_Socket un_tcp_socket;
	auto int i, puerto, int_opcion_menu, int_id_evento, int_Analog_Value;
	auto INT8U Error;

	//puerto = *(int*)pdata;

	while(1) {
		// Ponemos el socket en estado de escucha
		if (!tcp_listen(&un_tcp_socket, puerto, 0, 0, NULL, 0)){
#ifdef DEBUG
		printf("\nDEBUG: Error al abrir el socket\n");
#endif
		}
		// Inicializamos buffer para sacar la basura
		for ( i = 0; i < TAMANIO_BUFFER_LE; i++ ){
			buffer[i] = ' ';
		}

      // Le damos un tiempo para que el socket quede pronto
		OSTimeDlyHMSM(0,0,0,500);
#ifdef DEBUG
		printf("\nDEBUG: Escuchando en puerto 7 por conexiones\n");
#endif
		// Esperamos por la conexion y si no esta establecida y pronta, suspende por 500ms.
		while( !sock_established(&un_tcp_socket) ) { // && sock_bytesready(&un_tcp_socket)==-1
			tcp_tick(&un_tcp_socket);
			OSTimeDlyHMSM(0,0,0,500);
		}
		// Un delay para que se establezca la conexion
		OSTimeDlyHMSM(0,0,0,500);

		if (sock_established(&un_tcp_socket)){
#ifdef DEBUG
			printf("\nDEBUG: Conexion establecida\n");
#endif
			sock_mode(&un_tcp_socket, TCP_MODE_ASCII);
			do {

				// Se muestra menu y se espera opcion
				MENU_mostrarMenuPrincipal( TCP, buffer, &un_tcp_socket );
				MENU_obtenerOpcion( TCP, buffer, &un_tcp_socket );

				// Nos aseguramos que tenemos el recurso para nosotros
				OSSemPend(pSemaforoAtoI, 0, &Error);
				int_opcion_menu = atoi( buffer );
				// Devolvemos el recurso
				OSSemPost(pSemaforoAtoI);

#ifdef DEBUG
				printf("\nDEBUG: MAIN: Opcion elegida: %u\n", int_opcion_menu );
#endif
				switch( int_opcion_menu ){
					case( OPCION_1 ):
						// CONFIGURAR FECHA HORA
						printf("\nOPCION 1\n");
						MENU_pedirFechaHora( TCP, &FechaHora, buffer, &un_tcp_socket );
						RTC_fijarFechaHora( &FechaHora );
						break;

					case( OPCION_2 ):
						// CONSULTAR FECHA HORA ACTUAL
						MENU_consultarHora( TCP, buffer, &un_tcp_socket );
						RTC_leerFechaHora( &FechaHora );	// Leo el RTC
						MENU_printFechaHora( TCP, &FechaHora, buffer, &un_tcp_socket ); // Imprimo la Fecha y hora
						break;

					case( OPCION_3 ):
						// AGREGAR EVENTO
						MENU_pedirDatosEvento( TCP, &unEvento, &FechaHora, buffer, &un_tcp_socket );
						EVENTOS_agregarEvento( &unEvento ); // ver el tema del array de eventos, lo agregue en el mail lo saque del Eventos para poder manejar diferentes array , 1 por instancia, a no ser que sea todo el emis
						break;

					case ( OPCION_4 ):
						// ELIMINAR EVENTO SEGUN EL NUMERO DE EVENTO (ES DE 1 EN adelante segun posicion en el array))
						int_id_evento = MENU_eliminarEvento( TCP, buffer, &un_tcp_socket );
						printf("%d\n", int_id_evento);
						EVENTOS_eliminarEvento( int_id_evento );  // revisar despues de agregarEvento
						break;

					case( OPCION_5 ):
						// CONSULTAR EVENTO
						MENU_consultarEventos( TCP, buffer, &un_tcp_socket );
						MENU_printEvento( CONSOLA, buffer, &un_tcp_socket );
						break;

					case( OPCION_6 ):
						// CONSULTA ANALOGICA
						MENU_pedirEntradaAnalogica( TCP, buffer, &un_tcp_socket );

						// Nos aseguramos que tenemos el recurso para nosotros
						OSSemPend(pSemaforoAtoI, 0, &Error);
						int_opcion_menu = atoi( buffer );
						// Devolvemos el recurso
						OSSemPost(pSemaforoAtoI);

						int_Analog_Value = IO_getAnalogInput( int_opcion_menu ); //el valor que toma por parametro es un unsigned char
						MENU_mostrarEntradaAnalogica( TCP, &int_Analog_Value, buffer, &un_tcp_socket );
						break;

					case( OPCION_7 ):
						// SALIR
						printf("Salir");
						break;

					default:
						// OPCION INCORRECTA
						printf("\nA IMPLEMENTAR: Mandar este mensaje a la interfaz correcta: Opcion DEFAULT: %d \n", int_opcion_menu);
						printf("A IMPLEMENTAR: Mandar este mensaje a la interfaz correcta: Vuelva a ingresar\n");
						OSTimeDlyHMSM(0,0,0,500);
				}
				OSTimeDlyHMSM(0,0,0,500);
			} while(tcp_tick(&un_tcp_socket));
		}
#ifdef DEBUG
		printf("\nDEBUG: Conexion finalizada....\n");
#endif

	}
}


main(){

	auto INT8U Error;
   auto int puerto7;
   puerto7 = 7;

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
	//tcp_reserveport(7);

	EVENTOS_Eventos_init();

	// Deshabilitamos el scheduling mientras se crean las tareas
	OSSchedLock();

	//Creacion de tareas
	Error = OSTaskCreate(Led_Red, NULL, 256, 5);
	Error = OSTaskCreate(interfaz_tcp, NULL, 4096, 6 );
	//Error = OSTaskCreate(interfaz_tcp, NULL, 4096, 7 ); // al meter otro TCP, conecta pero no muestra el menu.
	Error = OSTaskCreate(interfaz_consola,NULL, 2048, 8);

	// Creacion del semaforo para proteger el acceso a RTC
	pSemaforoRTC = OSSemCreate(1);
	pSemaforoAtoI = OSSemCreate(1);

	// Re-habilitamos scheduling
	OSSchedUnlock();

	// Iniciamos el sistema operativo comenzando por la tarea en estado "ready" de nayor prioridad
	OSStart();
}