one: 
	gcc -I include src/testMain.c src/bitWiseUtil.c src/prepareInput.c src/phaseOne.c src/structDefinitions.c
	valgrind ./a.out
	# ./a.out

.PHONY: clean
clean:
	rm a.out
	rm input.bin