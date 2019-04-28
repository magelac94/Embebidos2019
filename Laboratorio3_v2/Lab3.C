#use IO.LIB
#use LED.LIB
#use TCPCON.lib
#use EVENTOS.LIB
#use UTILITIES.LIB
#use RTC.lib
#use MENU.lib

#define ON_TIME	400
#define OFF_TIME	800
#define ONE_SECOND	1000

struct tm FechaHora;
char entradaAnalogica[2];
char opcion_menu[5];
int int_opcion_menu, int_lugar_libre, int_id_evento;
Event unEvento;

cofunc void programaPrincipal( enum tipoUI interfazAUsar ){

	wfd MENU_mostrarMenuPrincipal( interfazAUsar );
	wfd MENU_obtenerOpcion( interfazAUsar, opcion_menu );
	int_opcion_menu = atoi( opcion_menu );

	switch( int_opcion_menu ){
		case( OPCION_1 ):
			// FIJAR HORA EN EL RELOJ
			wfd MENU_pedirFechaHora( &FechaHora, interfazAUsar );
			RTC_fijarFechaHora( &FechaHora );
			MENU_printFechaHora( &FechaHora, interfazAUsar );		// Imprimo la Fecha y hora modificadoS
			break;

		case( OPCION_2 ):
			// CONSULTAR HORA ACTUAL
			wfd MENU_consultarHora( interfazAUsar );
			RTC_leerFechaHora( &FechaHora );	// Leo el RTC
			MENU_printFechaHora( &FechaHora, interfazAUsar );		// Imprimo la Fecha y hora
			break;

		case( OPCION_3 ):
			// AGREGAR EVENTO
			wfd MENU_pedirDatosEvento( &unEvento, &FechaHora, interfazAUsar );
			EVENTOS_agregarEvento( &unEvento );
			break;

		case ( OPCION_4 ):
			// ELIMINAR EVENTO SEGUN EL NUMERO DE EVENTO (ES DE 1 EN adelante segun posicion en el array))
			wfd {int_id_evento = MENU_eliminarEvento( interfazAUsar )};
			EVENTOS_eliminarEvento( int_id_evento );
			break;

		case( OPCION_5 ):
			// CONSULTAR EVENTO
			wfd MENU_consultarEventos( interfazAUsar );
			EVENTOS_listarEventos();
			break;
			
		case( OPCION_6 ):
			// Consulta analogica
			wfd MENU_pedirEntradaAnalogica( interfazAUsar, &entradaAnalogica );
			wfd {int_Analog_Value = IO_getAnalogInput( &entradaAnalogica )}; //el valor que toma por parametro es un unsigned char
			wfd MENU_mostrarEntradaAnalogica( interfazAUsar, &int_Analog_Value );
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

main(){

// INIT
	HW_init();
	/*
	This function should be called in the beginning of a program that uses single-user cofunctions. It initializes
	internal data structures that are used by loophead().
	*/
	loopinit();
	/* 2.1.4 TCP/IP Stack Initialization
	The function sock_init() must be called near the start of your main() function in order to initialize
	the TCP/IP stack. The return value from sock_init() must indicate success before calling any other
	TCP/IP functions, with the possible exception of ifconfig().
	*/
	printf("Iniciando Socket\n");
	sock_init();
	printf("Socket Iniciado\n");
	EVENTOS_Eventos_init();

// LOOP MAQUINAS DE ESTADO
	while(1)
	{
		/*
		This function should be called within the “big loop” in your program.
		It is necessary for proper single-user cofunction abandonment handling
		*/
		loophead();
		//Maquina de estado para verificar que no estamos colgados
		costate RED_LED always_on
		{
			LED_RED_SET();
			waitfor(DelayMs(ON_TIME));
			LED_RED_RESET();
			waitfor(DelayMs(OFF_TIME));
		}

		//Maquina de estado para mantener conexion
		costate CONEXION_TCP always_on
		{
			wfd TCPCON_conexion();
		}

		//Maquina de estado para mostrar menu y manipular hora y eventos desde consola
		costate INSTANCIA_CONSOLA always_on
		{
			wfd programaPrincipal( CONSOLA );
		}
		// Laboratorio 3 - Maquina de estado para manejar los eventos desde TCP.
		costate INSTANCIA_TCP always_on
		{
			wfd programaPrincipal( TCP );
		}

		//Maquina de estado para checkear si es momento de ejecutar algun evento programado
		costate EVENT_CHECKER always_on
		{
			EVENTOS_ejecutarEventos();
		}
	}
}