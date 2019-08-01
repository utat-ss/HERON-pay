# This makefile should go in a specific test folder within examples,
# harness_tests, or manual_tests, e.g. `manual_tests/commands_test/makefile`

PROG = main1
# .c files from `src` folder to compile (except for in `lib-common`),
# separated by spaces
SRC = $(addprefix ../../src/, devices.c env_sensors.c heaters.c motors.c optical_spi.c)
include ../makefile
