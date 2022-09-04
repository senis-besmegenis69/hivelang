
################################################################################
#### Variables and settings
################################################################################

# Executable name
EXEC = hivec

# Build, bin, assets, and install directories (bin and build root directories are kept for clean)
BUILD_DIR_ROOT = ./build
BIN_DIR_ROOT = ./bin
INSTALL_DIR := ~/Desktop/$(EXEC)

# Sources (searches recursively inside the source directory)
SRC_DIR = ./source
SRCS := $(SRC_DIR)/main.c

# Includes
INCLUDE_DIR = ./include
INCLUDES := -I$(INCLUDE_DIR)

# C preprocessor settings
CPPFLAGS = $(INCLUDES) -MMD -MP

# C++ compiler settings
CC = gcc
CFLAGS = -std=c11
WARNINGS = -Wall -Wpedantic -Wextra

# Linker flags
LDFLAGS =

# Libraries to link
LDLIBS =

# Target OS detection
ifeq ($(OS),Windows_NT) # OS is a preexisting environment variable on Windows
	OS = windows
else
	UNAME := $(shell uname -s)
	ifeq ($(UNAME),Darwin)
		OS = macos
	else ifeq ($(UNAME),Linux)
		OS = linux
	else
		$(error OS not supported by this Makefile)
	endif
endif

# OS-specific settings
ifeq ($(OS),windows)
	# Link libgcc and libstdc++ statically on Windows
	LDFLAGS += -static-libgcc -static-libstdc++

	# Windows 32- and 64-bit common settings
	INCLUDES +=
	LDFLAGS +=
	LDLIBS +=

	ifeq ($(win32),1)
		# Windows 32-bit settings
		INCLUDES +=
		LDFLAGS +=
		LDLIBS +=
	else
		# Windows 64-bit settings
		INCLUDES +=
		LDFLAGS +=
		LDLIBS +=
	endif
else ifeq ($(OS),macos)
	# Mac-specific settings
	INCLUDES +=
	LDFLAGS +=
	LDLIBS +=
else ifeq ($(OS),linux)
	# Linux-specific settings
	INCLUDES +=
	LDFLAGS +=
	LDLIBS +=
endif

################################################################################
#### Final setup
################################################################################

# Windows-specific default settings
ifeq ($(OS),windows)
	# Add .exe extension to executable
	EXEC := $(EXEC).exe

	ifeq ($(win32),1)
		# Compile for 32-bit
		CFLAGS += -m32
	else
		# Compile for 64-bit
		CFLAGS += -m64
	endif
endif

# OS-specific build, bin, and assets directories
BUILD_DIR := $(BUILD_DIR_ROOT)/$(OS)
BIN_DIR := $(BIN_DIR_ROOT)/$(OS)
ifeq ($(OS),windows)
	# Windows 32-bit
	ifeq ($(win32),1)
		BUILD_DIR := $(BUILD_DIR)32
		BIN_DIR := $(BIN_DIR)32
	# Windows 64-bit
	else
		BUILD_DIR := $(BUILD_DIR)64
		BIN_DIR := $(BIN_DIR)64
	endif
endif

# Debug (default) and release modes settings
ifeq ($(release),1)
	BUILD_DIR := $(BUILD_DIR)/release
	BIN_DIR := $(BIN_DIR)/release
	CFLAGS += -O3
	CPPFLAGS += -DNDEBUG
else
	BUILD_DIR := $(BUILD_DIR)/debug
	BIN_DIR := $(BIN_DIR)/debug
	CFLAGS += -O0 -g
endif

# Objects and dependencies
OBJS := $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

################################################################################
#### Targets
################################################################################

.PHONY: all
all: $(BIN_DIR)/$(EXEC)

# Build executable
$(BIN_DIR)/$(EXEC): $(OBJS)
	@echo "Building executable: $@"
	@mkdir -p $(@D)
	@$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

# Compile C source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "Compiling: $<"
	@mkdir -p $(@D)
	@$(CC) $(CPPFLAGS) $(CFLAGS) $(WARNINGS) -c $< -o $@

# Include automatically generated dependencies
-include $(DEPS)

# Install packaged program
.PHONY: install
install: all
	@echo "Packaging program to $(INSTALL_DIR)"
	@mkdir -p $(INSTALL_DIR); cp -r $(BIN_DIR)/. $(INSTALL_DIR)

# Build and run
.PHONY: run
run: all
	@echo "Starting program: $(BIN_DIR)/$(EXEC)"
	@cd ./$(BIN_DIR); ./$(EXEC)

# Clean build and bin directories for all platforms
.PHONY: clean
clean:
	@echo "Cleaning $(BUILD_DIR_ROOT) and $(BIN_DIR_ROOT) directories"
	@$(RM) -r $(BUILD_DIR_ROOT)
	@$(RM) -r $(BIN_DIR_ROOT)

# Run clang-format on source code
.PHONY: format
format:
	@echo "Running clang-format"
	@clang-format -i $$(find $(SRC_DIR) $(INCLUDE_DIR) -name '*.c' -o -name '*.h' -o -name '*.inl')

# Generate documentation with Doxygen
.PHONY: doc
doc:
	@echo "Generating documentation"
	@doxygen doc/Doxyfile

# Print help information
.PHONY: help
help:
	@printf "\
	Usage: make target... [options]...\n\
	\n\
	Targets:\n\
		all               Build executable (debug mode by default) (default target)\n\
		install           Install packaged program to desktop (debug mode by default)\n\
		run               Build and run executable (debug mode by default)\n\
		clean             Clean build and bin directories (all platforms)\n\
		format            Run clang-format on source code\n\
		doc               Generate documentation with Doxygen\n\
		help              Print this information\n\
		printvars         Print Makefile variables for debugging\n\
	\n\
	Options:\n\
		release=1         Run target using release configuration rather than debug\n\
		win32=1           Build for 32-bit Windows (valid when built on Windows only)\n\
	\n\
	Note: the above options affect all, install, run, and printvars targets\n"

# Print Makefile variables
.PHONY: printvars
printvars:
	@printf "\
	OS: \"$(OS)\"\n\
	EXEC: \"$(EXEC)\"\n\
	BUILD_DIR: \"$(BUILD_DIR)\"\n\
	BIN_DIR: \"$(BIN_DIR)\"\n\
	INSTALL_DIR: \"$(INSTALL_DIR)\"\n\
	SRC_DIR: \"$(SRC_DIR)\"\n\
	SRCS: \"$(SRCS)\"\n\
	INCLUDE_DIR: \"$(INCLUDE_DIR)\"\n\
	INCLUDES: \"$(INCLUDES)\"\n\
	CC: \"$(CC)\"\n\
	CPPFLAGS: \"$(CPPFLAGS)\"\n\
	CFLAGS: \"$(CFLAGS)\"\n\
	WARNINGS: \"$(WARNINGS)\"\n\
	LDFLAGS: \"$(LDFLAGS)\"\n\
	LDLIBS: \"$(LDLIBS)\"\n"
