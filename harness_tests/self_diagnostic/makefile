# This makefile should go in a specific test folder within examples,
# harness_tests, or manual_tests, e.g. `manual_tests/commands_test/makefile`

PROG = main1
# .c files from `src` folder to compile (except for in `lib-common`),
# separated by spaces
SRC = $(addprefix ../../src/, can_commands.c can_interface.c devices.c env_sensors.c general.c heaters.c motors.c optical_spi.c)
include ../makefile
