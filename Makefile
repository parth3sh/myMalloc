CC = gcc
CFLAGS = -Wall -Wvla -g -fsanitize=address
LDFLAGS = 
OBJFILES = mymalloc.o main.o
TARGET = malloctest

all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES) $(LDFLAGS)

clean:
	rm -f $(OBJFILES) $(TARGET) *~
