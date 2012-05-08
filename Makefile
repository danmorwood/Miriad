all: domain.o plan.o matrix.o matrixTest

domain.o: domain.h domain.cpp
	g++ $(FLAGS) -c -o domain.o domain.cpp

plan.o: domain.h plan.h plan.cpp
	g++ $(FLAGS) -c -o plan.o plan.cpp

matrix.o: matrix.h matrix.cpp
	g++ $(FLAGS) -c -o matrix.o matrix.cpp

matrixTest: tests/matrixTest.cpp matrix.o
	g++ $(FLAGS) -o matrixTest tests/matrixTest.cpp matrix.o

clean:
	rm *.o
