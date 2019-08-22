INCLUDES = -Iinclude -I/usr/local/include
LIBRARIES = -L/usr/local/lib/cmake/glfw3 -lglfw3 -framework Cocoa -framework IOKit -framework CoreVideo

demo: glad.o simple.o
	clang++ -Wall $(LIBRARIES) glad.o simple.o -o demo

simple.o:
	clang++ -Wall $(INCLUDES) -c src/simple.cpp 

glad.o:
	clang -Iinclude -c src/glad.c 

.PHONY: clean
clean: 
	rm demo *.o


