INCLUDES = -Iinclude -I/usr/local/include
LIBRARIES = -L/usr/local/lib/cmake/glfw3 -lglfw3 -framework Cocoa -framework IOKit -framework CoreVideo

demo: glad.o
	clang++ -Wall $(LIBRARIES) $(INCLUDES) src/*.cpp glad.o -o demo
	rm glad.o

glad.o:
	clang -Iinclude -c src/glad.c 

.PHONY: clean
clean: 
	rm demo *.o


