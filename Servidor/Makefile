# Compilador
CC = g++

# Opciones de compilación
CFLAGS = -Wall

# Nombre del archivo ejecutable
TARGET = servidor

# Archivo fuente
SOURCE = servidor.cpp

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCE)

clean:
	rm -f $(TARGET)

.PHONY: all clean
