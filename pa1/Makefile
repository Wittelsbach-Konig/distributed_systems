COMP=clang
TARGET=pa1
CFLAGS=-std=c99 -Wall -pedantic
SOURCE=$(wildcard *.c)
BUILD=$(SOURCE:%.c=%.o)

$(TARGET): $(BUILD)
		$(COMP) $(CFLAGS) $(BUILD) -o $(TARGET)

%o.: %.c
		$(COMP) -c -o $@ $(CFLAGS) $^

clean:
		rm -f *.o
		rm -f $(TARGET)
		rm -f *.log