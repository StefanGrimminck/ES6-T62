PROJECT_NAME=i2c_develop

CFLAGS=

MAIN_SOURCES=main.c i2c_develop.c

GCC=arm-linux-gcc

all: $(PROJECT_NAME) copy

$(PROJECT_NAME): $(MAIN_SOURCES) Makefile *.h
	@$(GCC) $(CFLAGS) $(MAIN_SOURCES) -o $@

copy:
	scp $(PROJECT_NAME) root@192.168.0.100:/bin/hoi/

clean:
	@rm -rf $(PROJECT_NAME)