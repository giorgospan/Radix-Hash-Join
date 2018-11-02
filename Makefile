one: clean
	gcc -I include src/testMain.c src/bitWiseUtil.c src/prepareInput.c src/phaseOne.c src/structDefinitions.c
	# valgrind ./a.out
	./a.out 

# clean up
.PHONY: clean
clean:
	@rm -f ./a.out
	@rm -f input.bin
	@echo "Cleanup complete!"


# count lines of code
.PHONY: count
count:
	wc src/* include/*

