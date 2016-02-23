//13.02.16 - +ВОЛЬТМЕТР
int inPin = 4;
int cnt = 0;
static unsigned long prevTime = 0;
static unsigned long currTime = 0;

static unsigned long prevDispTime = 0;
static unsigned long currDispTime = 0;
//---------------------------
float R1 = 100000.0; // сопротивление R1 (100K)
float R2 = 23000.0; // сопротивление R2 (17K)
int value = 0;
float vout = 0.0;
float vin = 0.0;
int swPin = 0;
//---------------------------
int latchPin = 12;//защелка
int clockPin = 13;//тактовый
int dataPin = 11;//данные
//===========================

void multimetr(){
  value = analogRead(0);
  vout =  (5.0 / 1024.0) * value;
   
  
  if( millis()-prevDispTime > 2000) {
     vin = vout / (R2/(R1+R2));
     prevDispTime = millis(); 
  }
  dispF(vin);

}

void digit(int numb, int rang){
  byte numbs[11] = {
    B11000000, B11111001, B10100100, B10110000, B10011001, B10010010, B10000010, B11111000, B10000000, B10010000, B01111111    };
  byte rangs[4] = {
    B00000001, B00000010, B00000100, B00001000    };
  //выключаю индикатор
  digitalWrite(latchPin, LOW);

  //число
  shiftOut(dataPin, clockPin, MSBFIRST, numbs[numb]);
  //разряд
  shiftOut(dataPin, clockPin, MSBFIRST, rangs[rang-1]);

  //включаю индикатор
  digitalWrite(latchPin, HIGH);

}

void dispInt(int number){
  int one = 0;
  int ten = 0;
  int hnd = 0;
  int thd = 0;

  //тысячи
  if(number > 999){

    thd = number / 1000;
    hnd = (number % 1000) / 100;
    ten = (number % 100) / 10;
    one = number % 10;

    digit(thd, 1); 
    digit(hnd, 2);
    digit(ten, 3); 
    digit(one, 4);
  }
  else if(number < 1000 && number > 99){ // сотни
    hnd = (number % 1000) / 100;
    ten = (number % 100) / 10;
    one = number % 10;

    digit(hnd, 2);
    digit(ten, 3);
    digit(one, 4);   

  }
  else if(number < 100 && number > 9){ //десятки
    ten = (number % 100) / 10;
    one = number % 10;
   
    digit(ten, 3);
    digit(one, 4);    

  }
  else if(number < 10){
    digit(number, 4); 
  }

}

void dispF(float number){
    int a = round(number*100) / 100;
    int b = round(number*100) % 100;
    int one = 0;
    int ten = 0;

    if(a < 10) { //целая часть 0-9
      digit(a,2);
      digit(10,2); //точка
      }
    else if(a > 9 && a < 100){ //целая часть 10-99
      ten = (a % 100) / 10;
      one = a % 10;
     
      digit(ten, 1);
      digit(one, 2);
      digit(10,2);    
      }

    if(b < 10) { //дробная часть 0-9
      digit(0,3);
      digit(b,4);
      }
    else if(b > 9 && b < 100){ //дробная часть 10-99
      ten = (b % 100) / 10;
      one = b % 10;
     
      digit(ten, 3);
      digit(one, 4);   
      }
  }

void setup(){
  Serial.begin(9600);

  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT); 
  
  pinMode(inPin, INPUT);
  pinMode(3, INPUT);//VOLTMETR

}

void loop(){

  swPin = digitalRead(3);
  if(swPin == 0) {
    multimetr();
  }else{
    //TAHOMETR

     static uint8_t prevSt = LOW;
     uint8_t currSt = digitalRead(inPin);
    
     if (currSt != prevSt){
        prevSt = currSt;
        
        if(currSt == HIGH){
          prevTime = currTime;
          currTime = millis();
        }
     }
    
     if( millis()-prevTime > 2000){
        cnt = 0;
      }else if( millis()-prevDispTime > 250) {
        //update cnt
        prevDispTime = millis();
        cnt = 2*(60000/(currTime-prevTime));
        cnt = int(cnt/100)*100;
      }
     
     dispInt(cnt);
     //END TAHOMETR
    
  }


}//end loop





