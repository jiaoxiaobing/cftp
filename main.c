/*
 * main.c
 *
 *  Created on: 2017��11��2��
 *      Author: jiaobing
 */
#include "ftp_client.h"
/*******************************************************
*���ܣ�
*	get��ȡԶ����һ���ļ�
*	put������Զ��һ���ļ�
*	pwd����ʾԶ����ǰĿ¼
*	dir���г�Զ����ǰĿ¼
*	cd ���ı�Զ����ǰĿ¼
*	�� ����ʾ���ṩ������
*	ls ����ʾ����Ŀ¼
*	quit���˳�����
*
*********************************************************/


/******************�ϴ��ļ���������*******************/
int put_file(char *file)
{
	int ffd;			//file �ļ�������
	char buf[256];		//������
	int total_c;		//�ļ��ܴ�С
	int sent_c = 0;		//�Ѿ����͵��ֽ���
	int snd_c;			//�����ֽ���
	int ret;			//������ֵ
	int ack;			//�ź�ȷ��

	/*��Ҫ���͵��ļ�*/
	ffd = open(file, O_RDONLY);
	if(ffd == -1)
	{
		printf("open %s failled\n", file);
		goto SNDERR;
	}

	/*����READY_T���ź�*/
	ack = READY_T;
	ret = send(datafd, &ack, 4, 0);
	/*�ȴ��Է�����ȷ�ϵ�Ӧ��*/
	ret = recv(datafd, &ack, 4, MSG_WAITALL);
	if(ack == FAILL_T)
		goto SNDERR;
	/*�����ļ��ܴ�С*/
	total_c = lseek(ffd, 0, SEEK_END);
	lseek(ffd, 0, SEEK_SET);
	printf("have %.2f KB to send\n", (float)total_c/1024);
	ret = send(datafd, &total_c, sizeof(total_c), 0);



	/*��ʼ�����ļ�*/
	printf("sending %s .......\n", file);
	snd_c = 255;
	while(1)
	{
		bzero(buf, sizeof(buf));
		//���ļ��ж�ȡsnd_c�ֽڵ�����
		ret = read(ffd, &buf, snd_c);
		//����snd_c���ֽ�
		ret = send(datafd, buf, snd_c, 0);
		//���ֽ���-�ѷ����ֽ���
		total_c -= ret;
		sent_c  += ret;
		if(total_c < snd_c)
			snd_c = total_c;
		if(total_c <= 0)
			break;
	}
	/*���ͷ�������ź�*/
	ack=OK_T;
	ret = send(datafd, &ack, 4, 0);
	close(ffd);
	/*�ȴ��Է�������ɵ�Ӧ��*/
	ret = recv(datafd, &ack, 4, 0);
	printf("send %.2f KB\n", (float)sent_c/1024);
	printf("send %s success\n", file);
	return 0;

//������
SNDERR:
	//��ӡ������Ϣ
	perror("send err");
	//���ͽ��մ����ź�
	ack = FAILL_T;
	ret = send(datafd, &ack, 4, 0);
	//�ر��ļ�
	close(ffd);
	return -1;

}
/******************���ط��������������ļ�******************/
int dowload(char *file)
{
	int file_c;		//�ļ��ܴ�С
	int get_c;		//���յ����ֽ���
	int got_c=0;	//�Ѿ����յ��ֽ���
	char buf[512];	//���ջ���
	int ffd;		//�ļ�������
	int ret;		//������ֵ
	int ack;		//�ź�ȷ��

	/*�ȴ��Է�����READY_T���ź�*/
	ret = recv(datafd, &ack, 4, MSG_WAITALL);
	if(ack != READY_T)
	{
		printf("file donot exist\n");
		return -1;
	}

	/*�򿪻򴴽��ļ�*/
	ffd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if(ffd == -1)
	{
		printf("open file failled\n");
		goto RECVERR;
	}
	/*����ȷ���ź�*/
	ack = OK_T;
	ret = send(datafd, &ack, 4, 0);

	/*�ȴ��Է������ļ��ܴ�С*/
	ret = recv(datafd, &file_c, 4, MSG_WAITALL);
	printf("have %.2f KB to download\n", (float)file_c/1024);
	printf("downloading %s.......\n", file);

	/*��ʼ����*/
	get_c = 512;
	while(1)
	{
		//��ջ���
		bzero(buf, sizeof(buf));

		if(file_c < get_c)
				get_c = file_c;
		//��������
		ret = recv(datafd, buf, get_c, MSG_WAITALL);
		if(ret > 0)
		{
			//�ѽ��յ�������д���ļ�
			write(ffd, buf, ret);
			//�ܴ�С-���յ����ֽ���
			file_c -= ret;
			//�����յ�������
			got_c  += ret;
		}
		else if(ret == 0)
		{
			printf("download %s success\n", file);
			break;
		}
		else if(ret == -1)
		{
			printf("internet err!\n");
			break;
		}
	}
	/*�ȴ��������ȷ���ź�*/
	ret = recv(datafd, &ack, 4, 0);
	ack = OK_T;
	/*�����Խ������ȷ���ź�*/
	ret = send(datafd, &ack, 4, 0);
	printf("download %.2f KB\n", (float)got_c/1024);
	close(ffd);
	return 0;

//������
RECVERR:
	//��ӡ������Ϣ
	perror("download err");
	//���ͽ��մ����ź�
	ack = FAILL_T;
	ret = send(datafd, &ack, 4, 0);
	//�ر��ļ�
	close(ffd);
	return -1;

}
/******************��ʾ������Ϣ*******************/
void show_help()
{
	printf("================��������==================\n");
	printf("   ==get <file> �����ļ�==\n   ==put <file> �ϴ��ļ�== \
		  \n   ==dir �г�Զ����ǰĿ¼==\n   ==pwd ��ʾԶ����ǰĿ¼==\
		  \n   ==cd <dir> �г�Զ����ǰĿ¼==\n   ==? ��ʾ�������==\
		  \n   ==ls �г�����Ŀ¼==\n   ==quit �˳�==\n");
	printf("==========================================\n");
}

