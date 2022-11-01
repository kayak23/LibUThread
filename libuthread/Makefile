# Target library
lib 	:= libuthread.a
objs 	:= queue.o context.o uthread.o preempt.o sem.o 

CC		:= gcc
CFLAGS	:= -Wall -Wextra -Werror -MMD
CFLAGS	+= -g

all: $(lib)

deps := $(patsubst %.o,%.d,$(objs))
-include $(deps)

$(lib): $(objs)
	@echo "create library"
	@ar rcs $(lib) $(objs)

%.o: %.c 
	@echo "CC	$@"
	@$(CC) $(CFLAGS) -c -o $@ $<	

clean: 
	@echo "CLEAN"
	@rm -f $(lib) $(objs) $(deps)
