CC = clang
CPP = clang++
BINDIR = bin
SRCDIR = src
OBJDIR = obj
INCDIR = include
CFLAGS = -x c++ -I./$(INCDIR) `pkg-config --static --libs glfw3`
CPP_SRCS = $(wildcard $(SRCDIR)/*.cpp) $(wildcard $(SRCDIR)/**/*.cpp)
C_SRCS = $(wildcard $(SRCDIR)/*.c) $(wildcard $(SRCDIR)/**/*.c)
OBJS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(CPP_SRCS)) $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(C_SRCS))
TARGET = main.out

$(BINDIR)/$(TARGET): $(OBJS)
	$(CPP) -o $@ $(OBJS) $(CFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CPP) $(CFLAGS) -o $@ -c $<

$(OBJDIR)/%.o: **/%.c
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -f $(BINDIR)/$(TARGET)
	rm -rf obj/*.o
	rm -rf obj/**/*.o
