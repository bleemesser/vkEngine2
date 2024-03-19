CFLAGS = -std=c++17 -O2 -Isrc -Ilibs
# -Ilibs
# CFLAGS = -std=c++20 -O2 -Iinclude $(foreach dir,$(wildcard libs/*),$(if $(wildcard $(dir)/include),-I$(dir)/include,-I$(dir)))
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi
# we have main.cpp, and various .cpp and .hpp files in include/

main.o: main.cpp src/*.cpp src/*.hpp
	g++ $(CFLAGS) -o main.o *.cpp src/*.cpp $(LDFLAGS)

.PHONY: clean test shaders docs all

test: main.o
	#doxygen Doxyfile
	# ./shaders/compile.sh
	./main.o

shaders:
	./shaders/compile.sh

clean:
	rm -f main.o

docs:
	doxygen Doxyfile

all:
	./shaders/compile.sh
	#./main.o