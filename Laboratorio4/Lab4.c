#use IO.LIB
#use ucos2.LIB
#use LED.LIB
#use MENU.LIB

enum opcionMenu {
	OPCION_0 = 0,
	OPCION_1,
	OPCION_2,
	OPCION_3,
	OPCION_4,
	OPCION_5,
	OPCION_6,
	OPCION_7
};

enum tipoUI {
	CONSOLA = 0,
	TCP
};


void Led_Red(){
while(1){
	LED_RED_SET();
	OSTimeDlySec(1);
	LED_RED_RESET();
	OSTimeDlySec(1);
   }
}

void ProgramaPrincipal(enum tipoUI interfazAUsar){
	 //	OSTaskCreate(MENU_mostrarMenuPrincipal, NULL, 256, 5);
//   MENU_mostrarMenuPrincipal( interfazAUsar );

}


main(){
	HW_init();
	OSInit();

	// Tarea 1 Prende LED
	OSTaskCreate(Led_Red, NULL, 256, 4);

	// Tarea 2 Mostrar Menu Para Consola
	OSTaskCreate(ProgramaPrincipal, NULL, 256, 3);


	OSStart();
}