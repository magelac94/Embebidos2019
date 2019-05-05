// Integrantes: Aguerrondo, Carballo, Martin
// Abril 2019 UCU
/*
ATENCION: No pudimos utilizar las cofunctions indexadas por que cuando una queda
esperando y se llama la otra, los estados de la estructura CoData se corrompen.
Leimos que es una solucion cuando por ejemplo se quiere controlar 25 puertas y hay que
hacer la misma operacion sobre todas, pero esas operaciones no quedan esperando, comienzan
y terminan, entonces la estructura CoData asocada se mantiene consistente.

Al usar la consola como salida, el menu principal se ve incompleto como si se llenara el buffer
pero eso es imposible por que tiene espacio suficiente. No logramos determinar si el error es
por propiedades de la consola o problemas del codigo. Por TCP se muestra completo.

Si hacemos la salida por TCP, al seleccionar la opcion arranca un bucle infinito donde
el programa principal siempre sale por la opcion DEFAULT.

Para que solo ande por consola comentamos:
use TCPCON.lib

printf("Iniciando Socket\n");
sock_init();
printf("Socket Iniciado\n");

costate de la instancia TCP

caso TCP en funciones: enviar_a_ui y obtener_de_ui
*/
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
char opcion_menu[5];
int int_opcion_menu, int_lugar_libre, int_id_evento, int_Analog_Value;
Event unEvento;

cofunc void programaPrincipal[2]( enum tipoUI interfazAUsar ){
//cofunc void programaPrincipal( enum tipoUI interfazAUsar ){

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
			wfd MENU_pedirEntradaAnalogica( interfazAUsar, opcion_menu );
			int_opcion_menu = atoi(opcion_menu);
			int_Analog_Value = IO_getAnalogInput( int_opcion_menu ); //el valor que toma por parametro es un unsigned char
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
	  //	loophead();
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
			wfd programaPrincipal[CONSOLA]( CONSOLA );
		  //	wfd programaPrincipal( CONSOLA );
		}
		// Laboratorio 3 - Maquina de estado para manejar los eventos desde TCP.
		costate INSTANCIA_TCP always_on
		{
			wfd programaPrincipal[TCP]( TCP );
			//wfd programaPrincipal( TCP );
		}

		//Maquina de estado para checkear si es momento de ejecutar algun evento programado
		costate EVENT_CHECKER always_on
		{
			EVENTOS_ejecutarEventos();
		}
	}
}