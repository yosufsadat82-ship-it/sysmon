CC      = gcc
CFLAGS  = -Wall -Wextra -O2 -Iinclude
SRC     = src/sysmon.c src/sysinfo.c
TARGET  = sysmon

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

.PHONY: all clean install
