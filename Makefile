CC = g++

INCLUDES = -I/usr/include/mysql/
LIBS = -L/usr/local/lib/json/ -ljson_linux-gcc-5.4.0_libmt -L/usr/lib/mysql/ -lmysqlclient

target = server
OBJS = server.o DJson.o

all : clean ${target}

.PHONY : all clean

${target} : ${OBJS} 
	${CC} -g -o $@ $^ ${LIBS} ${INCLUDES}

%.o : %.cpp 
	${CC} -c $< -o $@ ${LIBS} ${INCLUDES}

clean:
	rm *.o ${target}
	
