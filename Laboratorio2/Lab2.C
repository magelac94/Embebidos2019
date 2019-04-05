#use IO.LIB
#use BTN.LIB
#use LED.LIB
#use UTILITIES.LIB
#use RTC.LIB		//falta implementar
#use MENU.LIB		//falta implementar
#use EVENTOS.LIB	//falta implementar

#define ON_TIME	400
#define OFF_TIME	800
#define ONE_SECOND	1000


main(){
	enum opcionMenu opcion;
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
			MENU_mostrarMenuPrincipal();
			waitfor(getswf(opcion));
			printf("\n");
			switch( opcion ){
				case( OPCION_1 ):					// Fijar hora del reloj
					RTC_fijarHora();
					break;

				case( OPCION_2 ):
					RTC_consultarHora();
					break;

				case( OPCION_3 ):
					EVENTO_agregarEvento();
					break;

				case( OPCION_4 ):
					EVENTO_eliminarEvento();
					break;

				case( OPCION_5 ):
					EVENTO_consultarEventos();
					break;

				default: printf("Vuelva a ingresar\n");

			}
		}
		
		// Parte 3 - Maquina de estado para checkear si es momento de ejecutar algun evento programado
		costate EVENT_CHECKER always_on
		{
			
		}

	}
}