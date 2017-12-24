# 指令编译器和选项  
CC=gcc  
CFLAGS=-Wall -std=gnu99 -g

# 静态链接库  
LDFLAGS =   
LDLIBS = -lpthread   

# 目标文件  
TARGET=server  
# 源文件  
SRCS=src/search_server.c lib/picohttpparser/picotest/picotest.c lib/picohttpparser/picohttpparser.c lib/md5/md5.c
   
OBJS = $(SRCS:.c=.o)  
     
$(TARGET):$(OBJS)  
#   @echo TARGET:$@  
#   @echo OBJECTS:$^  
	$(CC) -o $@ $^ $(LDFLAGS) $(LDLIBS) 
     
clean:  
	rm -rf $(TARGET) $(OBJS)  
       
%.o:%.c  
	$(CC) $(CFLAGS) -o $@ -c $<  
