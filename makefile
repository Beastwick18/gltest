# This is the worlds greatest makefile

CC = gcc
CXX = g++
BINDIR = bin
SRCDIR = src
OBJDIR = obj
INCDIRS = include include/imgui
INCLUDE = $(addprefix -I./,$(INCDIRS))
CXXFLAGS = -x c++ -std=c++17 -Wall $(INCLUDE)
CFLAGS = -x c++ -std=c++17 -Wall $(INCLUDE)
LIBS = `pkg-config --static --libs glfw3`
CXX_SRCS = $(wildcard $(SRCDIR)/*.cpp) $(wildcard $(SRCDIR)/**/*.cpp)
C_SRCS = $(wildcard $(SRCDIR)/*.c) $(wildcard $(SRCDIR)/**/*.c)
OBJS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(CXX_SRCS)) $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(C_SRCS))
DEPS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.d,$(CXX_SRCS)) $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.d,$(C_SRCS))
TARGET = main
DEBUGTARGET = debug
RELEASETARGET = release

.PHONY: all clean debug release

all: $(BINDIR) $(BINDIR)/$(TARGET) $(SRCDIR) $(OBJDIR) $(INCDIRS)

$(BINDIR)/$(TARGET): $(OBJS)
	$(CXX) -o $@ $(OBJS) $(CXXFLAGS) $(LIBS)

-include $(DEPS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@# Create parent directories for obj files
	mkdir -p "$(dir $@)"
	
	$(CXX) $(CXXFLAGS) -MMD -MP -o $@ -c $<

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@# Create parent directories for obj files
	mkdir -p "$(dir $@)"
	
	$(CC) $(CFLAGS) -o $@ -c $<

$(BINDIR):
	mkdir $@

$(OBJDIR):
	mkdir $@

$(INCDIR):
	mkdir $@

$(SRCDIR):
	mkdir $@

debug: CXXFLAGS+=-g
debug: CFLAGS+=-g
debug: clean $(BINDIR)/$(DEBUGTARGET)
$(BINDIR)/$(DEBUGTARGET): $(BINDIR) $(OBJDIR) $(OBJS)
	$(CXX) -o $@ $(OBJS) $(CXXFLAGS) $(LIBS)

release: CXXFLAGS+=-O3
release: CFLAGS+=-O3
release: clean $(BINDIR)/$(RELEASETARGET)
$(BINDIR)/$(RELEASETARGET): $(BINDIR) $(OBJDIR) $(OBJS)
	$(CXX) -o $@ $(OBJS) $(CXXFLAGS) $(LIBS)


clean:
	@# Remove binaries
	rm -f $(BINDIR)/*
	
	@# Remove object files
	rm -rf obj/*.o obj/**/*.o
	@# Remove dependency files
	rm -rf obj/*.d obj/**/*.d
