
one : 
	gcc -I include src/main.c src/setup.c src/arrayUtil.c src/structDefinitions.c
	# ./a.out
	# @./a.out < workloads/small/small.init
	@valgrind ./a.out
	# @valgrind ./a.out > inspect.txt


.PHONY: clean
clean:
	@rm a.out