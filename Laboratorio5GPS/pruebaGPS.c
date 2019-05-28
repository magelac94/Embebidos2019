#define OS_MAX_TASKS			5  		// Maximum number of tasks system can create (less stat and idle tasks)
#define OS_TASK_SUSPEND_EN		1		// Habilitar suspender y resumir tareas
#define OS_TASK_DEL_EN 			1
#define OS_TIME_DLY_HMSM_EN		1		// Habilitar la funcion de delay para pasar fecha y hora
#define OS_SEM_EN				1		// Habilitar semaforos
#define OS_MAX_EVENTS			7		// MAX_TCP_SOCKET_BUFFERS + 2 + 3 (3 semaforos son usados)
#define STACK_CNT_256			5		// tarea_Led_Red + idle
#define STACK_CNT_512			2		// main() + Interfaz Consola + ejecutar eventos
#define STACK_CNT_2K			3		// 2 Tareas TCP (MAX_TCP_SOCKET_BUFFERS)

#use RTC.lib
#use GPS_Custom.lib
#use IO.LIB
#use ucos2.lib
#use MENU.LIB

void GPS_configRTC(void* pdata){
	struct tm FechaHora;
	struct tm p_dateTime;
	unsigned long timestampGPS;
	unsigned long timestampRabbit;

	char* p_str;
	char buffer[500];

//	while(true){
		// Obtengo Fecha del GPS
		gps_get_utc( &p_dateTime, p_str);

		printFechaHora(0,p_dateTime,buffer, NULL)

	/*	timestampGPS = mktime( &p_dateTime );

		// Obtengo Fecha Placa
		timestampRabbit = RTC_leerTimestamp();

		// Evaluo si la fecha obtenida es igual a la que ya esta configurada
		if (timestampGPS != timestampRabbit){
			RTC_fijarFechaHora( &p_dateTime );
		}                             */
}


main() {
	struct tm FechaHora;
	struct tm p_dateTime;
	unsigned long timestampGPS;
	unsigned long timestampRabbit;

	char* p_str;

	HW_init();

	OSInit();

	OSTaskCreate(GPS_init, NULL, 256, 2);  // Inicializa Hardware GPS - Ejecuta 1 vez
	OSTaskCreate(GPS_gets, p_str, 256, 3); // Se obtiene datos gps

	printf("%s", p_str);
  	OSTaskCreate(GPS_configRTC, NULL, 256, 11); // Configura el Reloj Periodicamente

	OSStart();

}