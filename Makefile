GCC=g++
GCCFLAGS=-Wpedantic -O

main:
	$(GCC) $(GCCFLAGS) -o TubesAndBalls TubesAndBalls.cpp

clean:
	rm -rf TubesAndBalls.exe