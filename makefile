target = tdc_gen.so

SRC = tdc_gen.c
CC := gcc
OBJ := tdc_gen.o

.PHONY:clean

$(target):$(OBJ)	
	$(CC) -shared -o $(target) $(OBJ) 
$(OBJ):$(SRC)
	$(CC) -fPIC -Wall  -c $^

clean:
	rm *.o *.so
test:
	@echo $(SRC)
	@echo $(patsubst %.c,%.o,$(SRC))

