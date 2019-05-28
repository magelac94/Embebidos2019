#define GPRS_BAUD_RATE	9600
#define GPRS_MSG_LENGTH 120
#define DINBUFSIZE 				127
#define DOUTBUFSIZE 			127

#use Utilities.lib
brubelm@gmail.com
void main(){
	cont static char s1[] = "A"
	cont static char s1[] = "AT\r"
	
	serDopen(GPRS_BAUD_RATE);
	serDrdFlush();
	serDwrFlush();

	serDputs(s1);
	UT_delay(6000);
	serDputs(s2);
	UT_delay(1000);	
}

el modem puede morir en cualquier momento asi que tengo que ir chequeando constantemente
antes de cada envio y recepcion si el status es OK.

Para registro en la red preguntarle AT+CPIN?\r y vamos a recibir dos tipos de respuesta, una va a ser READY (no necesito pin)o puede responder SIMPIN (si necesita el pin). Si todo esto sale bien recibimos un OK.

Luego tenemos que ver si con eso es suficiente para registrarnos en la red. PAra logarlo AT+CREG?\r. Si estamos registrados en la red recibimos AT+CREG:0,1. Ver que son estos codigos 0,1 por que es posible que necesite un tiempito para terminar de regsitrarse. Si no estamos registrados tenemos que setearle la operadora AT+COPS=coddeantel\r

Luego de estar registrados, mandar sms. SEtear el modem en modo texto. AT+COMANDOMODOTEXTO y luego mandar AT+MSG\r ALT+0026 (es el ctrl+z)

Encapsular para que quede prolijo. En base a la respuesta hacer el caso correcto. No avanzar si no recibimos las respuestas correctas. Robusto para que si el modem se apaga vuelva a encenderse.