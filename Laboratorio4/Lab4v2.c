//#class auto 			// Change default storage class for local variables: on the stack
#define DEBUG			// activo para imprimir estado de conexion TCP
/* uCOS configuration */
#define OS_MAX_TASKS			2  		// Maximum number of tasks system can create (less stat and idle tasks)
#define OS_TASK_SUSPEND_EN		1		// Habilitar suspender y resumir tareas
#define OS_TIME_DLY_HMSM_EN		1		// Habilitar la funcion de delay para pasar fecha y hora
#define STACK_CNT_256			2		// Led_Red + idle
#define STACK_CNT_512			1		// main()
#define STACK_CNT_2K			1		// 1 Tarea TCP

/* Definimos la prioridad de las tareas */
#define TASK3			3
#define TASK4			4

/* TCP/IP configuration */
#define TCPCONFIG 0
#define USE_ETHERNET		1
#define MY_IP_ADDRESS "10.10.0.10"
#define MY_NETMASK "255.255.255.0"
#define PORT 7
#define MY_GATEWAY "10.10.0.1"
#define TAMANIO_BUFFER_LE 512        // Este es el tamanio que le damos a nuestros buffers para leer y enviar al socket

/* Incluimos las librerias luego de los define para sobre escribir los macros deseados */
#use IO.LIB
#use LED.LIB
#use EVENTOS.LIB


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
void interfaz_consola(void* pdata);

// Tarea 3 - Menu por TCP
/* Esta tarea escucha por una conexion entrante por el puerto 7.
Cuando hay una conexion activa, muestra el menu por la interfaz tcp.
Debemos asignarle un stack de 2K por el socket tcp.
*/
void interfaz_tcp(void* pdata){

	static char buffer_recepcion[TAMANIO_BUFFER_LE], buffer_envio[TAMANIO_BUFFER_LE];
	static tcp_Socket tcp_socket;
	auto int bytes_leidos, bytes_enviados, i;

	while(1) {
		// Ponemos el socket en estado de escucha
		tcp_listen(&tcp_socket, 7, 0, 0, NULL, 0);	// Ponemos a escuchar el puerto

		// Inicializamos los buffers para sacar la basura
		for ( i = 0; i < TAMANIO_BUFFER_LE; i++ ){
			buffer_recepcion[i] = ' ';
			buffer_envio[i] = ' ';
		}

		// Esperamos por la conexion y si no esta establecida y pronta, suspende por 1 seg.
#ifdef DEBUG
		printf("\nDEBUG: Escuchando en puerto 7 por conexiones\n");
#endif
		while( !sock_established(&tcp_socket)){ // && sock_bytesready(&tcp_socket)==-1 ) {
			tcp_tick(NULL);
			OSTimeDlyHMSM(0,0,1,0);
		}

#ifdef DEBUG
		printf("\nDEBUG: Conexion establecida\n");
#endif
		// Un delay para que se establezca la conexion
		OSTimeDlyHMSM(0,0,2,0);
		//Hago primero y al final actualizo el socket

		// Mandamos y revibimos del socket
		do {
			// Aca pongo el programa principal. Cuando espero por input hago ticks mientras espero
			// Esto es una prueba de concepto.
			MENU_mostrarMenuPrincipal( TCP, buffer_envio );
			OSTimeDlyHMSM(0,0,5,0);

			/* Esta funcion lee del buffer del socket tcp hasta el lago de
			buffer_recepcion - 1 y lo guarda en buffer_recepcion. Si no lo puede llenar
			lee lo que hay y retorna */
			bytes_leidos=sock_fastread( &tcp_socket, buffer_recepcion, sizeof(buffer_recepcion)-1 );
			if(bytes_leidos>0) {
				buffer_recepcion[bytes_leidos]=0; // Terminamos la cadena del string
			}

			/* Esta funcion escribe en el buffer del socket tcp hasta el largo
			de buffer_envio
			*/
			bytes_enviados=sock_fastwrite( &tcp_socket, buffer_envio, sizeof(buffer_envio)-1 );
			/*if(bytes_enviados>0) {
				//buffer_envio[bytes_leidos]=0; // Terminamos la cadena del string
			}*/

		} while(tcp_tick(&tcp_socket));
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
	Error = OSTaskCreate(Led_Red, NULL, 256, TASK3 );
	Error = OSTaskCreate(interfaz_tcp, NULL, 2048, TASK4 );

	// Re-habilitamos scheduling
	OSSchedUnlock();

	// Iniciamos el sistema operativo comenzando por la tarea en estado "ready" de nayor prioridad
	OSStart();
}