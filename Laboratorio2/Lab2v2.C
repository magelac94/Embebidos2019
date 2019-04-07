#use IO.LIB
#use LED.LIB
#use UTILITIES.LIB
#use MENU.LIB
#use EVENTOS.LIB
#use RTC.lib

#define ON_TIME	400
#define OFF_TIME	800
#define ONE_SECOND	1000

struct tm* p1_FechaHora;
struct tm* p2_FechaHora;

main(){
	char p_opcion[3];    // Array para opcion del menu principal.
   char datosEvento[2];

	HW_init();
   EVENTOS_initEventos();
//   struct tm* p_FechaHora;

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

	 		waitfor(getswf( p_opcion )) ;
         printf("PRUEBA GET: --- %s ---- \n",&p_opcion[0]);

			switch(p_opcion[0]){
				case '1':
            	// FIJAR HORA EN EL RELOJ

               //  MENU_pedirFechaHora();   // CHEQUEAR QUE NO FUNCIONA
               p1_FechaHora->tm_hour = 1;
					p1_FechaHora->tm_min = 23;
					p1_FechaHora->tm_mday = 6;
					p1_FechaHora->tm_mon = 4;
					p1_FechaHora->tm_year = 19;

            	RTC_fijarFechaHora( p1_FechaHora );
  					printFechaHora( p1_FechaHora );		// Imprimo la Fecha y hora modificadoS

               break;

				case '2':
                  // CONSULTAR HORA ACTUAL

                  MENU_consultarHora();
                  RTC_leerFechaHora( p2_FechaHora );	// Leo el RTC
						printFechaHora( p2_FechaHora );		// Imprimo la Fecha y hora
                  				// NO esta devolviendo la fecha ingresada en punto 1-revisar
					break;

				case '3':
						//	AGREGAR EVENTO
					  MENU_pedirDatosEvento();
					break;

				case '4':
  //					MENU_eliminarEvento();
  						printf("Estoy en OPCION 4");
					break;

				case '5':
	//				MENU_consultarEventos();
  					printf("Estoy en OPCION 5");
					break;

            case '6':

   				printf("Salir");
					break;

		 		default:
             	printf("\nOpcion DEFAULT :%s \n",&p_opcion[0]);
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