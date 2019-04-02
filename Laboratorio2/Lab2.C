#use IO.LIB
#use BTN.LIB
#use LED.LIB
#use UTILITIES.LIB

#define ON_TIME		400
#define OFF_TIME	800
#define ONE_SECOND	1000

main()
{
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
		}
	}
}
