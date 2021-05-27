#define S 0x01
#define T 0x02
#define R 0x03
#define A 0x04
#define H 0x05
#define I 0x6
#define V 0x07
#define K 0x08
#define Y 0x09
#define L 0x0A
#define O 0x0B
#define D 0x0C
#define M 0x0D
#define C 0x1A
#define _ 0x00

byte test;
bool isAddress = true;
byte address;
bool isCommand = false;
byte command;
byte data;

void setWriteModeRS485() {
  PORTD |= 1 << PD2;
  delay(1);
}

ISR(USART_TX_vect) {
  PORTD &= ~(1 << PD2); 
}

int writeData() {
  byte message[35] = {
    S, T, R, A, S, H, I, V, S, K, Y, I, _, 
    V, A, S, Y, L, _,
    V, O, L, O, D, Y, M, Y, R, O, V, Y, C, H,
    0x00, 0x00
  };

  byte reflected[33];
  for (int i = 0; i < 33; i++) {
    reflected[i] = ReverseByte(message[i]);
  }

  unsigned short checkSum = Compute_CRC16(reflected);
  byte firstByteOfCheckSum = (checkSum >> 8) & 0xFF;
  byte secondByteOfCheckSum = checkSum & 0xFF;

  message[33] = firstByteOfCheckSum;
  message[34] = secondByteOfCheckSum;

  for(int k = 0; k < 5; k++){
  for (int i = 0; i < 35; i++) {
    if(k==1 && i==2){
      test=message[i];
      Serial.write((7<<test));
   
      }
      else if (k==4 && i==3){
           test=message[i];
      Serial.write((3<<test)|(4<<test)|(5<<test));
        }
      else{
    Serial.write(message[i]);
      }
   
  }
  }
}

void setup() {
  delay(1000);

  DDRD = 0b00000111;
  PORTD = 0b11111000;

  Serial.begin(9600, SERIAL_8N1);
  UCSR0B |= (1 << UCSZ02) | (1 << TXCIE0);
  UCSR0A |= (1 << MPCM0);

  delay(1);

  address = 0xDE;
}

void loop() {
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

  for (int b = 0; b < 33; b++) {
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
