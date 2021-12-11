CC = gcc
CPP = g++
BINDIR = bin
SRCDIR = src
OBJDIR = obj
INCDIR = include
CFLAGS = -x c++ -I./$(INCDIR)
DFLAGS = -g -x c++ -I./$(INCDIR)
RFLAGS = -O3 -x c++ -I./$(INCDIR)
LIBS = `pkg-config --static --libs glfw3`
CPP_SRCS = $(wildcard $(SRCDIR)/*.cpp) $(wildcard $(SRCDIR)/**/*.cpp)
C_SRCS = $(wildcard $(SRCDIR)/*.c) $(wildcard $(SRCDIR)/**/*.c)
OBJS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(CPP_SRCS)) $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(C_SRCS))
TARGET = main
DEBUGTARGET = debug
RELEASETARGET = release

.PHONY: clean

all: $(BINDIR) $(BINDIR)/$(TARGET) $(SRCDIR) $(OBJDIR) $(INCDIR)

$(BINDIR)/$(TARGET): $(OBJS)
	$(CPP) -o $@ $(OBJS) $(CFLAGS) $(LIBS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CPP) $(CFLAGS) -o $@ -c $<

$(OBJDIR)/%.o: **/%.c
	$(CC) $(CFLAGS) -o $@ -c $<

$(BINDIR):
	mkdir $@

$(OBJDIR):
	mkdir $@

$(INCDIR):
	mkdir $@

$(SRCDIR):
	mkdir $@

debug: clean $(BINDIR) $(OBJDIR) $(OBJS)
	$(CPP) -o $(BINDIR)/$(DEBUGTARGET) $(OBJS) $(DFLAGS) $(LIBS)

release: clean $(BINDIR) $(OBJDIR) $(OBJS)
	$(CPP) -o $(BINDIR)/$(RELEASETARGET) $(OBJS) $(RFLAGS) $(LIBS)

clean:
	@# Remove binaries
	rm -f $(BINDIR)/*
	
	@# Remove object files
	rm -rf obj/*.o obj/**/*.o