/******************��������*******************/
int get_cmd(char *msg)
{
	int cmd = -1;
	if(strncmp(msg, "get", 3) == 0 || strncmp(msg, "GET", 3) == 0) cmd = GET;
	else if(strncmp(msg, "put", 3) == 0 || strncmp(msg, "PUT", 3) == 0) cmd = PUT;
	else if(strncmp(msg, "pwd", 3) == 0 || strncmp(msg, "PWD", 3) == 0) cmd = PWD;
	else if(strncmp(msg, "dir", 3) == 0 || strncmp(msg, "DIR", 3) == 0) cmd = GDIR;
	else if(strncmp(msg, "cd",  2) == 0 || strncmp(msg, "cd", 2) == 0) cmd = CD;
	else if(strncmp(msg, "?",   1) == 0 )  cmd = HELP;
	else if(strncmp(msg, "ls",  2) == 0 || strncmp(msg, "LS", 2) == 0) cmd = LS;
	else if(strncmp(msg, "quit",4) == 0 || strncmp(msg, "QUIT", 4) == 0) cmd = QUIT;
	else if(strcmp(msg, "q") == 0 || strcmp(msg, "Q") == 0) cmd = QUIT;
	else printf("command not found (? for help)\n");
	return cmd;
}


/*******************�ļ�������߳�********************/
void *transfer_routine(void *arg)
{
	//�жϷ��ͻ��ǽ���
	switch(flag)
	{
		case PUT:	//�����ļ�
			put_file(file_name);
			break;
		case GET:	//�����ļ�
			dowload(file_name);
			break;
	}
	//������ʾ
	write(0,"myftp>", sizeof("myftp>"));
}

/*******************���ղ���ʾ����ǰĿ¼������********************/
void get_dir()
{
	int r = 1;			//��ʼ����һ���ֽ�
	int get_c = 0;		//�Ѿ����յ��ֽ���
	char r_buf[256];	//���յ��������ļ�������
	char temp_buf[256];	//���ջ�����
	//��ջ���
	bzero(r_buf, sizeof(r_buf));
	while(1)
	{	//��ջ���
		bzero(temp_buf, sizeof(temp_buf));
		//����,����tcp���ֽ���������Ƚ����ļ������ȣ��ٽ����������ļ���
		r = recv(fd, temp_buf, r, MSG_WAITALL);
		//�����ѽ��յ��ֽ���
		get_c += r;
		//�����ѽ��յ��ֽ���
		strcat(r_buf, temp_buf);
		//Ҫ���յ��ֽ���Ϊ0ʱ�������
		if(r_buf[0] == 0) break;
		//�ж��Ƿ��Ѿ��������յ�һ���ļ���
		if((char)get_c != r_buf[0])
		{
			r = (int)r_buf[0]-get_c;
			continue;
		}
		r_buf[get_c] = '\0';
		r_buf[0] = '*';
		//��ӡ
		printf("%s\n", r_buf);
		//��ʼ��
		bzero(r_buf, sizeof(r_buf));
		get_c = 0;
		r = 1;

	}
	printf("===================================\n");
	//������ʾ
	write(0,"myftp>", sizeof("myftp>"));
}

