CC = clang
CPP = clang++
BINDIR = bin
SRCDIR = src
OBJDIR = obj
INCDIR = include
CFLAGS = -x c++ -I./$(INCDIR) 
LIBS = `pkg-config --static --libs glfw3`
CPP_SRCS = $(wildcard $(SRCDIR)/*.cpp) $(wildcard $(SRCDIR)/**/*.cpp)
C_SRCS = $(wildcard $(SRCDIR)/*.c) $(wildcard $(SRCDIR)/**/*.c)
OBJS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(CPP_SRCS)) $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(C_SRCS))
TARGET = main.out

.PHONY: clean

$(BINDIR)/$(TARGET): $(OBJS)
	$(CPP) -o $@ $(OBJS) $(CFLAGS) $(LIBS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	# Create parent directories for obj file
	mkdir -p "$(dir $@)"
	
	$(CPP) $(CFLAGS) -o $@ -c $<

$(OBJDIR)/%.o: **/%.c
	# Create parent directories for obj file
	mkdir -p "$(dir $@)"
	
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	# Remove binaries
	rm -f $(BINDIR)/*.out
	
	# Remove object files
	rm -rf obj/*.o obj/**/*.o
