run: advanced
	./advanced
advanced: advanced.cpp
	$(CXX) advanced.cpp -o advanced -std=c++11
test: input.txt output.txt advanced
	./advanced < input.txt | diff -aq - output.txt
input.txt: input-gen
	./input-gen > input.txt
output.txt: input.txt sample
	./sample < input.txt > output.txt
input-gen: input-gen.c
	$(CC) input-gen.c -o input-gen
clean:
	$(RM) advanced input-gen
