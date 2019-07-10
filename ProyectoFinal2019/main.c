#define DEBUG			// activo para imprimir mensajes de DEBUG
#define GPSDEBUG		// al activarlo imprime las tramas que llegan
//#define GPRSDEBUG
//#define TCPDEBUG
#define BOTONESDEBUG
//#define MENUDEBUG
//#define CHECKDEBUG


/* uCOS configuration */
#define OS_MAX_TASKS			9		// Cantidad maxima de tareas que se pueden crear, sin contar STAT e IDLE
#define OS_TASK_SUSPEND_EN		1		// Habilitar suspender y resumir tareas
#define OS_TASK_DEL_EN			1		// Habilitar eliminacion de tareas
#define OS_TIME_DLY_HMSM_EN		1		// Habilitar la funcion de delay para pasar fecha y hora
#define OS_Q_EN					1		// Habilitar colas (queues)
#define OS_Q_POST_EN		  	1		// Enable posting messages to queue
#define OS_MAX_EVENTS			2		// MAX_TCP_SOCKET_BUFFERS + 0 Mbox + 1 Queue + 0 Semaforos
#define STACK_CNT_256			2		// tarea_Led_Red + idle
#define STACK_CNT_512			7		// main() + GPRS_tarea_encender_modem + CONSOLA_tarea_comandos_a_mano
#define STACK_CNT_2K			2		// 1 Tareas TCP (MAX_TCP_SOCKET_BUFFERS)

/* TCP/IP configuration */
#define TCPCONFIG 0
#define USE_ETHERNET		1
#define MY_IP_ADDRESS "10.10.0.10"
#define MY_NETMASK "255.255.255.0"
#define LPORT 7
#define MY_GATEWAY "10.10.0.1"
#define MAX_TCP_SOCKET_BUFFERS	1					// Determina la cantidad maxima de sockets con buffer preasignado
#define TCP_BUF_SIZE				1024				// Tamanio del buffer TCP (L + E)
#define TAMANIO_BUFFER_LE 		512      			// Este es el tamanio que le damos a nuestros buffers para leer y enviar al socket
#define STDIO_ENABLE_LONG_STRING

/* Incluimos las librerias luego de los define para sobre-escribir los macros deseados */
#use TSalud.lib
#use RTC.lib
#use GPS_Custom.lib
#use IO.lib
#use LED.lib
#use CHECKPOINT.lib
#use GPRS.lib
#use GPS_funciones.LIB
#use controlBotones.lib
#use TCP1.lib
#use Utilities.lib

#memmap xmem
#use "ucos2.lib"
#use "dcrtcp.lib"
#use MENU.lib

enum prioridadTareas{
	TAREA_LED = 5,
	TAREA_MODEM,
	TAREA_TCP,
	TAREA_GPSINIT,
	TAREA_GPS,
	TAREA_RELOJ,
	TAREA_SALUD,
	TAREA_BOTONES
};
/* Definicion de semaforos, mbox y queues. GLOBALES*/
OS_EVENT *SmsQ;
void* SmsQStorage[5]; // La queue tiene para guardar 5 mensajes pendientes.

/* Definicion de otras variables. GLOBALES*/
char ID_PARTICIPANTE[33];

/* Array para almacenar datos de config en el UserBlock*/
void* save_data[6];
unsigned int save_lens[6];

main(){
	// Variables
	auto INT8U Error;
	static tcp_Socket un_tcp_socket[MAX_TCP_SOCKET_BUFFERS];

  	// Inicializa el hardware de la placa
	HW_init();

	// Inicializacion de Checkpoints
	CHECKPOINT_Checkpoints_init();

	// Inicializacion de otras Variables
	strcpy(ID_PARTICIPANTE," ");
	CHKP_RADIO = 0;
	RIT_MAX = 0;
	RIT_MIN = 0;
	T_CONTROL = 0;

	// Leemos si hay config guardada en UserBlock
	save_data[0] = checkpoints;
	save_lens[0] = sizeof(checkpoints);
	save_data[1] = &CHKP_RADIO;
	save_lens[1] = sizeof(CHKP_RADIO);
	save_data[2] = ID_PARTICIPANTE;
	save_lens[2] = sizeof(ID_PARTICIPANTE);
	save_data[3] = &RIT_MAX;
	save_lens[3] = sizeof(RIT_MAX);
	save_data[4] = &RIT_MIN;
	save_lens[4] = sizeof(RIT_MIN);
	save_data[5] = &T_CONTROL;
	save_lens[5] = sizeof(T_CONTROL);
	readUserBlockArray(save_data, save_lens, 6, 0);

	// Inicializa la estructura de datos interna del sistema operativo uC/OS-II
	OSInit();

	// Iniciamos el stack TCP/IP
#ifdef TCPDEBUG
	printf("\nDEBUG: Iniciando Sockets\n");
#endif
	sock_init();
#ifdef DEBUGTCP
	printf("\nDEBUG: Socket Iniciados\n");
#endif


	// Deshabilitamos el scheduling mientras se crean las tareas
	OSSchedLock();

	// Creacion de semaforos, mbox y queues
 	SmsQ = OSQCreate(&SmsQStorage[0], 5); // Crear una cola donde poner los mensajes a enviar

	//Creacion de tareas
	Error = OSTaskCreate(LED_tarea_led_red, NULL, 256, TAREA_LED); // 1
	Error = OSTaskCreate(GPRS_tarea_modem, NULL, 512, TAREA_MODEM); // 2
  	Error = OSTaskCreate(TCP1_tarea_interfaz_tcp, &un_tcp_socket[0], 2048, TAREA_TCP ); // 3
	Error = OSTaskCreate(GPS_init, NULL, 512, TAREA_GPSINIT); // 4
	Error = OSTaskCreate(GPSFUNCIONES_tarea_obtenertrama, NULL, 512 , TAREA_GPS); // 5
	Error = OSTaskCreate(GPSFUNCIONES_tarea_config_Reloj, NULL, 512 , TAREA_RELOJ ); // 6
	Error = OSTaskCreate(TSALUD_tarea_salud,NULL, 512, TAREA_SALUD); // 7
	Error = OSTaskCreate(CONTROLBOTONES_tarea_botones,NULL, 512, TAREA_BOTONES); // 8

 	// Re-habilitamos scheduling
	OSSchedUnlock();

	// Iniciamos el sistema operativo comenzando por la tarea en estado "ready" de mayor prioridad
	OSStart();
}