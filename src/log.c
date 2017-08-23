#include "log.h"

// UART must be initialized before calling print
int print(char *str, ...) {
    va_list ptr;

    va_start(ptr, str);

    char buf[LOG_BUFFER_SIZE];
    int ret = vsprintf(buf, str, ptr);
    va_end(ptr);

    send_uart(buf);

    return ret;
}
