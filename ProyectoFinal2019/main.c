#define DEBUG			// activo para imprimir mensajes de DEBUG

/* uCOS configuration */
#define OS_MAX_TASKS			3		// Cantidad maxima de tareas que se pueden crear, sin contar STAT e IDLE
#define OS_TASK_SUSPEND_EN		1		// Habilitar suspender y resumir tareas
#define OS_TASK_DEL_EN			1		// Habilitar eliminacion de tareas
#define OS_TIME_DLY_HMSM_EN		1		// Habilitar la funcion de delay para pasar fecha y hora
#define OS_Q_EN					1		// Habilitar colas (queues)
#define OS_Q_POST_EN			1		// Enable posting messages to queue
#define OS_MAX_EVENTS			2		// MAX_TCP_SOCKET_BUFFERS + 0 Mbox + 1 Queue + 0 Semaforos
#define STACK_CNT_256			2		// tarea_Led_Red + idle
#define STACK_CNT_512			4		// main() + GPRS_tarea_encender_modem + CONSOLA_tarea_comandos_a_mano
#define STACK_CNT_2K			2		// 1 Tareas TCP (MAX_TCP_SOCKET_BUFFERS)

/* TCP/IP configuration */
#define TCPCONFIG 0
#define USE_ETHERNET		1
#define MY_IP_ADDRESS "10.10.0.10"
#define MY_NETMASK "255.255.255.0"
#define LPORT 7
#define MY_GATEWAY "10.10.0.1"
#define MAX_TCP_SOCKET_BUFFERS	1					// Determina la cantidad maxima de sockets con buffer preasignado
#define TCP_BUF_SIZE			1024				// Tamanio del buffer TCP (L + E)
#define TAMANIO_BUFFER_LE 		512      			// Este es el tamanio que le damos a nuestros buffers para leer y enviar al socket

/* Incluimos las librerias luego de los define para sobre escribir los macros deseados */
#use TSalud.lib
#use RTC.lib
#use GPS_Custom.lib
#use GPS_funciones.LIB
#use IO.lib
#use LED.lib
#use GPRS.lib
#use CONSOLA.lib
#use Utilities.lib

#memmap xmem
#use "ucos2.lib"
#use "dcrtcp.lib"

/* Definicion de semaforos, mbox y queues. GLOBALES*/
OS_EVENT *SmsQ;
void* SmsQStorage[5]; // La queue tiene para guardar 5 mensajes pendientes.

/* Definicion de otras variables. GLOBALES*/
char id_participante[32];

main(){

	// Variables
	auto INT8U Error;
	static tcp_Socket un_tcp_socket[MAX_TCP_SOCKET_BUFFERS];
	char* tramaGPS;
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

// Deshabilitamos el scheduling mientras se crean las tareas
	OSSchedLock();

	//Creacion de semaforos, mbox y queues
	SmsQ = OSQCreate(&SmsQStorage[0], 5); // Crear una cola donde poner los mensajes a enviar

	//Creacion de tareas
	Error = OSTaskCreate(LED_tarea_led_red, NULL, 256, 5);
	Error = OSTaskCreate(GPRS_tarea_modem, NULL, 512, 6);
	//Error = OSTaskCreate(CONSOLA_tarea_comandos_a_mano, NULL, 512,7);
	Error = OSTaskCreate(TCP1_tarea_interfaz_tcp, &un_tcp_socket[0], 2048, 9 );

	//Error = OSTaskCreate(GPS_init, NULL, 512, 8);  // Inicializa Hardware GPS - Ejecuta 1 vez
	//Error = OSTaskCreate(GPS_gets, tramaGPS,2048 , 9); // Se obtiene datos gps
	//Error = OSTaskCreate(tarea_config_Reloj, tramaGPS, 2048 , 10 );
	//Error = OSTaskCreate(tarea_salud,NULL, 512, 11);
	//Error = OSTaskCreate(tarea_botones,NULL, OJO, 10);

// Re-habilitamos scheduling
	OSSchedUnlock();

	// Iniciamos el sistema operativo comenzando por la tarea en estado "ready" de mayor prioridad
	OSStart();
}