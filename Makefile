bin = MedNES
src = $(wildcard NES/*.cpp NES/Mapper/*.cpp NES/Common/*.cpp)
obj = $(src:.cpp=.o)

CXXFLAGS = -g -Wall -Wextra -O2 -std=c++14 -pedantic $(shell pkg-config --cflags sdl2)
LDFLAGS = $(shell pkg-config --libs sdl2)

.PHONY: all clean

all: $(bin)

$(bin): $(obj)
	$(CXX) -o $@ $^ $(LDFLAGS)

clean:
	-rm $(bin) $(obj)
