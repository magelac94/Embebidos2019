//#class auto 			// Change default storage class for local variables: on the stack
#define DEBUG			// activo para imprimir estado de conexion TCP
//#define NOANDA        //Activo si quiero probar el caso que no anda

/* uCOS configuration */
#define OS_MAX_TASKS			7  		// Maximum number of tasks system can create (less stat and idle tasks)
#define OS_TASK_SUSPEND_EN		1		// Habilitar suspender y resumir tareas
#define OS_TIME_DLY_HMSM_EN		1		// Habilitar la funcion de delay para pasar fecha y hora
#define OS_SEM_EN				1		// Habilitar semaforos
#define OS_MAX_EVENTS			7		// MAX_TCP_SOCKET_BUFFERS + 2 + 3 (3 semaforos son usados)
#define STACK_CNT_256			2		// Led_Red + idle
#define STACK_CNT_512			2		// main() + Interfaz Consola + ejecutar eventos
#define STACK_CNT_2K			3		// 2 Tareas TCP (MAX_TCP_SOCKET_BUFFERS)

/* TCP/IP configuration */
#define TCPCONFIG 0
#define USE_ETHERNET		1
#define MY_IP_ADDRESS "10.10.0.10"
#define MY_NETMASK "255.255.255.0"
#define LPORT 7
#define MY_GATEWAY "10.10.0.1"
#define MAX_TCP_SOCKET_BUFFERS	2					// Determina la cantidad maxima de sockets con buffer preasignado
#define TCP_BUF_SIZE			1024				// Tamanio del buffer TCP (L + E)
#define TAMANIO_BUFFER_LE 		512      			// Este es el tamanio que le damos a nuestros buffers para leer y enviar al socket

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
OS_EVENT* pSemaforoEventos;


// Tarea 1 - Prende y apaga el led. Cuando espera se suspende.
// Por eso le daremos una prioridad alta. Es nuestro test para ver que todo corre.
// Le asignaremos un stack de 256.
void tarea_led_red(void* pdata){

	while(1){
		LED_RED_SET();
		OSTimeDlySec(1);
		LED_RED_RESET();
		OSTimeDlySec(1);
	}
}



