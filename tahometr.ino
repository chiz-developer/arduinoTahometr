//17.12.15 - вывод готового алгоритма в лед-дисплей
int inPin = 10;
int cnt = 0;
static unsigned long prevTime = 0;
static unsigned long currTime = 0;
//---------------------------
int latchPin = 12;//защелка
int clockPin = 13;//тактовый
int dataPin = 11;//данные


void digit(int numb, int rang){
  byte numbs[10] = {
    B11000000, B11111001, B10100100, B10110000, B10011001, B10010010, B10000010, B11111000, B10000000, B10010000    };
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
  delay(6);

}

void disp(int number){
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

void setup(){
  Serial.begin(9600);

  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT); 
  
  pinMode(inPin, INPUT);

}

void loop(){

 static uint8_t prevSt = LOW;

 uint8_t currSt = digitalRead(inPin);

 if (currSt != prevSt){
    prevSt = currSt;
    
    if(currSt == HIGH){
      prevTime = currTime;
      currTime = millis();
    }
 }
 if(millis()-prevTime > 2000){
   cnt = 0;
 }else{
   cnt = 1000/(currTime-prevTime);
 }
 disp(cnt);
}//end loop



