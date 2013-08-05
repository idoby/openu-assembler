# Some globals
export CC 			= gcc
export FLAGS		?= -ansi -pedantic -Wall -Wextra -g -I$(abspath .)
export BUILD_SYSTEM_PATH ?= $(abspath makefile)
export WITH_TESTS	?= y

include defs.make

# Turn all defined source files into the corresponding object file names
OBJECTS = $(SOURCES:.c=.o)

# If an executable target is defined, link all of the dependencies and output it
ifneq ($(TARGET),)
$(TARGET): $(SOURCE_DIRS) $(OBJECTS) $(EXTRA_DEPS)
	$(CC) $(OBJECTS) -o $(TARGET)

.DEFAULT_GOAL := $(TARGET)
endif


# If we have subdirs...
ifneq ($(SOURCE_DIRS),)
# Each dir has its own object file
.PHONY: $(SOURCE_DIRS)
$(SOURCE_DIRS):
	make -j4 -C $@ -f $(BUILD_SYSTEM_PATH) $(notdir $@).o

DIR_OBJECTS =	$(addsuffix .o,				\
				$(join $(SOURCE_DIRS),		\
				$(addprefix /,				\
				$(notdir $(SOURCE_DIRS)))))
OBJECTS += $(DIR_OBJECTS)

# And a clean target
DIRS_CLEAN 	= $(addsuffix _clean,$(SOURCE_DIRS))

.PHONY: $(DIRS_CLEAN)
$(DIRS_CLEAN):
	make -j4 -C $(patsubst %_clean,%,$@) -f $(BUILD_SYSTEM_PATH) clean
endif

# Actual cleaning command
.PHONY: clean
clean: $(DIRS_CLEAN)
	rm -f $(OBJECTS) $(TARGET) $(MODULE_OUTPUT)
	
# Turn a source file into an object file
.c.o:
	$(CC) $(FLAGS) -c $< -o $@

# Output an object file representing a whole subdirectory
ifneq ($(MODULE_OUTPUT),)
$(MODULE_OUTPUT): $(OBJECTS) $(EXTRA_DEPS)
	ld -r $(OBJECTS) -o $@
endif