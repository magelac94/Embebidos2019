#use IO.LIB
#use LED.LIB
#use UTILITIES.LIB
#use MENU.LIB

#define ON_TIME	400
#define OFF_TIME	800
#define ONE_SECOND	1000


main(){
	char* p_opcion;
	HW_init();

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
			MENU_mostrarMenuPrincipal( p_opcion );
			//waitfor(getswf( p_opcion )); Esto se mueve al menu por que es el encargado de interactuar con el usuario
			switch( *p_opcion ){
				case( OPCION_1 ):					// Fijar hora del reloj
					//Es el modulo menu el encargado de mostrar lo que debe ingresar el usuario
					//tenemos podriamos hacer un wait for para que la maquina MENU libere el CPU
					// y la maquina EVENT_CHECKER pueda hacer su trabajo de ver si es hora de correr algun
					// evento programado (prender/apagar led). Por ahora va asi por un error de compilacion
					//Cuando termine, sigue y hace el brak y vuelve al menu principal.
					MENU_fijarFechaHora();
					break;

				case( OPCION_2 ):
					MENU_consultarHora();
					break;

				case( OPCION_3 ):
					MENU_agregarEvento();
					break;

				case( OPCION_4 ):
					MENU_eliminarEvento();
					break;

				case( OPCION_5 ):
					MENU_consultarEventos();
					break;

				default:
            	printf("Vuelva a ingresar\n");

			}
		}

		// Parte 3 - Maquina de estado para checkear si es momento de ejecutar algun evento programado
//		costate EVENT_CHECKER always_on
//		{
//
//		}

	}
}