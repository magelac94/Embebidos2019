#use IO.LIB
#use BTN.LIB
#use LED.LIB
#use UTILITIES.LIB


#define ON_TIME	400
#define OFF_TIME	800
#define ONE_SECOND	1000
#define MAX_EVENTS 5
typedef struct {		// Struct de eventos
	char command;
	char param;
	unsigned long time;
} Event;

Event eventos[MAX_EVENTS];		// Lista de Eventos
	
void mostrarMenuPrincipal(){
	printf("Elija una opcion\n");
			printf("-----------------------------------------\n");
			printf("1 - Fijar la hora del reloj\n");
			printf("2 - Consultar la hora del reloj\n");
			printf("3 - Agregar evento del calendario\n");
			printf("4 - Quitar evento del calendario\n");
			printf("5 - Consultar lista de eventos activos\n");
			printf("6 - Salir\n");
}

void fijarHora(){
	printf("Opcion 1 - Fijar Hora del reloj\n");
	printf("Ingrese Una hora\n");
	getswf(hora)
	if (hora <= 0 || hora > 23 ){		// Valido que hora sea correcta
		printf("Valor Erroneo debe estar entre 0 y 23\n");
	}else{
		// FIJAR LA HORA EN EL RELOJ  y volver al menu
	}
}

void consultarHora(){
	printf("Opcion 2 - Consultar Hora del reloj\n");
	// Mostrar Hora
}

void agregarEvento(){

	while(1){
		printf("Opcion 3 - Agregar Evento en Calendario\n");
		printf("Tipo de Evento (1 o 2):\n");
		getswf(evento);
		if (evento < 0 || evento > 1){
			printf("Debe elegir 1 o 2, volver a ingresar\n");
		}else{
			printf("Led a Encender(0-6):\n");
			getswf(led);
			if (led < 0 || led > 6){
					printf("Debe elegir un numero entre  0 y 6");
			}else{
				printf("Ingrese Hora:\n");
				getswf(hora);
				if (hora <0 || hora>23){
					printf("Debe elegir un numero entre  0 y 23");

				}else{		// Configuro el evento con los datos ingresados
					
					eventos[0].command = evento;		// ver el tema de donde ingresar el evento dentro del array
					eventos[0].param = led;
					eventos[0].time = hora
				}
				
			}

		}

	}
}

void eliminarEvento(){
	// ver como ubicar el evento
}

void listarEventos(){
	int j;
	j = 0;
	for (i=0;i<MAX_EVENTS;i++){ // verificar final de datos para no recorrer todo el array
		printf("----------------------------------------:\n", );
		printf("EVENTO %s:\n", j);
		printf("Comando: %s\n", eventos[i].command);
		printf("LED : %s\n", eventos[i].param);
		printf("Hora: %s\n", eventos[i].time);

		j++;
	}
}

main(){
	char opcion;
	HW_init();

	while(1)
	{
		costate RED_LED always_on
		{
			LED_RED_SET();
			waitfor(DelayMs(ON_TIME));
			LED_RED_RESET();
			waitfor(DelayMs(OFF_TIME));
		}

		costate MENU always_on
		{
			printf("loop");
			waitfor(DelayMs(ONE_SECOND));

			mostrarMenuPrincipal();
			waitfor(getswf(opcionMenu));
			printf("\n");
			switch(opcionMenu){
				case("1"):					// Fijar hora del reloj
					fijarHora();
					break;

				case("2"):
					consultarHora();
					break;

				case("3"):
					agregarEvento();
					break;

				case("3"):
					eliminarEvento();
					break;

				case("3"):
					consultarEventos();
					break;

				default: printf("Vuelva a ingresar\n");

			}
		}

	}
}