/******************���շ�������Ϣ�߳�*******************/
void *rec_msg(void *arg)
{
	char r_buf[256];	//���ջ���
	int r;				//����ֵ����
	pthread_t tid;		//�߳�id
	int *ack = (int *)r_buf;

	while(1)
	{
		//��ջ���
		bzero(r_buf, sizeof(r_buf));
		//����
		r = recv(fd, r_buf, 255, 0);
		if(r == -1)
		{
			perror("recv fialed");
			continue;
		}
		r_buf[r] = '\0';

		//�жϹ�����ʽ
		switch(flag)
		{
			case GET:		//�����ļ�
				if(*ack == READY_T)
				{
					printf("ready to dowload\n");
					pthread_create(&tid, NULL, &transfer_routine, NULL);
				}
				is_data = 0;
				break;
			case PUT:		//�ϴ��ļ�
				if(*ack == READY_T)
				{
					printf("ready to sent\n");
					pthread_create(&tid, NULL, &transfer_routine, NULL);
				}
				is_data = 0;
				break;
			case GDIR:		//��ȡĿ¼����
					printf("\n");
					get_dir();
				break;
			case PWD:		//��ǰ·��
				printf("%s\n", r_buf);
				//������ʾ
				write(0,"myftp>", sizeof("myftp>"));
				break;
			case CD:		//�ı䵱ǰ·��
				printf("%s\n", r_buf);
				//������ʾ
				write(0,"myftp>", sizeof("myftp>"));
				break;
		}
	}
}


/***********�ȴ�����TCP����***************/
void *data_routine(void *arg)
{
	socklen_t len;
	len = sizeof(data_adr);
	while(1)
	{
		//�ȴ�����������
		datafd = accept(dfd, (struct sockaddr *)&(data_adr), &len);
		is_data = 1;
	}
}

/******************�г���ǰĿ¼���ļ���Ŀ¼*******************/
int ls_dir()
{
	DIR *dp = opendir("./");
	struct dirent *ep;
	int r;
	char type[104] = "---";

	if(dp == NULL)
	{
		perror("opendir() failed");
		return -1;
	}
	errno = 0;
	while(1)
	{
		ep = readdir(dp);
		if(ep == NULL && errno == 0)
		{
			break;
		}
		else if(ep == NULL)
		{
			perror("readdir() failed");
			return -1;
		}
		if(ep->d_name[0] == '.')
		{
			continue;
		}
		bzero(type, sizeof(type));
		type[0] = '*';
		if(ep->d_type == DT_DIR)
			type[1] = 'd';
		else
			type[1] = '-';
		type[2] = '-';
		type[3] = '-';
		strncat(type, ep->d_name, strlen(ep->d_name)+5);
		printf("%s\n", type);
		if(r == -1) perror("send err");
	}
	closedir(dp);
	return 0;
}

/****�ͷ���Դ���˳�******/
void quit()
{
	shutdown(datafd, SHUT_RDWR);
	shutdown(fd, SHUT_RDWR);
	close(datafd);
	close(fd);
	exit(0);
}

