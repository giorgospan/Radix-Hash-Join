# specify directories
INCDIR  := ./include
SRCDIR  := ./src
TESTDIR := ./test
BUILDDIR:= ./build/release

# path to our custom test file
WORKPATH:= ./workloads/small/custom_input_file

# name of our executable
TARGET  := Driver

# specify source,object,header files & test source files
SOURCES := $(wildcard $(SRCDIR)/*.c)
OBJECTS := $(SOURCES:$(SRCDIR)/%.c=./%.o)
INCLUDES:= $(wildcard $(INCDIR)/*.h)
TESTS   := $(wildcard $(TESTDIR)/*.c)

CC      := gcc
CFLAGS  := -g -I $(INCDIR)  # -g enables debugging

#############################################################
# Build harness and Driver
all: $(BUILDDIR)/harness $(BUILDDIR)/$(TARGET)

# all: clean $(BUILDDIR)/$(TARGET)
# 	$(BUILDDIR)/$(TARGET) < $(WORKPATH)

valgrind: clean $(BUILDDIR)/$(TARGET)
	valgrind --leak-check=yes $(BUILDDIR)/$(TARGET) < $(WORKPATH)

# create executable [by linking object files]
$(BUILDDIR)/$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) 

# create object files
$(OBJECTS): ./%.o : $(SRCDIR)/%.c 
	$(CC) $(CFLAGS) -c $< -o $@

#############################################################

# create harness executable
$(BUILDDIR)/harness: ./harness.o
	g++ -g -o $@ $^

# create harness object file
./harness.o: $(SRCDIR)/harness.cpp
	g++ -g -c $< -o $@


#############################################################

# run testX , X=1,2,3...
# note that we also need the files of our actual program in order to run a test
test%: test%.o $(filter-out ./main.o, ./$(OBJECTS))
	$(CC) $(CFLAGS) -o $@ $^ -lcunit
	@./$@

test%.o: $(TESTDIR)/test%.c
	$(CC) $(CFLAGS) -c $< -o $@


#############################################################

# clean up
.PHONY: clean
clean:
	@rm -f harness.o $(OBJECTS) $(BUILDDIR)/* ./dumpFiles/*
	# @find ./test* -maxdepth 0 -type f -delete 
	@echo "Cleanup complete!"

# count 
.PHONY: count
count:
	@wc $(SOURCES) $(INCLUDES) $(TESTS)