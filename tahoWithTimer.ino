
// 7-и сегментник
// храним в массиве все цифры
static uint8_t d_bits[12] = {0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0xFF,0xBF};   // цифры без точки
static uint8_t d_bits_pt[12] = {0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0xFF,0x3F}; // цифры с точкой
static uint8_t seg[4]= {0xE3,0xE5,0xE9,0xF1}; // сегмент
byte digit[4] = {0,0,0,0}; // цифры в разрядах 7-и сегментника
byte nSeg = 0; 
unsigned int num = 0; // выводимое число 0-9999


unsigned int rpm = 0;     // обороты в минуту
float oldtime=0; // 
float time=0;    // 

unsigned int rpm_array[10] = {0,0,0,0,0}; // для смягчения показаний RPM

//-----------------------------------------------------------------------------------------
void setup()
{
  //пины для здвиговых регистров - hardware SPI (ЗАДАЕМ ЧЕРЕЗ ПОРТЫ) (просто извращаемся на низком уровне програмирования =))))
  DDRB |= (1<<5);    // устанавливаем вывод PB5 как выход pin 13 (clock) - hardware SPI // SCK - hardware SPI (SH - подключен на 13 пин)
  DDRB |= (1<<2);    // устанавливаем вывод PB2 как выход pin 10 (latch) - hardware SPI // провод CS (ST) подсоединяем к 8-му пину
  DDRB |= (1<<3);    // устанавливаем вывод PB3 как выход pin 11 (data) - hardware SPI  // MOSI - hardware SPI (DS - подключен на 11 пин)
  PORTB &= ~(1<<5);  // устанавливаем низкий уровень на выводе PB5 pin 13 (clock) - hardware SPI
  PORTB &= ~(1<<2);  // устанавливаем низкий уровень на выводе PB2 pin 10 (latch) - hardware SPI
  PORTB &= ~(1<<3);  // устанавливаем низкий уровень на выводе PB3 pin 11 (data) - hardware SPI

  setup_hardware_spi(); // настройка SPI
  delay(10);
  setup_timer2_ovf(); // запускаем прерывание по второму таймеру
  delay(100);
  attachInterrupt(0,RPM1,FALLING);  //установка прерывания на втором цифровом пине
  delay(100);
}

void loop()
{
  delay(200);

  rpm_array[0] = rpm_array[1];
  rpm_array[1] = rpm_array[2];
  rpm_array[2] = rpm_array[3];
  rpm_array[3] = rpm_array[4];
  rpm_array[4] = rpm_array[5];
  if((micros() - oldtime)<1000000){ // что бы обнулять показатания когда нет сигнала дольше 1 сек
    rpm_array[4] = 60000000/time; // вычисляем PRM если сигнал чаще чем раз в 1 секунду  
  } else {
    rpm_array[4] = 0;
  }
  rpm = (rpm_array[0] + rpm_array[1] + rpm_array[2] + rpm_array[3] + rpm_array[4]) / 5;

  num = rpm;
  num = int(num/10)*10; // округление до 10-х
  num_print();
}

//-----------------------------------------------------------------------------------------
// Обработка прерывания по таймеру 2 
ISR(TIMER2_COMPA_vect) { // Обработчик прерывания для вектора прерывания по таймеру (когда счетчик проходит значение OCR2A)
  // отправляем байты на семисегментник
  if(nSeg>3) nSeg=0; 
  PORTB &= B11111011; // устанавливает LOW на цифровом выводе 10 (ЗАДАЕМ ЧЕРЕЗ ПОРТЫ) (типа так на много быстрее) это то же что digitalWrite(10 ,LOW);
  spi_transfer(d_bits[digit[nSeg]]); spi_transfer(seg[nSeg]); 
  PORTB |= B00000100; // устанавливает HIGH на цифровом выводе 10 (ЗАДАЕМ ЧЕРЕЗ ПОРТЫ) (типа так на много быстрее) это то же что digitalWrite(10 ,HIGH);
  nSeg++; 
}

//-----------------------------------------------------------------------------------------
void RPM1() // вычисляем промежуток времени
{
   time = micros() - oldtime; 
   oldtime = micros();
}

//-----------------------------------------------------------------------------------------
void num_print() // выводим число из переменной num на 7-и сегментник
{
  digit[0] = num/1000%10; // сотни
  digit[1] = num/100%10; // десятки
  digit[2] = num/10%10; // единицы
  digit[3] = num%10; // единицы
}

//-----------------------------------------------------------------------------------------
// ПРЕРЫВАНИЯ
//-----------------------------------------------------------------------------------------
// Настойка таймера №2
void setup_timer2_ovf(void) {
  TCCR2A = (1<<WGM21);    // Режим CTC (сброс по совпадению) (счётчик после совпадения с регистром сравнения будет сбрасываться автоматически)
//  TCCR2A &= ~( (1<<WGM21) | (1<<WGM20) ); // другой режим (не нашел что он означает)

//  TCCR2B = (1<<CS20);     // Тактирование от CLK. Тактировать без деления частоты.
                                           // Если нужен предделитель :
//   TCCR2B = (1<<CS21);                     // Тактировать с коэффициентом деления 8.
//   TCCR2B = (1<<CS20)|(1<<CS21);           // Тактировать с коэффициентом 32.
//   TCCR2B = (1<<CS22);                     // Тактировать с коэффициентом 64.
//   TCCR2B = (1<<CS20)|(1<<CS22);           // Тактировать с коэффициентом 128.
//   TCCR2B = (1<<CS21)|(1<<CS22);           // Тактировать с коэффициентом 256.
   TCCR2B = (1<<CS20)|(1<<CS21)|(1<<CS22); // Тактировать с коэффициентом 1024.
  
  OCR2A = 55;            // Регистр сравнения (количество тактов до совпадения). Диапазон от 0 до 255.
                          // Частота прерываний будет = Fclk/(N*(1+OCR2A)) =>>>>>>> 16000000 / (1024*(1+255)) = 61.04 Гц 
                          // где N - коэф. предделителя (1, 8, 32, 64, 128, 256 или 1024)
                          // для 7-и сегментника значение для OCR2A надо в 4 раза меньше из-за того, что он 4-х рахрядный. То есть, для 61.04 Гц вместо 255 надо 63 (((255+1)/4)-1)
                          // Что бы глаза не уставали желательно что бы частота обновления 7-и сегментника была около 70 Гц. Для этого OCR2A = 55 (69.75 Гц)
  TIMSK2 = (1<<OCIE2A);   // Разрешить прерывание по совпадению Т2
  SREG |= (1<<7);         // Глобально разрешить прерывания
}

//-----------------------------------------------------------------------------------------
// Отправка байта по SPI
byte spi_transfer(byte data)
{
  SPDR = data;                    // начало передачи
  while (!(SPSR & (1<<SPIF)))     // ждем окончания передачи
  {
  };
  return SPDR;                    // возврат полученных байт (они не нужны, но просто так надо)
}

// Настройка SPI
void setup_hardware_spi(void) {
  // spi предделитель:
  // SPI2X SPR1 SPR0
  //   0     0     0    fosc/4
  //   0     0     1    fosc/16
  //   0     1     0    fosc/64
  //   0     1     1    fosc/128
  //   1     0     0    fosc/2
  //   1     0     1    fosc/8
  //   1     1     0    fosc/32
  //   1     1     1    fosc/64
  SPCR |= ( (1<<SPE) | (1<<MSTR) ); // включить SPI как ведущий
  SPCR &= ~ ( (1<<SPR1) | (1<<SPR0) ); // clear prescaler bits
  SPSR |= (1<<SPI2X); // set prescaler bits
}