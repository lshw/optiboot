uint16_t addr;
#include "optiboot.h"
#define FLASH_BLK_NUMS 124

uint8_t buff[SPM_PAGESIZE];
void write_test() {
  memset(buff, 0xff, sizeof(buff));
  strcpy(buff, "liushiwei"); //填充
  Serial.flush();
  optiboot_writePage(buff, 125); //写第125块
  Serial.println("write buff to block 125");
  Serial.flush();

  memset(buff, 0xff, sizeof(buff)); //缓冲区清零
  Serial.println("clear buff");
  Serial.flush();
  disp_buff();
  optiboot_readPage(buff, 125); //读125块
  Serial.println("read block 125 to buff");
  Serial.flush();
  disp_buff();
}
void copy_low_to_high() {
  for (uint8_t i = 0; i < 124; i++) {
    optiboot_readPage(buff, i); //读块
    optiboot_writePage(buff, 124 + i); //写块
  }
}
void setup() {
  Serial.begin(38400);
  Serial.print("pageSize=");
  Serial.println(SPM_PAGESIZE);
  //write_test();
  copy_low_to_high();//复制程序从低位地址，到高位地址
  optiboot_readPage(buff, 124);//读取高位地址的第一块

  disp_buff();
  addr = buff[23 * 4 + 3] << 8; //得到int23的地址
  addr += buff[23 * 4 + 2];
  Serial.print("low int23 address=");
  Serial.print(addr << 1);
  Serial.println(",call it....");
  asm("icall"::"z"(addr)); //调用低rom的int23地址
  Serial.println("ok");
  addr = addr + 124 * 128 / 2; //换算成对应的高位地址
  Serial.print("high int23 address=");
  Serial.print(addr << 1);
  Serial.println(",call it....");
  asm("icall"::"z"(addr)); //调用高rom的int23地址
  Serial.println("ok");
  Serial.flush();
}
void disp_buff() {
  String disp = "";
  for (uint8_t i = 0; i < 128; i++) {
    if (i % 16 == 0) {
      if (i > 0)  Serial.println(disp);
      disp = " ";
    }
    Serial.write(' ');
    if (buff[i] > ' ' &&  buff[i] < 0x80) {
      Serial.print(buff[i], HEX);
      disp += (char) buff[i];
    } else {
      if (buff[i] < 0x10) Serial.write('0');
      Serial.print(buff[i], HEX);
      disp += '.';
    }
  }
  Serial.println(disp);
  Serial.println();
}

void loop() {
  // put your main code here, to run repeatedly:
}

ISR(_VECTOR(23)) {
  Serial.println("int23");
  Serial.flush();

}
