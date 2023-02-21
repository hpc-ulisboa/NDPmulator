CC=gcc
CCF=-c -O0
LDF=-static
SRC=main.c
OBJ=$(SRC:.c=.o)
EXE=test_ndp

all: $(SRC) $(EXE)

$(EXE): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDF)

.c.o:
	$(CC) $(CCF) $< -o $@ $(LDF)

clean:
	rm -f $(OBJ) $(EXE)
