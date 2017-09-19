#include "humidity_test.h"

uint32_t getHumidityAndTemperature() {
  int i = 0;
  uint32_t data = 0x00;

  set_gpio_a(0, 0);
  for (i; i<4; i++){
    data << 8;
    data |= send_spi(0);
  }
  set_gpio_a(0, 0b00100001);

  return data;
}

int main() {
  init_spi();
  port_expander_init();
  set_gpio_a(0, 0b00100001);
  init_uart();

  while (1) {
    uint32_t data = getHumidityAndTemperature();
    print("%u\n", data);
    _delay_ms(1000);
  }
}
