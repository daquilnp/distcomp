# Makefile created for Assignment 3 4F03 
# Created for the tesla server (K40)

#CC=pgcc     #gcc or pgcc
CC_OMP=gcc #gcc or pgcc
CC_ACC=pgcc #gcc or pgcc

# Note: you must have -DMONITOR to have -DBONUS_VIDEO work properly
#CFLAGS_OMP=-g -mp -02 -DVERSION_OMP -Wall -DMONITOR -DBONUS_VIDEO -DBONUS_EFF # -fopenmp and -Wall if using gcc #FOR PGCC
CFLAGS_OMP=-g -pg -fopenmp -O2 -DVERSION_OMP -DMONITOR -DBONUS_EFF #-DBONUS_VIDEO# -fopenmp and -Wall if using gcc   #FOR GCC
CFLAGS_ACC=-g -pg -fast -acc -Minfo -O2 -ta=tesla,cc30 -DVERSION_ACC #-DBONUS_EFF 

PROGRAM_NAME= genimg
PROG_OMP=$(PROGRAM_NAME)_omp
PROG_ACC=$(PROGRAM_NAME)_acc
PROG_ACC_BONUS_EFF=$(PROGRAM_NAME)2_omp
OBJS_OMP = bonus_omp.o mutate_omp.o mate_omp.o compimage_omp.o randimage_omp.o fitness_omp.o readwriteppm_omp.o main_omp.o
OBJS_ACC = mutate_acc.o mate_acc.o compimage_acc.o randimage_acc.o fitness_acc.o readwriteppm_acc.o main_acc.o #xorshift_acc.o  
OBJS_OMP_BONUS_EFF = mutate_bonus.o mate_bonus.o compimage_bonus.o randimage_bonus.o fitness_bonus.o readwriteppm_bonus.o main_bonus.o 

all: $(PROG_ACC_BONUS_EFF)

.c.o:

ifeq ($(MAKECMDGOALS),acc)
	$(CC_ACC) -c $(CFLAGS_ACC) -o $@ $?
else
	$(CC_OMP) -c $(CFLAGS_OMP) -o $@ $?
endif
	
omp: $(OBJS_OMP)
	@echo "Making omp..."       
	$(CC_OMP) $(CFLAGS_OMP) -o $(PROG_OMP) $? -lgomp

acc: $(OBJS_ACC)
	@echo "Making acc..."
	$(CC_ACC) $(CFLAGS_ACC) -o $(PROG_ACC) $? -lgomp

$(PROG_ACC_BONUS_EFF): $(OBJS_OMP_BONUS_EFF)
	@echo "Making bonus efficient algorithm..."	
	$(CC_OMP) $(CFLAGS_OMP) -o $@ $? -lgomp

clean:
	rm -f  *.o $(PROGRAM_NAME) $(PROG_OMP) $(PROG_ACC) $(PROG_ACC_BONUS_EFF) *~
