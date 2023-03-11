BINARY=Detalhamento  			# Final file name
CODEDIRS=. src or ./src/sec		# Core files directorie
INCDIRS=. ./include/ 			# Dependencies directorie

CC=gcc
OPT=-O0

# generate files that encode make rules for the .h dependencies
DEPFLAGS=-MP -MD

# automatically add the -I onto each include directory
CFLAGS = -Wall -Wextra -g $(foreach D,$(INCDIRS),-I$(D)) $(OPT) $(DEPFLAGS)

# for-style iteration (foreach) and regular expression completions (wildcard)
CFILES = $(foreach D,$(CODEDIRS),$(wildcard $(D)/*.c))

# regular expression replacement
OBJECTS  = $(patsubst %.c,%.o,$(CFILES))
DEPFILES = $(patsubst %.c,%.d,$(CFILES))

#Compile all the files with dependencies then clean .o and .d files
all:$(BINARY) 
	@rm -rf $(OBJECTS) $(DEPFILES)

$(BINARY): $(OBJECTS)
	@$(CC) -o $@ $^

# only want the .c file dependency here, thus $< instead of $^.
%.o:%.c
	@$(CC) $(CFLAGS) -c -o $@ $<

#Clenup all .o .d and .exe files
clean:
	@rm -rf $(BINARY) $(OBJECTS) $(DEPFILES)

#Compile and run all the files with dependencies then clean .o and .d files
run:$(BINARY)
	@rm -rf $(OBJECTS) $(DEPFILES)
	@.\$(BINARY)

# include the dependencies
-include $(DEPFILES)