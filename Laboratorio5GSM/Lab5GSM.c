//Estos define los tenemos que mover a la lib IO
#define GPRS_BAUD_RATE	9600
#define GPRS_MSG_LENGTH 120
#define DINBUFSIZE 		127
#define DOUTBUFSIZE 	127
#define TIMEOUT_10		10

#use IO.lib
#use Utilities.lib


void main(){
	
	int status;
	char dataRx[GPRS_MSG_LENGTH];
	
	// Configuramos STATUS asi despues podemos leer el pin
	BitWrPortI(PEDDR, &PEDDRShadow, INPUT_DIR, BIT_1); 
	
	// Configuramos puerto serial D
	serDopen(GPRS_BAUD_RATE);
	serDrdFlush();
	serDwrFlush();
	/*
	Prener el modem. Esto lo hacemos
	configurando el pin que conecta el KEY como salida, esperamos y
	luego lo configuramos como entrada y esperamos nuevamente.
	*/
	status = IO_getInput(PORT_E, BIT_1);

	while(!status){
		BitWrPortI(PEDDR, &PEDDRShadow, OUTPUT_DIR, BIT_4);
		IO_setOutput(PORT_E,0,BIT_4);
		UT_delay(2000);
		BitWrPortI(PEDDR, &PEDDRShadow, INPUT_DIR, BIT_4);
		UT_delay(2000);
		status = IO_getInput(PORT_E, BIT_1);
	}
	
	serDputs("A");
	UT_delay(6000);
	serDputs("AT\r");
	UT_delay(1000);
	
	do {
		UT_delay(100)
	}while(!serDrdUsed());

   serDread(dataRx, sizeof(dataRx), TIMEOUT_10);
   printf("Respuesta del modem: %s", dataRx);
   //printf(dataRx); si sale basura del printf de arriba, ver con este a ver si es el tipo de dato %s el problema
   
   /* 
	Dicen que tenemos que chequear siempre antes de operar que el modem no este muerto.
	No se si aca antes de intentar ver si estamos registrados, en este caso de prueba vale la pena.
	*/
	serDputs("AT+CPIN?\r");
	UT_delay(1000);
	serDread(dataRx, sizeof(dataRx), TIMEOUT_10);
	printf("Respuesta del modem: %s", dataRx);
	
}


/* Funcion para prender el modem. De esta forma nos aseguramos
	que antes de usarlo, este encendido. Capaz que puede llamarse diferente.
	O podriamos tener una funcion que se llame checkStatus que mire si esta vivo, y
	si esta apagado, llama a esta de modemON
	*/
int modemON(){	
	int status;
	
	status = IO_getInput(PORT_E, BIT_1);
	while(!status){
		BitWrPortI(PEDDR, &PEDDRShadow, OUTPUT_DIR, BIT_4);
		IO_setOutput(PORT_E,0,BIT_4);
		UT_delay(2000);
		BitWrPortI(PEDDR, &PEDDRShadow, INPUT_DIR, BIT_4);
		UT_delay(2000);
		status = IO_getInput(PORT_E, BIT_1);
	}
	return status;
}