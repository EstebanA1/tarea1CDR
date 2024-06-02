# Nombre del ejecutable
TARGET = servidor

# Compilador
CC = g++

# Opciones de compilación
CFLAGS = -Wall -std=c++11

# Archivos fuente
SRCS = servidor.cpp

# Archivos objeto
OBJS = $(SRCS:.cpp=.o)

# Regla por defecto
all: $(TARGET)

# Regla para compilar el ejecutable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Regla para compilar los archivos objeto
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Regla para limpiar los archivos compilados
clean:
	rm -f $(TARGET) $(OBJS)
