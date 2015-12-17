//17.12.15
//Проверка работы алгоритма через сериал порт
int inPin = 10;
int cnt = 0;
static unsigned long prevTime = 0;
static unsigned long currTime = 0;

void setup()
{
 Serial.begin(9600);
}

void loop()
{
 static uint8_t prevSt = LOW;

 uint8_t currSt = digitalRead(inPin);

 if (currSt != prevSt){
    ++cnt;
    prevSt = currSt;
    prevTime = currTime;
    currTime = millis();
 }
 Serial.println((1000/(currTime-prevTime))/2);
 
}
