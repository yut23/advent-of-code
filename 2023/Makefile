ifeq ($(origin CXX), default)
	CXX = clang++
endif

# Set by the wrapper script to run the compiler under bear-intercept, then it
# will combine all the generated files into a compilation database after make
# finishes.
INTERCEPT_WRAPPER ?=

ifeq ($(USE_LIBCXX),TRUE)
	STDLIB_CXXFLAGS = -stdlib=libc++ -fexperimental-library
	STDLIB_LDFLAGS = $(STDLIB_CXXFLAGS)
endif

INCLUDES = -Isrc

LOCAL_CXXFLAGS = -Wall -Wextra -O3 -g -std=c++20 $(INCLUDES) $(STDLIB_CXXFLAGS)
DEBUG_CXXFLAGS = $(LOCAL_CXXFLAGS) -O0 -DDEBUG_MODE
LDFLAGS = -Wl,--as-needed $(STDLIB_LDFLAGS)
FAST_CXXFLAGS = -Wall -Wextra -O3 -std=c++20 -fno-omit-frame-pointer -DFAST_MODE $(INCLUDES) $(STDLIB_CXXFLAGS)
PROFILE_CXXFLAGS = $(FAST_CXXFLAGS) -g
FAST_LDFLAGS = -Wl,--as-needed -fno-omit-frame-pointer $(STDLIB_LDFLAGS)
BEAR_ARGS = --config ../tools/cpp/bear_config.json

ifneq ($(DISABLE_SANITIZERS),TRUE)
	LOCAL_CXXFLAGS += -fsanitize=undefined -fsanitize=address -fno-omit-frame-pointer
	LDFLAGS += -fsanitize=undefined -fsanitize=address -fno-omit-frame-pointer
else
	LOCAL_CXXFLAGS += -Werror
	FAST_CXXFLAGS += -Werror
endif

REL_BUILD_DIR = build/release
DEBUG_BUILD_DIR = build/debug
FAST_BUILD_DIR = build/fast
PROFILE_BUILD_DIR = build/profile

# List of all .cpp source files.
CPP = $(wildcard src/*.cpp)

# All .o files go to build dirs
REL_OBJ = $(CPP:src/%.cpp=$(REL_BUILD_DIR)/%.o)
DEBUG_OBJ = $(CPP:src/%.cpp=$(DEBUG_BUILD_DIR)/%.o)
FAST_OBJ = $(CPP:src/%.cpp=$(FAST_BUILD_DIR)/%.o)
PROFILE_OBJ = $(CPP:src/%.cpp=$(PROFILE_BUILD_DIR)/%.o)
# gcc/clang will create these .d files containing dependencies.
DEP = $(REL_OBJ:.o=.d) $(DEBUG_OBJ:.o=.d) $(FAST_OBJ:.o=.d) $(PROFILE_OBJ:.o=.d)

REL_EXECUTABLES := $(REL_OBJ:.o=)
DEBUG_EXECUTABLES := $(DEBUG_OBJ:.o=)
FAST_EXECUTABLES := $(FAST_OBJ:.o=)
PROFILE_EXECUTABLES := $(PROFILE_OBJ:.o=)

TESTS := $(filter $(DEBUG_BUILD_DIR)/test%,$(DEBUG_EXECUTABLES))
TESTS := $(TESTS:$(DEBUG_BUILD_DIR)/test%=test%)
TESTS := $(shell printf '%s\n' $(TESTS) | sort -n)

all: release debug fast
release: $(REL_EXECUTABLES)
debug: $(DEBUG_EXECUTABLES)
fast: $(FAST_EXECUTABLES)
profile: $(PROFILE_EXECUTABLES)

list:
	@printf 'normal:  %s\n' $(REL_EXECUTABLES)
	@printf 'debug:   %s\n' $(DEBUG_EXECUTABLES)
	@printf 'fast:    %s\n' $(FAST_EXECUTABLES)
	@printf 'profile: %s\n' $(PROFILE_EXECUTABLES)
	@printf 'tests:   %s\n' $(TESTS)

# create build directories if they don't exist
$(REL_BUILD_DIR) $(DEBUG_BUILD_DIR) $(FAST_BUILD_DIR) $(PROFILE_BUILD_DIR):
	mkdir -p $@

# Include all .d files
-include $(DEP)

# Build target for every single object file.
# The potential dependency on header files is covered
# by calling `-include $(DEP)`.
# The -MMD flags additionally creates a .d file with
# the same name as the .o file.
# Also include order-only dependencies on the build directory
$(REL_BUILD_DIR)/%.o: src/%.cpp Makefile | $(REL_BUILD_DIR)
	$(INTERCEPT_WRAPPER) $(CXX) $(LOCAL_CXXFLAGS) $(CXXFLAGS) -MMD -c $< -o $@
$(DEBUG_BUILD_DIR)/%.o: src/%.cpp Makefile | $(DEBUG_BUILD_DIR)
	$(INTERCEPT_WRAPPER) $(CXX) $(DEBUG_CXXFLAGS) $(CXXFLAGS) -MMD -c $< -o $@
$(FAST_BUILD_DIR)/%.o: src/%.cpp | $(FAST_BUILD_DIR)
	$(CXX) $(FAST_CXXFLAGS) $(CXXFLAGS) -MMD -c $< -o $@
$(PROFILE_BUILD_DIR)/%.o: src/%.cpp | $(PROFILE_BUILD_DIR)
	g++ $(PROFILE_CXXFLAGS) $(CXXFLAGS) -MMD -c $< -o $@

# Link the object files into executables
$(REL_EXECUTABLES) $(DEBUG_EXECUTABLES): %: %.o
	$(CXX) $^ -o $@ $(LDFLAGS)
$(FAST_EXECUTABLES): %: %.o
	$(CXX) $^ -o $@ $(FAST_LDFLAGS)
$(PROFILE_EXECUTABLES): %: %.o
	g++ $^ -o $@ $(FAST_LDFLAGS)

# run unit tests
test: $(TESTS)

$(TESTS): %: $(DEBUG_BUILD_DIR)/% FORCE
	@ASAN_OPTIONS=$${ASAN_OPTIONS:+$${ASAN_OPTIONS}:}handle_abort=1 $<

clean:
	rm -f $(REL_EXECUTABLES) $(DEBUG_EXECUTABLES) $(FAST_EXECUTABLES) $(PROFILE_EXECUTABLES) $(REL_OBJ) $(DEBUG_OBJ) $(FAST_OBJ) $(PROFILE_OBJ) $(DEP)

FORCE:
.PHONY: all release debug fast profile test list clean FORCE
