#include "humidity_test.h"

uint32_t getHumidityAndTemperature() {
  set_cs_low(CS, &CS_PORT);

  uint32_t b1 = send_spi(0);
  uint32_t b2 = send_spi(0);
  uint32_t b3 = send_spi(0);
  uint32_t b4 = send_spi(0);
  uint32_t b = (b1 << 24) + (b2 << 16) + (b3 << 8) + b4;

  set_cs_high(CS, &CS_PORT);

  return b;
}

int main() {
  init_spi();
  init_cs(CS, &CS_DDR);
  set_cs_high(CS, &CS_PORT);
  init_uart();

  print("start");

  while (true) {
    uint32_t data = getHumidityAndTemperature();
    print("%d\n", data);
    _delay_ms(100);
  }
}
