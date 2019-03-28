#define LEDXON(x) setOutput( PuertoParaleloA, x , 1 )
#define LEDXOFF(y) setOutput( PuertoParaleloA, y , 0 )
#define BUTTOMSTATE(x) getInput(PuertoParaleloB, x )
#define BUTTOMBOOLEAN(y) getInput(PuertoParaleloB,y )    // terminar el if

enum portName {
	PuertoParaleloA = PADR ,
   PuertoParaleloB = PBDR
};

// Este enum podria ser imlicito, es decir, no poner a que vale cada uno
// Sin definirlo arranca siempre en 0 y va aumentando en 1 en orden.
enum bitNumber {
	PA0 = 0,
   PA1 = 1,
   PA2 = 2,
   PA3 = 3,
   PA4 = 4,
   PA5 = 5,
   PA6 = 6,
   PA7 = 7
};

enum bitNumberButtoms {
	PB2 = 0,
   PB3,
   PB4,
   PB5,
   PF4,
   PF5,
   PF6,
   PF7
};

// Funcion para configurar leds como OUTPUT
void configurarLEDs(){
	WrPortI(SPCR, &SPCRShadow, 0x084); //Rabbit3000 user manual - seccion 9.1
}
void configurarBotones(){
	WrPortI(PBDDR, &PBDDRShadow, 0x000	); //Rabbit3000 user manual - pag 139
}



// Esta es la funcion pedida para LED
void setOutput( enum portName p_port, enum bitNumber p_pin, unsigned char p_state){

 switch(p_port){
  	case (PuertoParaleloA):
     BitWrPortI(p_port, &PADRShadow, p_state , p_pin);
     break;
  }
}

// Esta es la funcion pedida para BOTONES
char getInput(enum portName p_port, enum bitNumberButtoms p_pin){
	char resultado;
	resultado = BitRdPortI(p_port, p_pin) ;
   return resultado;
}



// Esta funcion me parece mas util o intuitiva
void PuertoALedOn(int led) {
	BitWrPortI(PADR, &PADRShadow, 0, led);
}

// Gastar ciclos
void delay1(){
	int i;
	for( i = 0; i < 32767; i++ ) {
   	i * i;
   };
}

// Main
int main(){

	// configuracion de puertos
	configurarLEDs();
   configurarBotones();

   // Pruebas LEDs
   LEDXON(PA0);
   delay1();
   LEDXON(PA1);
   delay1();
   LEDXON(PA2);
   delay1();
   setOutput( PuertoParaleloA, PA3, 1);
   delay1();
   setOutput( PuertoParaleloA, PA4, 1);
   delay1();
   setOutput( PuertoParaleloA, PA5, 1);
   delay1();
   setOutput( PuertoParaleloA, PA6, 1);
   delay1();
   setOutput( PuertoParaleloA, PA7, 1);
   delay1();
   LEDXOFF(PA0);
   delay1();

   // Pruebas Botones
   while(1){
	   delay1();
   	printf("\nPuerto: %d, valor: %d",PB2, getInput(PuertoParaleloB, PB2 ));

   }

  	// Pruebas Potenciometro

   /*
	 r = serAopen(9600);
    if (r==1){
    	printf("Es 1");
   } else   {
    	printf("Es 0");
      }
      */


   while(1);
}