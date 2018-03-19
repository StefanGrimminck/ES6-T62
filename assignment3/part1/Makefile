PROJECT_NAME=i2c_develop

CFLAGS=

MAIN_SOURCES=main.c

GCC=gcc

all: $(PROJECT_NAME)

$(PROJECT_NAME): $(MAIN_SOURCES) Makefile *.h
	@$(GCC) $(CFLAGS) $(MAIN_SOURCES) -o $@

clean:
	@rm -rf $(PROJECT_NAME)