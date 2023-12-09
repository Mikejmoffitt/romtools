CC := cc
CFLAGS := -std=c99 -O2 -Isrc -g -Wall

LDFLAGS :=
SOURCES := $(wildcard src/*.c)
OBJECTS := $(SOURCES:.c=.o)
TARGET := m92-tracemap
INSTALL_PREFIX := /usr/local/bin

.PHONY: all clean

all: $(TARGET)

clean:
	$(RM) $(OBJECTS) $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $(LDFLAGS)

.c.o:
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

install: $(TARGET)
	cp $(TARGET) $(INSTALL_PREFIX)/$(TARGET)
