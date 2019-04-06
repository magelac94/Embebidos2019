#use IO.lib
#use RTC.lib
#use Utilities.lib

#define TEN		10
char* p_entrada;
struct tm* p_FechaHora;
main(){

	HW_init();

	// Prueba de lectura de RTC
	RTC_leerFechaHora( p_FechaHora );	// Leo el RTC
	printFechaHora( p_FechaHora );		// Imprimo la Fecha y hora

	// Reuso la estructura fecha hora donde lei la hora actual
	p_FechaHora->tm_hour = 1;
	p_FechaHora->tm_min = 23;
	p_FechaHora->tm_mday = 6;
	p_FechaHora->tm_mon = 4;
	p_FechaHora->tm_year = 19;
	RTC_fijarFechaHora( p_FechaHora );
   costate {
   	   waitfor(DelayMs(TEN));
   }
   printFechaHora( p_FechaHora );		// Imprimo la Fecha y hora modificados

	// Prueba de insertar eventos


	while(1){

	};

////	Test basico de comunicacion con consola serial
//	while(1){
//		printf("Comienza el loop...\n");
//		printf("Espero por entrada: ");
//		gets( p_entrada );
//		printf("\nLa entrada fue: %s", p_entrada);
//      printf("\n");
//	}
}