#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/select.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>

#define PORT 8080

char *ip_addr = NULL;

FILE *fp = NULL;

// for alsa params
int err;
int break_flag, start_print_progress_flag=0;

pthread_mutex_t lock;

void *socket_write(){

	struct sockaddr_in si_me;
	int s, i, j, ret, slen=sizeof(si_me), cnt=0, cnt2=0;
	char data[] = "get";
	unsigned char buf[8192];
	int readfile_flag = 1;
	struct timeval tm;
	long long t1, t2;

	if((s = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP)) == -1){		
		fprintf(stderr, "socket create fail!!\n");
		break_flag = 1;
		return 0;
	}
	else
		fprintf(stderr, "create socket success!!!\n");

	memset((char *) &si_me, 0, sizeof(si_me));
	si_me.sin_family = PF_INET;
	si_me.sin_port = htons(PORT);	
	si_me.sin_addr.s_addr = inet_addr(ip_addr); // for example

	if(connect(s, (struct sockaddr *)&si_me, sizeof(si_me)) < 0){
		fprintf(stderr, "socket connect error, %s\n", strerror(errno));
		break_flag = 1;
		return;
	}
	else
		fprintf(stderr, "socket connecting success and starting to transfer file....\n");

	
	gettimeofday(&tm, NULL);
	t1 = tm.tv_sec; 

	start_print_progress_flag = 1;	
	while(1){
		ret = fread(buf, 1, 8192, fp);
		if(ret > 0)
			ret = send(s, buf, ret, 0);
		else if(ret = 0 && cnt == 0){
			fprintf(stderr, "file size =0 byte, please check\n");
			break;
		}
		cnt += ret;
		//if(feof(fp))
		if(ret < 8192){
			start_print_progress_flag = 0;
			break;
		}
		//cnt2++;
		//if(cnt2 >= 20){
		//	fprintf(stderr, "-");
		//	cnt2 = 0;
		//}
	}
	gettimeofday(&tm, NULL);
	t2 = tm.tv_sec; 

	if(ret < 0){		
		fprintf(stderr, "\nsocket sending error, %s\n", strerror(errno));
		break_flag = 1;
	}
	else if (ret >= 0)
		fprintf(stderr, "\nsending data success....transfer file size is = %dbytes, \ntransfering time: %llusecs\n", cnt, t2-t1);
	cnt = 0;

	//sendto(s, data, sizeof(data), 0, (struct sockaddr *) &si_me, sizeof(si_me));

	close(s);
	break_flag = 1;
}

void *printProgress(){

	int cnt2 = 0;	
	while(1){
		if(start_print_progress_flag){
			cnt2++;
			if(cnt2 >= 10000){
				fprintf(stderr, "-");
				cnt2 = 0;
			}
		}
		usleep(1);
	}

}

int main(int argc, char *argv[]){

	int ret, i=0;	

	if(argc < 2){
		fprintf(stderr, "please use correct parameters\n");
		return;
	}

	for(i=1 ; i<argc ; i++){
		if(strcmp(argv[i], "-p") == 0){
			ip_addr = (char *)malloc(64);
			ip_addr = argv[i+1];
			//memcpy(ip_addr, argv[i+1], sizeof(argv[i+1]));
		}
		
		if(strcmp(argv[i], "-f") == 0){
			//file_name = (char *)malloc(sizeof(argv[i+1]));
			fp = fopen(argv[i+1], "rb");
			if(fp < 0){
				fprintf(stderr, "file opened fail\n");
				return -1;
			}
		}
	}
	if(ip_addr == NULL){
		fprintf(stderr, "need ip address!!!\n");
		return -1;
	}
	else{
		fprintf(stderr, "server ip:%s\n", ip_addr);
	}
	

	//if (pthread_mutex_init(&lock, NULL) != 0)
	//{
	//        fprintf(stderr, "\n mutex init failed\n");
        //	return 1;
    	//}

	pthread_t id_socket_write, id_print_progress;	
	ret = pthread_create(&id_socket_write, NULL, socket_write, NULL);
	if(ret != 0){
		fprintf(stderr, "socket_recv thread creation fail!!!\n");
		return -1;
	}
	ret = pthread_create(&id_print_progress, NULL, printProgress, NULL);
	if(ret != 0){
		fprintf(stderr, "printfProgress thread creation fail!!!\n");
		return -1;
	}

	//pthread_join(id_socket_write, NULL);
	break_flag = 0;
	while(!break_flag){	
		usleep(1);
  	}

	//if(ip_addr != NULL)
	//	free(ip_addr);
	fclose(fp);
	
	//pthread_mutex_destroy(&lock);		
}
