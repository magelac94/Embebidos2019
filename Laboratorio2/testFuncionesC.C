#use IO.lib
#use EVENTOS.lib
#use RTC.lib
#use Utilities.lib


#define TEN		10
char entrada[10];
struct tm FechaHora;
Event unEvento;
Event arrayEventos[10];
unsigned long timestamp;

main(){

	printf("\nPuntero a unEvento: %p", &unEvento);
	printf("\nPuntero a arrayEventos: %p", &arrayEventos);
	printf("\nPuntero a primer elemento de arrayEventos: %p", &arrayEventos[0]);

	// Seteo los atributos de unEvento
	unEvento.command = 0x4;
	unEvento.param = 0x4;

	FechaHora.tm_hour = 25;
	FechaHora.tm_min = 18;
	FechaHora.tm_mday = 10;
	FechaHora.tm_mon = 4;
	FechaHora.tm_year = 119;

	timestamp = mktime(&FechaHora);
	unEvento.time = timestamp;

	printf("\nImprimo el evento unEvento: ");
	printEvento( &unEvento );

	printf("\nImprimo el primer evento del arrayEventos: ");
	printEvento( &arrayEventos[0] );

	arrayEventos[0].command = unEvento.command;
   arrayEventos[0].param = unEvento.param;

   printf("\n\n------------------------------------ ");
	printf("\nImprimo el evento unEvento: ");
	printEvento( &unEvento );

	printf("\nImprimo el primer evento del arrayEventos: ");
	printEvento( &arrayEventos[0] );

	unEvento.command = 0x8;

   printf("\n\n------------------------------------ ");
   printf("\nImprimo el primer evento del arrayEventos: ");
	printf("\nImprimo el evento unEvento: ");
	printEvento( &unEvento );

	printf("\nImprimo el primer evento del arrayEventos: ");
	printEvento( &arrayEventos[0] );



	//HW_init();

	// Prueba de lectura de RTC
	//RTC_leerFechaHora( &FechaHora );	// Leo el RTC
	//printFechaHora( &FechaHora );		// Imprimo la Fecha y hora

	// Reuso la estructura fecha hora donde lei la hora actual
	//FechaHora.tm_hour = 1;
	//FechaHora.tm_min = 23;
	//FechaHora.tm_mday = 6;
	//FechaHora.tm_mon = 4;
	//FechaHora.tm_year = 19;
	//RTC_fijarFechaHora( &FechaHora );
 //  costate {
  // 	   waitfor(DelayMs(TEN));
  // }
//   printFechaHora( &FechaHora );		// Imprimo la Fecha y hora modificados

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