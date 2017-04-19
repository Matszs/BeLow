//PIR
#define PIR_Sensor 10 //PIR handles wake-up interrupts, therefore attached to pin 10

//LED
#define LED 13
void setup() {
  //set all unused digital pins to output
  for (int i = 0; i<17; i++){
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }
  
  //PIR
  pinMode(PIR_Sensor, INPUT);
  digitalWrite(LED, LOW);
  attachInterrupt(PIR_Sensor, wake, HIGH);
  NVIC_EnableIRQ((IRQn_Type)0x02);
  
  

  
  SerialUSB.begin(9600);
  delay(10000);//temporary delay for testing, do not remove!!
  sleepDeep();

}

int inactivity_Counter = 0;
bool awake = true;

void loop() {
inactivity_Counter++;
delay(1);
if(inactivity_Counter==100000){
  inactivity_Counter = 0;
  sleepDeep();
}

  

}

void sleepDeep(){
  digitalWrite(LED, LOW);
  awake = false;

  USB->DEVICE.CTRLA.reg &= ~USB_CTRLA_ENABLE;
  PM->AHBMASK.reg &= ~PM_AHBMASK_USB;
  PM->APBBMASK.reg &= ~PM_APBBMASK_USB;

 SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
  
 __WFI();
}
void wake(){
  noInterrupts();
  if (!awake){
    wakeUSB();
   }
  inactivity_Counter = 0;
  interrupts();
  awake =true;
  }

void wakeUSB(){

  digitalWrite(LED, HIGH);
  PM->APBBMASK.reg |= PM_APBBMASK_USB;
  PM->AHBMASK.reg |= PM_AHBMASK_USB;
  USB->DEVICE.CTRLA.reg |= USB_CTRLA_ENABLE;

}



