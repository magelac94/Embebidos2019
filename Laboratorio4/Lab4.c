#use IO.LIB
#use ucos2.LIB
#use LED.LIB

void Red_led(){
while(1){
	LED_RED_SET();
	OSTimeDlySec(1);
	LED_RED_RESET();
	OSTimeDlySec(1);
   }
}


main(){
	HW_init();
	OSInit();

	OSTaskCreate(Red_led, NULL, 256, 4);


	OSStart();
}