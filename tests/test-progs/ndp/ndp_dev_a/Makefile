CC=g++
CCF=
LDF=-static
SRC_O0=main.cpp
SRC_O3=baseline.cpp
SRC=$(SRC_O0) $(SRC_O3)
OBJ=$(SRC:.cpp=.o)
EXE=bench

# Specify a different compiler
ifeq ($(arch), arm)
        CC=aarch64-none-linux-gnu-g++
else ifeq ($(arch), riscv)
        CC=riscv64-unknown-linux-gnu-g++
endif

# Optional arguments
ifeq ($(fs), 1)
	CCF+=-DFS
ifeq ($(driver), 1)
	CCF+=-DDRIVER
endif
endif

all: $(SRC) $(EXE)

$(EXE): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDF)

$(SRC_O0:.cpp=.o): $(SRC_O0)
	$(CC) $(CCF) -O0 -c $< -o $@ $(LDF)

$(SRC_O3:.cpp=.o): $(SRC_O3)
	$(CC) $(CCF) -O3 -c $< -o $@ $(LDF)

clean:
	rm -f $(OBJ) $(EXE)
