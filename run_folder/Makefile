GCC = g++

LIB  = -L../fade2d/fade_lib -lgomp -lblas -llapacke -lfade2d -lm

GCC_LIB = -lfftw3 

COPT = -DGSL -FFTW

GCC_COPT = -pthread -Wl,-rpath,../fade2d/fade_lib 

VPATH = ../advection:../

SOURCES = example_meshes.cpp transport.cpp main_2d_cpu.cpp

OBJECTS = $(SOURCES:.cpp=.o)

all: 2D_CPU_CPU.exe

2D_CPU_CPU.exe: $(OBJECTS)
	$(GCC) $^ -o $@ $(GCC_LIB) $(LIB) $(COPT) $(GCC_COPT)

%.o: %.cpp
	$(GCC) -c $< -o $@ $(COPT)

clean:
	rm -rf *.o
	rm -rf *.exe
