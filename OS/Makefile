CC = gcc
CFLAGS= -Wall
OBJS =  main.c modules/lexer.c modules/filesystem.c  modules/commands.c modules/user.c modules/userActions.c modules/utils.c modules/scheduler.c
LIBS =

main: ${OBJ}
	$(CC) ${OBJS} ${LIBS} ${CFLAGS} -o ../server/OS/$@
