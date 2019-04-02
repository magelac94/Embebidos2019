#use IO.LIB
#use BTN.LIB
#use LED.LIB
#use UTILITIES.LIB

#define ON_TIME		400
#define OFF_TIME	800

main()
{
	HW_init();

	while(1)
	{
		costate
		{
			LED_RED_SET();
			waitfor(DelayMs(ON_TIME));
			LED_RED_RESET();
			waitfor(DelayMs(OFF_TIME));			
		}
	}
}
