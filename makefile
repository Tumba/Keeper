GPP = g++
GCC = gcc
OUTFILE = "keeper.so"

COMPILE_FLAGS = -m32 -fPIC -c -O3 -w -D LINUX -D PROJECT_NAME=\"test\" -I ./SDK/amx/


all:
	$(GPP) $(COMPILE_FLAGS) ./SDK/*.cpp
	$(GPP) $(COMPILE_FLAGS) *.cpp
	$(GPP) -m32 -O3 -fshort-wchar -shared -o $(OUTFILE) *.o -lpthread
	rm -f *.o
