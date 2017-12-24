#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <assert.h>

#include "../lib/picohttpparser/picotest/picotest.h"
#include "../lib/picohttpparser/picohttpparser.h"

#include <sys/syscall.h>
#include <pthread.h>

#include "../lib/md5/md5.h"

#define PORT 9003
#define QUEUE_MAX_COUNT 5
#define BUFF_SIZE 4096

#define SERVER_STRING "Server: searchhttpd/0.1.0\r\n"

struct search_item{
	char post_id[20];
	char title[250];
};

struct args{
       struct search_item ** sia;
       char * old_md5_str;
	 int * siaCount;
	 int * search_data_lock;
};


struct search_item sia[2];

struct search_item * init_search_data(int * siaCount,char * md5_str){
	const char * file_path = "data/post_online";
	Compute_file_md5(file_path, md5_str);
	
	struct search_item *sia;
	sia = malloc(sizeof(struct search_item));

	FILE *fp;
	fp=fopen("data/post_online","r");
	if(fp==NULL){
		printf("can not open file!");
	}	
	int LineNum = 0;
	while(!feof(fp)){
		char line[1024];
		fgets(line,1024,fp);
		char *ptr;
             char k = '\t';
             ptr=strchr(line,k);
		if(ptr == NULL){
			continue;
		}
		memset(sia[LineNum].post_id,0,20);
             	strncpy(sia[LineNum].post_id,line,6);	        	
		memset(sia[LineNum].title,0,250);
            	strncpy(sia[LineNum].title,ptr+1,250);
		//strcpy(sia[LineNum].title,ptr+1);
		//sia[LineNum].title[strlen(sia[LineNum].title)-1] = 0;

		LineNum++;
		sia = realloc(sia,(LineNum+1)*sizeof(struct search_item));
	}

	fclose(fp);
	*siaCount = LineNum;
	return sia;
};

char * search_from_data(struct search_item * sia,int siaCount,char * query_word,char * query_number,char * query_page){
	char * ret;
	ret = malloc(1000);
	memset(ret,0,1000);
	char * retTail;
	ret[0] = '[';
	retTail = ret+1;
	int i;
	int findNum = 0;
	int queryNumber = atoi(query_number);
	int queryPage = atoi(query_page);
	for(i=0;i<siaCount;i++){
		char * ptr;
		ptr = strstr(sia[i].title,query_word);
		if(ptr == NULL){
			continue;
		}
		findNum++;
		if(findNum <= (queryPage-1)*queryNumber){
			continue;
		}
		if(findNum > queryPage * queryNumber){
			break;
		}
		size_t j;
             j = snprintf(retTail, sizeof(sia[i].post_id)+1, "%s,", sia[i].post_id);
		retTail = retTail + j;	
	}

	if((retTail - ret) == 1){
		strncpy(retTail,"]",1);
	}else{
		strncpy(retTail-1,"]",1);
	}

	return ret;
}

int Compute_file_md5(const char * file_path,char * md5_str){
	int i;
	int fd;
	int ret;
	unsigned char data[1024];
	unsigned char md5_value[16];
	MD5_CTX md5;

	fd = open(file_path,0, "r");
	if(-1 == fd){
		perror("open");
		return -1;
	}
	
	MD5Init(&md5);

	while(1){
		ret = read(fd, data, 1024);
		if(-1 == ret){
			perror("read");
			return -1;
		}
		MD5Update(&md5, data, ret);
		if(0 == ret || ret < 1024){
			break;
		}
	}

	close(fd);

	MD5Final(&md5, md5_value);

	for(i = 0;i < 16;i++){
		snprintf( md5_str+i*2, 2+1, "%02x", md5_value[i]);
	}
	return 0;
}

void * refresh_search_data(void * data){
	while(1){
		struct args * iargs = (struct args *)data;
		const char * file_path = "data/post_online";
		char md5_str[33];
		int ret;
		ret = Compute_file_md5(file_path, md5_str);
		if(0 == ret){
			//printf("[file - %s] md5 value:\n",file_path);
			//printf("%s\n",md5_str);
		}
		if(strcmp(iargs->old_md5_str,md5_str) == 0){
			//printf("MD5 Same \n");
		}else{
			//printf("MD5 Change \n");
			memset(iargs->old_md5_str,0,33);
			strcpy(iargs->old_md5_str,md5_str);
			
			*(iargs->search_data_lock) = 1;	
			free(*(iargs->sia));
			*(iargs->sia) = init_search_data(iargs->siaCount,iargs->old_md5_str);
			*(iargs->search_data_lock) = 0;
		}
		sleep(5);
	}
}

