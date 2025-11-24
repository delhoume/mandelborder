# Makefile for Mandelbrot SDL2 renderer

CXX = g++
CXXFLAGS = -std=c++11 -Wall -O3
LDFLAGS = -lSDL2

# macOS specific flags
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
    CXXFLAGS += -I/opt/homebrew/include -I/usr/local/include
    LDFLAGS += -L/opt/homebrew/lib -L/usr/local/lib
endif

# Linux specific flags
ifeq ($(UNAME_S),Linux)
    LDFLAGS += -lm
endif

TARGET = mandelbrot_sdl2
SOURCE = mandelbrot_sdl2.cpp

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCE) $(LDFLAGS)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
