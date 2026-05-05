CXX = g++
CXXFLAGS = -fopenmp -O3

all: compile_cpp

compile_cpp:
	@echo "Compilando Questoes 2 e 3 com OpenMP..."
	$(CXX) $(CXXFLAGS) ex2.cpp -o q2_merge
	$(CXX) $(CXXFLAGS) ex3.cpp -o q3_quadtree

exec_all: compile_cpp
	@echo "\n--- Questao 1 (Dicionario AVL) ---"
	python3 ex1.py
	@echo "\n--- Questao 2 (K-Way Merge) ---"
	./q2_merge
	@echo "\n--- Questao 3 (Quadtree) ---"
	./q3_quadtree
	@echo "\n--- Questao 4 (Router LPM) ---"
	python3 ex4.py
	@echo "\n Limpando os executaveis C++ (q2_merge e q3_quadtree)..."
	rm -f q2_merge q3_quadtree

clean:
	rm -f q2_merge q3_quadtree