# specify directories
INCDIR := ./include
SRCDIR := ./src
TESTDIR:= ./test

# name of executable
TARGET := myprog

# specify source,object,header files [+ test source files]
SOURCES := $(wildcard $(SRCDIR)/*.c)
OBJECTS := $(SOURCES:$(SRCDIR)/%.c=./%.o)
INCLUDES := $(wildcard $(INCDIR)/*.h)
TESTS := $(wildcard $(TESTDIR)/*.c)

CC := gcc
CFLAGS := -g -I $(INCDIR)  # -g enables debugging

#############################################################

# run the program
all: clean ./$(TARGET)
	# valgrind ./$(TARGET)
	./$(TARGET)

# create executable [by linking object files]
./$(TARGET): ./$(OBJECTS)
	$(CC) $(CFLAGS) -o $@ ./$(OBJECTS) 

# create object files
./$(OBJECTS): ./%.o : $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

#############################################################

# run testX , X=1,2,3...
# note that we also need the files of our actual program in order to run a test
test%: test%.o $(filter-out ./main.o, ./$(OBJECTS))
	@$(CC) $(CFLAGS) -o $@ $^ -lcunit
	@./$@

test%.o: $(TESTDIR)/test%.c
	$(CC) $(CFLAGS) -c $< -o $@


#############################################################

# clean up
.PHONY: clean
clean:
	@rm -f $(OBJECTS) ./$(TARGET) 
	@rm -f input.bin results.log
	@find ./test* -maxdepth 0 -type f -delete
	@echo "Cleanup complete!"

# count 
.PHONY: count
count:
	wc $(SOURCES) $(INCLUDES) $(TESTS)