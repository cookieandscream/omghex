TARGET=trains
OBJS=src/main.o src/utils.o src/hexmap.o

CFLAGS=$(shell sdl2-config --cflags) -Wall -g
CXXFLAGS=$(shell sdl2-config --cflags) -Wall -g
LDFLAGS=$(shell sdl2-config --cflags --static-libs)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $(TARGET) $(LDFLAGS) $(CXXFLAGS) $(OBJS)

clean:
	-$(RM) $(TARGET) *.o src/*.o
