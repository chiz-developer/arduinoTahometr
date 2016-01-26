//17.12.15 - вывод готового алгоритма в лед-дисплей
int inPin = 4;
int cnt = 0;
static unsigned long prevTime = 0;
static unsigned long currTime = 0;
//---------------------------
int latchPin = 12;//защелка
int clockPin = 13;//тактовый
int dataPin = 11;//данные


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
  //delay(6);

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
   cnt = (60000/(currTime-prevTime));
 }
 //disp(cnt);
 dispF(13.09);
}//end loop



