# specify directories
INCDIR  := ./include
SRCDIR  := ./src
TESTDIR := ./test
BUILDDIR:= ./build/release

# path to our custom test file
WORKPATH:= ./workloads/small/custom_input_file

# name of our executable
TARGET  := Driver

# specify source,object,header files & test source/object files
SOURCES := $(wildcard $(SRCDIR)/*.c)
OBJECTS := $(SOURCES:$(SRCDIR)/%.c=./%.o)
INCLUDES:= $(wildcard $(INCDIR)/*.h)

TESTSRCS:= $(wildcard $(TESTDIR)/*.c)
TESTOBJS:= $(TESTSRCS:$(TESTDIR)/%.c=./%.o)

CC      := gcc
CFLAGS  := -O3 -g -I $(INCDIR)  # -O3 for even more compiler optimization

#############################################################
# Build harness Driver 
all: $(BUILDDIR)/harness $(BUILDDIR)/$(TARGET)

# all: clean $(BUILDDIR)/$(TARGET)
# 	$(BUILDDIR)/$(TARGET) < $(WORKPATH)

# valgrind: clean $(BUILDDIR)/$(TARGET)
# 	valgrind --leak-check=yes $(BUILDDIR)/$(TARGET) < $(WORKPATH)

# create executable [by linking object files]
$(BUILDDIR)/$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) 

# create object files
$(OBJECTS): ./%.o : $(SRCDIR)/%.c 
	$(CC) $(CFLAGS) -c $< -o $@

#############################################################

# create harness executable
$(BUILDDIR)/harness: ./harness.o
	g++ -O3 -o $@ $^

# create harness object file
./harness.o: $(SRCDIR)/harness.cpp
	g++ -O3 -c $< -o $@

#############################################################

# note that we discard the main function of our program.
# that is because we want to use the main function that we 
# have in TESTDIR
unittest: $(TESTOBJS) $(filter-out ./main.o, ./$(OBJECTS))
	$(CC) $(CFLAGS) -o $@ $^ -lcunit

$(TESTOBJS): ./%.o : $(TESTDIR)/%.c 
	$(CC) $(CFLAGS) -c $< -o $@


#############################################################

# clean up
.PHONY: clean
clean:
	@rm -f harness.o $(OBJECTS) $(TESTOBJS) $(BUILDDIR)/* ./dumpFiles/* unittest ./workloads/small/call*
	@echo "Cleanup complete!"

# count 
.PHONY: count
count:
	@wc $(SOURCES) $(INCLUDES) $(TESTS)