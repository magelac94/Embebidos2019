#use funcIO.LIB	//Deben incluir la ruta completa del directroio "../lib/" en el archivo de LIB.DIR


main()
{
	char i;
	int aux;

	Init_Placa(9600);
	
	for( i=0; i<=8; i++ ) 	//Incluyo el LED de estado
	{
		Wr_Output(i, 1);
		Delay_ms(500);
		Wr_Output(i, 0);
	}
	
	while(1)
	{
		ClearScreen();
		for( i=0; i<=1; i++ ) 	//Incluyo el LED de estado
		{
			
			printf("AN%d = %d\n",i,Rd_Analog(i));
			
		}
		
		LEDstatus(-1);
		Delay_ms(1000);
	}
}