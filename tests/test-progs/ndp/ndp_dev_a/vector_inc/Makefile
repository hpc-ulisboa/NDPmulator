CC=gcc
CCF=-O0
LDF=-static
SRC=main.c
OBJ=$(SRC:.c=.o)
EXE=vector_inc

all: $(SRC) $(EXE)

$(EXE): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDF)

.c.o:
	$(CC) $(CCF) -c $< -o $@ $(LDF)

clean:
	rm -f $(OBJ) $(EXE)
