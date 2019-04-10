#use IO.LIB
#use LED.LIB
#use UTILITIES.LIB

#use EVENTOS.LIB
#use RTC.lib
#use MENU.LIB

#define ON_TIME	400
#define OFF_TIME	800
#define ONE_SECOND	1000

struct tm* p_FechaHora;
char opcion_menu[5];
int int_opcion_menu, int_lugar_libre;
Event *p_unEvento;

main(){

// INIT
	HW_init();
	EVENTOS_initEventos();

// LOOP MAQUINAS DE ESTADO
	while(1)
	{
		// Parte 1 - Maquina de estado para verificar que no estamos colgados
		costate RED_LED always_on
		{
			LED_RED_SET();
			waitfor(DelayMs(ON_TIME));
			LED_RED_RESET();
			waitfor(DelayMs(OFF_TIME));
		}

		// Parte 2 - Maquina de estado para mostrar menu y manipular hora y eventos
		costate MENU always_on
		{
			MENU_mostrarMenuPrincipal();
			waitfor(getswf( opcion_menu ));
			int_opcion_menu = atoi( opcion_menu );
			printf("PRUEBA GET: --- %d ---- \n",int_opcion_menu);

			switch( int_opcion_menu ){
				case( OPCION_1 ):
					// FIJAR HORA EN EL RELOJ
					wfd MENU_pedirFechaHora( p_FechaHora );
					RTC_fijarFechaHora( p_FechaHora );
					printFechaHora( p_FechaHora );		// Imprimo la Fecha y hora modificadoS
					break;

				case( OPCION_2 ):
					// CONSULTAR HORA ACTUAL
					wfd MENU_consultarHora();
					RTC_leerFechaHora( p_FechaHora );	// Leo el RTC
					printFechaHora( p_FechaHora );		// Imprimo la Fecha y hora
					break;

				case( OPCION_3 ):
					// AGREGAR EVENTO
					// Aca capaz falta una funcion que busque el espacio libre antes de pedir datos.
					// Evalua si hay sigue sino dice que no hay mas espacio.
					// int_lugar_libre = quedaLugar( &un_Evento );
					//p_unEvento = &eventos[0];
					wfd MENU_pedirDatosEvento( p_unEvento );
					EVENTOS_agregarEvento( p_unEvento );
					break;

					case ( OPCION_4 ):
						// ELIMINAR EVENTO SEGUN EL NUMERO DE EVENTO (ES DE 1 EN adelante segun posicion en el array))
						wfd MENU_eliminarEvento();
						break;

				case( OPCION_5 ):
					// CONSULTAR EVENTO
					wfd MENU_consultarEventos();
					EVENTOS_listarEventos();
					break;

				case( OPCION_6 ):
					// SALIR
					printf("Salir");
					break;

				default:
					// OPCION INCORRECTA
					printf("\nOpcion DEFAULT: %d \n", int_opcion_menu);
					printf("Vuelva a ingresar\n");
				}
		}

		// Parte 3 - Maquina de estado para checkear si es momento de ejecutar algun evento programado
		costate EVENT_CHECKER always_on
		{
	  	//	wfd MENU_consultarEventos();
    //     EVENTO_ejecutarEventos();
		}

	}
}