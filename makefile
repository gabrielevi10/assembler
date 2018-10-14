CPP      = g++
CC       = gcc
SRCS	 = $(shell find src -name '*.cpp')
OBJ      = $(addprefix obj/,$(notdir $(SRCS:%.cpp=%.o))) 
LIBS     = 
INCS     = 
CXXINCS  = -I"include/"
BIN      = montador
CXXFLAGS = $(CXXINCS) -std=c++11 -g3 -ggdb3
CFLAGS   = $(INCS) -std=c11 -ggdb3
RM       = rm -f

.PHONY: all all-before all-after clean clean-custom

all: $(BIN) all-after

clean: clean-custom
	${RM} $(OBJ) $(BIN) *.obj *.pre

$(BIN): $(OBJ)
	$(CPP) $(OBJ) -o $(BIN) $(LIBS)

obj/%.o: src/%.cpp
	$(CPP) -c $< -o $@ $(CXXFLAGS)
