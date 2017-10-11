#define COMM_BYTE_READ 01000000
#define COMM_BYTE_WRITE 00000000
// bit[5:3] is register address
// bit[2] is CREAD. Set to 1 to enable continuous read

#define STATUS_ADDR 0x00
#define MODE_ADDR   0x01
#define CONFIG_ADDR 0x02
#define DATA_ADDR   0x03
#define ID_ADDR     0x04
#define GPOCON      0x05
#define OFFSET      0x06
#define FULL_SCALE  0x07

#define CONFIG_DEFAULT 0x040008
