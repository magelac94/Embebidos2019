#define STACK_CNT_256 3
#define STACK_CNT_512 3
#define OS_TASK_DEL_EN 1

#use IO.LIB
#use LED.LIB
#use MENU.LIB
#use UTILITIES.LIB
#use RTC.lib
#use ucos2.LIB

void Led_Red(){
while(1){
	LED_RED_SET();
	OSTimeDlySec(1);
	LED_RED_RESET();
	OSTimeDlySec(1);
   }
}


void ProgramaPrincipal(void* pdata){
	struct tm FechaHora;
	char opcion_menu;
	int int_opcion_menu;
    enum tipoUI interfazAUsar;
    interfazAUsar = *(int*)pdata;

	MENU_mostrarMenuPrincipal( interfazAUsar );
	MENU_obtenerOpcion( interfazAUsar, &opcion_menu );
	int_opcion_menu = atoi( &opcion_menu );

	switch( int_opcion_menu ){
		case( OPCION_1 ):
			MENU_pedirFechaHora( &FechaHora, interfazAUsar );
			RTC_fijarFechaHora( &FechaHora );
			MENU_printFechaHora( &FechaHora, interfazAUsar );		// Imprimo la Fecha y hora modificadoS

			break;
		default:
			// OPCION INCORRECTA
			printf("\nOpcion DEFAULT: %d \n", int_opcion_menu);
			printf("Vuelva a ingresar\n");
		}
  }




main(){

	int parametro;

	HW_init();
	OSInit();



	// Tarea 1 Prende LED
	OSTaskCreate(Led_Red, NULL, 256, 4);

	parametro = CONSOLA;
	// Tarea 2 Mostrar Menu Para Consola
	OSTaskCreate(ProgramaPrincipal , &parametro, 512, 3);
   //   OSTaskDel(3);

	// Tareas 3 Mostrar Menu Por TCP


	OSStart();
}