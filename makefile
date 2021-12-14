CC = gcc
CPP = g++
BINDIR = bin
SRCDIR = src
OBJDIR = obj
INCDIRS = include include/imgui
INCLUDE = $(addprefix -I./,$(INCDIRS))
CFLAGS = -x c++ -std=c++17 $(INCLUDE)
DFLAGS = -g -x c++ $(INCLUDE)
RFLAGS = -O3 -x c++ $(INCLUDE)
LIBS = `pkg-config --static --libs glfw3`
CPP_SRCS = $(wildcard $(SRCDIR)/*.cpp) $(wildcard $(SRCDIR)/**/*.cpp)
C_SRCS = $(wildcard $(SRCDIR)/*.c) $(wildcard $(SRCDIR)/**/*.c)
OBJS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(CPP_SRCS)) $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(C_SRCS))
TARGET = main
DEBUGTARGET = debug
RELEASETARGET = release

.PHONY: clean

all: $(BINDIR) $(BINDIR)/$(TARGET) $(SRCDIR) $(OBJDIR) $(INCDIRS)

$(BINDIR)/$(TARGET): $(OBJS)
	$(CPP) -o $@ $(OBJS) $(CFLAGS) $(LIBS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@# Create parent directories for obj files
	mkdir -p "$(dir $@)"
	
	$(CPP) $(CFLAGS) -o $@ -c $<

$(OBJDIR)/%.o: **/%.c
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

debug: $(BINDIR)/$(DEBUGTARGET)
$(BINDIR)/$(DEBUGTARGET): clean $(BINDIR) $(OBJDIR) $(OBJS)
	$(CPP) -o $@ $(OBJS) $(DFLAGS) $(LIBS)

release: clean $(BINDIR)/$(RELEASETARGET)
$(BINDIR)/$(RELEASETARGET): $(BINDIR) $(OBJDIR) $(OBJS)
	$(CPP) -o $@ $(OBJS) $(RFLAGS) $(LIBS)

clean:
	@# Remove binaries
	rm -f $(BINDIR)/*
	
	@# Remove object files
	rm -rf obj/*.o obj/**/*.o
