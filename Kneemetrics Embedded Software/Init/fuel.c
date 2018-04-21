#include "Fuel.h"

#define i2c_write_fuel   Fuel_I2C_WriteRegister
#define i2c_read_fuel    Fuel_I2C_ReadRegister

char HAL_FuelGaugeVoltage() {
  char vcell[2];

  i2c_read_fuel(MAX17048G_ADDRESS,0x02,2,vcell);
  //vcell = vcell >> 4;  // last 4 bits of vcell are nothing
  return vcell;//(vcell / 805.0f * 100);
}
/*
int HAL_FuelGaugePercent() {
  unsigned int soc;
  float percent;

  soc = HAL_FuelGaugei2cRead16(0x04);  // Read SOC register of MAX17048G
  percent = (byte) (soc >> 8);  // High byte of SOC is percentage
  percent += ((float)((byte)soc))/256;  // Low byte is 1/256%

  return round(percent);
}*/

void HAL_FuelGaugeConfig(unsigned char percent) {
 // if ((percent >= 32)||(percent == 0)) { // Anything 32 or greater will set to 32%
  //  HAL_FuelGaugei2cWrite16(0x9700, 0x0C);
 // } else {MAX17048G_ADDRESS
    unsigned char percentBits = 32 - percent;
    unsigned char data[2];
    data[0]=0x97;
    data[1]=20|percentBits;
    i2c_write_fuel(MAX17048G_ADDRESS,0x0C,2,&data);

 // }
    data[0]=0x9B;
    data[1]=0xDC;
  // VALERT min and max voltage setting of 3.1V and 4.3V respectively (20mV per LSb)
    i2c_write_fuel(MAX17048G_ADDRESS, 0x14, 2, &data);

}

/*void HAL_FuelGaugeQuickStart() {
  HAL_FuelGaugei2cWrite16(0x4000, 0x06);  // Write a 0x4000 to the MODE register
}*/

/*byte HAL_FuelGaugeGetAlertReason() {
  return HAL_FuelGaugei2cRead16(0x1A >> 8);
}

void HAL_FuelGaugeClearAlert() {
  //TODO
}*/

