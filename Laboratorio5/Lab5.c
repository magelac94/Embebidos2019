#define OS_MAX_TASKS			5  		// Maximum number of tasks system can create (less stat and idle tasks)
#define OS_TASK_SUSPEND_EN		1		// Habilitar suspender y resumir tareas
#define OS_TASK_DEL_EN 			1
#define OS_TIME_DLY_HMSM_EN		1		// Habilitar la funcion de delay para pasar fecha y hora
#define OS_SEM_EN				1		// Habilitar semaforos
#define OS_MAX_EVENTS			7		// MAX_TCP_SOCKET_BUFFERS + 2 + 3 (3 semaforos son usados)
#define STACK_CNT_256			2		// Led_Red + idle
#define STACK_CNT_512			2		// main() + Interfaz Consola
#define STACK_CNT_2K			2		// 2 Tareas TCP (MAX_TCP_SOCKET_BUFFERS)

#use GPS_Custom.lib
#use ucos2.lib

main(){
	INT8U Error;
	char* p_str;

	OSInit();
	printf("Hola Mundo");
	Error = OSTaskCreate(GPS_init, NULL, 256, 5);
	Error = OSTaskCreate(GPS_gets, p_str, 256, 5);
	printf("%s\n",*p_str );

	OSStart();
}