/**** ����̨�������� ******/
void cmd_console()
{
	int r;				//������ֵ
	char s_buf[256];	//������
	char *sent_buf = s_buf;
	int local_cmd = 0;

	while(1)
	{	//��ջ���
		bzero(s_buf, sizeof(s_buf));
		//������ʾ
		write(0,"myftp>", sizeof("myftp>"));
		//���ն���������
		gets(s_buf);
		if(s_buf[0] == '\0') continue;
		//�������������,�ж���������
		switch(get_cmd(s_buf))
		{
			case GET:	//�����ļ�
				if(strlen(s_buf) < 5)
				{
					printf("pls input get <file>\n");
					break;
				}
				//����get�����ַ�
				sent_buf += 3;
				//���ô����ļ���
				strcpy(file_name, sent_buf+1);
				printf("%s\n",file_name);
				//���ù���ģʽΪ�����ļ�
				flag = GET;
				//���õ�һ���ַ�Ϊ������
				sent_buf[0] = (char) flag;
				break;
			case PUT:	//�ϴ��ļ�
				if(strlen(s_buf) < 5)
				{
					printf("pls input put <file>\n");
					break;
				}
				//����put�����ַ�
				sent_buf += 3;
				//���ô����ļ���
				strcpy(file_name, sent_buf+1);
				//���ù���ģʽΪ�ϴ��ļ�
				flag = PUT;
				sent_buf[0] = (char) flag;
				break;
			case GDIR:	//��ȡĿ¼����
				flag = GDIR;
				printf("=========��������ǰĿ¼�ļ�=========\n");
				sent_buf[0] = (char) flag;
				sent_buf[1]	= '\0';
				break;
			case PWD:	//��ȡ��ǰ·��
				flag = PWD;
				//���������������
				sent_buf[0] = (char) flag;
				sent_buf[1]	= '\0';
				//r = send(fd, s_buf, strlen(s_buf), 0);
				break;
			case CD:	//�ı䵱ǰ·��
				flag = CD;
				sent_buf += 2;
				sent_buf[0] = (char) flag;
				break;
			case HELP:	//��ʾ���������Ϣ
				show_help();
				local_cmd = 1;
				break;
			case LS:
				printf("=========���ص�ǰĿ¼�ļ�=========\n");
				ls_dir();
				printf("=================================\n");
				local_cmd = 1;
				break;
			case QUIT:	//�˳�
				quit();
				break;
			default:
				break;
		}
		if(!local_cmd)
		{
			//���������������
			r = send(fd, sent_buf, strlen(sent_buf), 0);
			//ָ�ػ������Ŀ�ͷ
			sent_buf = s_buf;
		}
		local_cmd = 0;
	}
}

/******************������*******************/
int main(int argc, char const *argv[])
{

	/***Ĭ�Ϸ�����IP��ַ*****/
	char sip[20] = "192.168.1.210";

	int r;				//������ֵ
	pthread_t tid;		//�߳�id


	int ack;
	int data_port = 31629;	//���ݴ���˿�

	if(argc < 2)
	{
		printf("pls input: %s [ip]\n", argv[0]);
		exit(0);
	}
	flag = -1;			//����ģʽ��ʼ��Ϊ��
	is_data = 0;
	port = 2121;		//Ĭ�϶˿�Ϊ2121
	if(argc > 2)
	{
		//�Լ����ض���˿�
		int n = strlen(argv[2]);
		int i;
		data_port = 0;
		for( i=0; i<n; i++)
		{
			if('0'<= argv[2][i] && argv[2][i] <= '9')
			{
				data_port += (argv[2][i] - '0')*(i+1);
			}
		}
	}

	strncpy(sip, argv[1], strlen(argv[1])+1);


	printf("server_ip = %s : %d\n", sip, port);

	//1.socket	���������׽��֣�һ���������ӷ�������һ�������ļ�����
	fd = socket(AF_INET, SOCK_STREAM, 0);
	dfd = socket(AF_INET, SOCK_STREAM, 0);
	if(fd == -1 || datafd == -1)	perror("socket err:"), exit(-1);

	//2.connect
	sadr.sin_family = AF_INET;
	sadr.sin_port = htons(port);
	inet_aton(sip, &sadr.sin_addr);

	data_adr.sin_family = AF_INET;
	data_adr.sin_port = htons(data_port);
	data_adr.sin_addr.s_addr = htonl(INADDR_ANY);

	//�󶨵�ַ
	r = bind(dfd, (struct sockaddr *)&data_adr, sizeof(data_adr));
	if(r == -1) perror("2.bind: "), exit(-1);
	r = listen(dfd, 1);
	if(r == -1) perror("3.listen: "), exit(-1);

	//���ӷ�����
	r = connect(fd, (struct sockaddr *)&sadr, sizeof(sadr));
	if(r == -1) perror("���ӷ���������"), exit(-1);

	/*�ȴ��Է����ͷ���Ȩ��*/
	r = recv(fd, &ack, 4, MSG_WAITALL);
	if(ack == FAILL_T)
	{
		printf("�������ܾ�����,���Ժ�����\n");
		ack=OK_T;
		send(fd, &ack, sizeof(ack), 0);
		sleep(2);
		return 0;
	}
	else
	{
		printf("���ӷ������ɹ�\n");
	}

	//���������������߳�
	r = pthread_create(&tid, NULL, &rec_msg, NULL);
	//��ʾ���������Ϣ
	show_help();
	//�����ȴ����ݴ���TCP�����߳�
	r = pthread_create(&tid, NULL, &data_routine, NULL);

	//���Ͷ˿�
	r = send(fd, &data_port, 4, MSG_WAITALL);

	cmd_console();

	return 0;
}