int main(){
	int server_fd = -1;
	int client_fd = -1;

	u_short port = PORT;
	struct sockaddr_in client_addr;
	struct sockaddr_in server_addr;
	socklen_t client_addr_len = sizeof(client_addr);
	
	char buf[BUFF_SIZE];
	char recv_buf[BUFF_SIZE];
	char hello_str[] = "Hello word!";	
	int hello_len = 0;

	char QueryWord[100];
	memset(QueryWord,0,100);
	char QueryNumber[100];
	memset(QueryNumber,0,100);
	char QueryPage[100];
	memset(QueryPage,0,100);
	char JsonStr[4096];
	
	pthread_t pid;

	int search_data_lock = 0;

	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(server_fd == -1){
		perror("socket");
		exit(-1);
	}
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(server_fd,(struct sockaddr *)&server_addr,sizeof(server_addr)) < 0){
		perror("bind");
		exit(-1);
	}

	if(listen(server_fd, QUEUE_MAX_COUNT) < 0){
		perror("listen");
		exit(-1);
	}
	
	printf("http server running on port %d\n", port);

	//初始化检索数据
	int siaCount = 0;
	char file_md5_str[33];
	memset(file_md5_str,0,33);
	struct search_item *sia = init_search_data(&siaCount, file_md5_str);
	printf("SEARCH_DATA NUM; %d\n",siaCount);
	char test[3];
	memset(test,0,3);
	strncpy(test,sia[10001].title,3);
	//初始化检索数据结束
	
	//更新检索源数据开始
	int ret = 0;
	struct args iargs;
	iargs.sia =  &sia;
	iargs.old_md5_str = file_md5_str;
	iargs.siaCount = &siaCount;
	iargs.search_data_lock = &search_data_lock;
	ret = pthread_create(&pid,NULL,(void*)refresh_search_data,(void *)& iargs);
	if(ret){
		printf("create pthread error!\n");
	}
	//更新检索源数据结束
	
	while(1){
		client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
		if(client_fd < 0){
			perror("accept");
			exit(-1);
		}
		
		size_t buflen = 0;
		hello_len = recv(client_fd, recv_buf + buflen , BUFF_SIZE - buflen,0);

		const char *method, *path;
		int pret, minor_version;
		struct phr_header  headers[100];
		size_t  prevbuflen = 0, method_len, path_len, num_headers;

		/* parse the request */
		buflen += hello_len;
		num_headers = sizeof(headers) / sizeof(headers[0]);
		pret = phr_parse_request(recv_buf, buflen, &method, &method_len, &path, &path_len,
                             &minor_version, headers, &num_headers, prevbuflen);


		int checkFlag = 1;
		char seg[] = "&";
		char *p;
		p = strchr(path,'&');
		if( !p || !*p){
			checkFlag = -9;			
		}
		if((int)path_len <3){
			checkFlag = -8;
		}
		if(checkFlag > 0){
			char str[(int)path_len];
			memcpy(str, path+2, (int)path_len-2);
			char *substr= strtok(str, seg);
			while (substr != NULL) { 
                	
				char *ptr;
				char k = '=';
				ptr=strchr(substr,k);
				if(!ptr){
					substr = strtok(NULL,seg);
					continue;
				}
				char get_key[100];
				memset(get_key,0,sizeof(get_key));
				memcpy(get_key,substr,ptr-substr);
				char get_value[100];
				memset(get_value,0,100);
				//strcpy(get_value,ptr+1);
				int result = snprintf(get_value, sizeof(get_value), "%s", ptr+1); 
				if(result==sizeof(get_value) || result<0){
					get_value[sizeof(get_value)-1] = 0;
				}
				char handle_key[100] = {'q','u','e','r','y','_','w','o','r','d'};
				if(strcmp(get_key,handle_key)==0){
					memset(QueryWord,0,100);
					//strcpy(QueryWord,get_value);
					int result = snprintf(QueryWord, sizeof(QueryWord), "%s", get_value); 
					if(result==sizeof(QueryWord) || result<0){
						QueryWord[sizeof(QueryWord)-1] = 0;
					}
				}
				char handle_key1[100] = {'q','u','e','r','y','_','n','u','m','b','e','r'};
				if(strcmp(get_key,handle_key1)==0){
					memset(QueryNumber,0,100);
					//strcpy(QueryNumber,get_value);
					int result = snprintf(QueryNumber, sizeof(QueryNumber), "%s", get_value); 
					if(result==sizeof(QueryNumber) || result<0){
						QueryNumber[sizeof(QueryNumber)-1] = 0;
					}
				}
				char handle_key2[100] = {'q','u','e','r','y','_','p','a','g','e'};
				if(strcmp(get_key,handle_key2)==0){
					memset(QueryPage,0,100);
					//strcpy(QueryPage,get_value);
					int result = snprintf(QueryPage, sizeof(QueryPage), "%s", get_value); 
					if(result==sizeof(QueryPage) || result<0){
						QueryPage[sizeof(QueryPage)-1] = 0;
					}
				}
                		substr = strtok(NULL,seg);
			}
		}
		//检查参数
		if(atoi(QueryNumber) < 1 || atoi(QueryNumber) > 100){
			if(checkFlag > 0){
				checkFlag = -1;
			}			
		}
		if(atoi(QueryPage) < 1 || atoi(QueryPage) > 100){
			if(checkFlag > 0){
				checkFlag = -2;
			}
		}
		printf("ACCESS LOG: flag[%d] QueryWord[%s] QueryPage[%s] QueryNumber[%s]\n",checkFlag,QueryWord,QueryPage,QueryNumber);
		//遍历结构体数组搜索开始
		while(1){
			if(search_data_lock == 0){
				break;
			}
			usleep(100);
		}
		
		if(checkFlag > 0){
			char * retStr;
			retStr= search_from_data(sia,siaCount,QueryWord,QueryNumber,QueryPage);
			strcpy(JsonStr,retStr);
			free(retStr);
		}else{
			snprintf(JsonStr, sizeof(JsonStr), "%s", "check param failed"); 
		}		
		//遍历结构体数组搜索模式结束

		sprintf(buf , "HTTP/1.0 200 OK\r\n");
		send(client_fd, buf, strlen(buf),0);
		strcpy(buf, SERVER_STRING);
		send(client_fd, buf, strlen(buf),0);
		strcpy(buf,"\r\n");
		send(client_fd, buf, strlen(buf), 0);
		sprintf(buf,JsonStr);
		send(client_fd, buf, strlen(buf),0);

		//clean
		memset(QueryWord,0,100);
		memset(QueryNumber,0,100);
		memset(QueryPage,0,100);

		close(client_fd);
	}
	close(server_fd);
	return 0;	
}