// Tarea 2 - Menu por consola
void tarea_interfaz_consola(void* pdata){

	Event unEvento;
	struct tm FechaHora;
	char buffer[TAMANIO_BUFFER_LE];
	int i, int_opcion_menu, int_id_evento, int_Analog_Value;
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
				EVENTOS_eliminarEvento( int_id_evento );  // revisar despues de agregarEvento
				break;
			case( OPCION_5 ):
				// CONSULTAR EVENTO
				MENU_consultarEventos( CONSOLA, buffer, NULL );
				MENU_printEventos( CONSOLA, buffer, NULL );
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
void tarea_interfaz_tcp(void* pdata){

	Event unEvento;
	struct tm FechaHora;
	char buffer[TAMANIO_BUFFER_LE];
	tcp_Socket* pun_tcp_socket;
	auto int i, int_opcion_menu, int_id_evento, int_Analog_Value;
	auto INT8U Error;

	pun_tcp_socket = (tcp_Socket*)pdata;

	while(1) {
		// Ponemos el socket en estado de escucha
		if (!tcp_listen(pun_tcp_socket, LPORT, 0, 0, NULL, 0)){
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
		printf("\nDEBUG: Escuchando en puerto %u por conexiones\n", LPORT );
#endif
		// Esperamos por la conexion y si no esta establecida y pronta, suspende por 500ms.
		while( !sock_established(pun_tcp_socket) ) { // && sock_bytesready(pun_tcp_socket)==-1
			tcp_tick(pun_tcp_socket);
			OSTimeDlyHMSM(0,0,0,500);
		}
		// Un delay para que se establezca la conexion
		OSTimeDlyHMSM(0,0,0,500);

		if (sock_established(pun_tcp_socket)){
#ifdef DEBUG
			printf("\nDEBUG: Conexion establecida\n");
#endif
			sock_mode(pun_tcp_socket, TCP_MODE_ASCII);
			do {

				// Se muestra menu y se espera opcion
				MENU_mostrarMenuPrincipal( TCP, buffer, pun_tcp_socket );
				MENU_obtenerOpcion( TCP, buffer, pun_tcp_socket );

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
						MENU_pedirFechaHora( TCP, &FechaHora, buffer, pun_tcp_socket );
						RTC_fijarFechaHora( &FechaHora );
						break;

					case( OPCION_2 ):
						// CONSULTAR FECHA HORA ACTUAL
						MENU_consultarHora( TCP, buffer, pun_tcp_socket );
						RTC_leerFechaHora( &FechaHora );	// Leo el RTC
						MENU_printFechaHora( TCP, &FechaHora, buffer, pun_tcp_socket ); // Imprimo la Fecha y hora
						break;

					case( OPCION_3 ):
						// AGREGAR EVENTO
						MENU_pedirDatosEvento( TCP, &unEvento, &FechaHora, buffer, pun_tcp_socket );
						EVENTOS_agregarEvento( &unEvento );
						break;

					case ( OPCION_4 ):
						// ELIMINAR EVENTO SEGUN EL NUMERO DE EVENTO (ES DE 1 EN adelante segun posicion en el array))
						int_id_evento = MENU_eliminarEvento( TCP, buffer, pun_tcp_socket );
						printf("%d\n", int_id_evento);
						EVENTOS_eliminarEvento( int_id_evento );  // revisar despues de agregarEvento
						break;

					case( OPCION_5 ):
						// CONSULTAR EVENTO
						MENU_consultarEventos( TCP, buffer, pun_tcp_socket );
						MENU_printEventos( TCP, buffer, pun_tcp_socket );
						break;

					case( OPCION_6 ):
						// CONSULTA ANALOGICA
						MENU_pedirEntradaAnalogica( TCP, buffer, pun_tcp_socket );

						// Nos aseguramos que tenemos el recurso para nosotros
						OSSemPend(pSemaforoAtoI, 0, &Error);
						int_opcion_menu = atoi( buffer );
						// Devolvemos el recurso
						OSSemPost(pSemaforoAtoI);

						int_Analog_Value = IO_getAnalogInput( int_opcion_menu ); //el valor que toma por parametro es un unsigned char
						MENU_mostrarEntradaAnalogica( TCP, &int_Analog_Value, buffer, pun_tcp_socket );
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
			} while(tcp_tick(pun_tcp_socket));
		}
#ifdef DEBUG
		printf("\nDEBUG: Conexion finalizada....\n");
#endif

	}
}


// Tarea 4 - Ejecutar eventos
void tarea_ejecutar_eventos( void* pdata){
	EVENTOS_ejecutarEventos();
	OSTimeDlyHMSM(0, 0, 5, 0);
}

main(){

	auto INT8U Error;
	static tcp_Socket un_tcp_socket[MAX_TCP_SOCKET_BUFFERS];

	// Inicializa el hardware de la placa
	HW_init();

	// Inicializa la estructura de datos interna del sistema operativo uC/OS-II
	OSInit();

   // Creacion del semaforo para proteger el acceso a funciones no reentrantes y recursos compartidos
pSemaforoRTC = OSSemCreate(1);
pSemaforoAtoI = OSSemCreate(1);
pSemaforoEventos = OSSemCreate(1);

	// Iniciamos el stack TCP/IP
#ifdef DEBUG
	printf("\nDEBUG: Iniciando Sockets\n");
#endif
	sock_init();
#ifdef DEBUG
	printf("\nDEBUG: Socket Iniciados\n");
#endif

	// Deshabilitamos el scheduling mientras se crean las tareas
	OSSchedLock();

	//Creacion de tareas
	Error = OSTaskCreate(tarea_led_red, NULL, 256, 5);
	//Error = OSTaskCreate(tarea_ejecutar_eventos, NULL, 2048, 6);
	Error = OSTaskCreate(tarea_interfaz_tcp, &un_tcp_socket[0], 2048, 7 );
	Error = OSTaskCreate(tarea_interfaz_tcp, &un_tcp_socket[1], 2048, 8 );
	Error = OSTaskCreate(tarea_interfaz_consola,NULL, 512, 9);

   // Inicializamos el array de eventos
	EVENTOS_Eventos_init();

	// Re-habilitamos scheduling
	OSSchedUnlock();

	// Iniciamos el sistema operativo comenzando por la tarea en estado "ready" de mayor prioridad
	OSStart();
}