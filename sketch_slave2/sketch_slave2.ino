#define ZERO 0x00
#define ONE  0x01 
#define TWO  0x02 
#define FOUR 0x04
#define DOT  0xFF

byte test;
bool isAddress = true;
bool isCommand = false;
byte address;
byte command;
byte data;

void writeData() {
  byte message[12] = {
    ONE, FOUR, DOT, 
    ZERO, ONE, DOT, 
    TWO, ZERO, ZERO, TWO, 
    0x00, 0x00
  };

  byte reflected[10];
  for (int i = 0; i < 10; i++) {
    reflected[i] = ReverseByte(message[i]);
  }

  unsigned short checkSum = Compute_CRC16(reflected);
  byte checkSumFirstByte = (checkSum >> 8) & 0xFF;
  byte checkSumSecondByte = checkSum & 0xFF;

  message[10] = checkSumFirstByte;
  message[11] = checkSumSecondByte;
for(int k = 0; k < 5; k++){
  for (int i = 0; i < 12; i++) {
    if(k==2 && i==0){
      test=message[i];
      Serial.write((test<<1));
   
      }
      else if (k==3 && i==3){
           test=0x03;
      Serial.write(test);
        }
      else{
    Serial.write(3[i]);
      }
  }
  }
}

void setWriteModeRS485() {
  PORTD |= 1 << PD2;
  delay(1);
}

ISR(USART_TX_vect)
{
  PORTD &= ~(1 << PD2); 
}

void setup() {
  delay(1000);

  DDRD = 0b00000111;
  PORTD = 0b11111000;

  Serial.begin(9600, SERIAL_8N1);
  UCSR0B |= (1 << UCSZ02) | (1 << TXCIE0);
  UCSR0A |= (1 << MPCM0);

  delay(1);

  address = 0x7B;
}

void loop() {
  data = (~PIND);
  data = data >> 3;
  if (Serial.available()) {
    byte inByte = Serial.read();
    if (isAddress) {
      if (address == inByte) {
          isAddress = false;
          isCommand = true;
          UCSR0A &= ~(1 << MPCM0);
        }
    } else if (isCommand) {
      command = inByte;
      isCommand = false;
      if (command = 0xB1) {
          isAddress = true;
          setWriteModeRS485();
          writeData();
        }
    }
  }
}



unsigned short Compute_CRC16(byte* bytes) {
  const unsigned short generator = 0x589;
  unsigned short crc = 0x0000;
  for (int b = 0; b < 10; b++) {
    crc ^= (unsigned short) (bytes[b] << 8);

    for (int i = 0; i < 8; i++) {
      if ((crc & 0x8000) != 0) {
        crc = (unsigned short) ((crc << 1) ^ generator);
      } else {
        crc <<= 1;
      }
    }
  }
  unsigned short myNewResult = Reflect16(crc);
  return myNewResult;
}


unsigned short Reflect16(unsigned short val) {
  unsigned short resVal = 0;
  for (int i = 0; i < 16; i++) {
    if ((val & (1 << i)) != 0) {
      resVal |= (unsigned short)(1 << (15 - i));
    }
  }
  return resVal;
}

byte ReverseByte(byte b) {
  int a = 0;
  for (int i = 0; i < 8; i++) {
    if ((b & (1 << i)) != 0) {
      a |= 1 << (7 - i);
    }
  }
  return (byte) a;
}

int CombineBytes(byte b1, byte b2) {
  int combined = b1 << 8 | b2;
  return combined;
}
