# build an executable named solution from sudoku_solver.cpp
all: sudoku_solver.cpp
	g++ sudoku_solver.cpp -o solution
	./solution

clean:
	$(RM) solution