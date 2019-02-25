//aathil//first code cliient.....................
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fstream>
#include <string.h>
#include <arpa/inet.h>
#include <signal.h>
#include<string>
#include <bits/stdc++.h> 
using namespace std;

int create_socket(int,char *);

#ifdef WINDOWS
    #include <direct.h>
    #define GetCurrentDir _getcwd
#else
    #include <unistd.h>
    #define GetCurrentDir getcwd
 #endif

#define MAXLINE 4096 /*max text line length*/

int
main(int argc, char **argv)
{
 int sockfd;
 struct sockaddr_in servaddr;
 char sendline[MAXLINE], recvline[MAXLINE];
 //basic check of the arguments
 //additional checks can be inserted
 if (argc !=3) {
  cerr<<"Usage: ./a.out <IP address of the server> <port number>"<<endl;
  exit(1);
 }
 //Create a socket for the client
 //If sockfd<0 there was an error in the creation of the socket
 if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) <0) {
  cerr<<"Problem in creating the socket"<<endl;
  exit(2);
 }
 //Creation of the socket
 memset(&servaddr, 0, sizeof(servaddr));
 servaddr.sin_family = AF_INET;
 servaddr.sin_addr.s_addr= inet_addr(argv[1]);
 servaddr.sin_port =  htons(atoi(argv[2])); //convert to big-endian order
 //Connection of the client to the socket
 if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr))<0) {
  cerr<<"Problem in connecting to the server"<<endl;
  exit(3);
 }
cout<<"ftp>";

 while (fgets(sendline, MAXLINE, stdin) != NULL) 
 {
	int flag=0;
  	send(sockfd, sendline, MAXLINE, 0);   //sending message to client.......................................
   	char *token,*dummy;
  	dummy=sendline;
  	token=strtok(dummy," ");
  	int h_flag=0;
   
   		if(strcmp("HEAD",token)==0)
   		{
     			h_flag=1;
   		}
		if (strcmp("GOODBYE\n",sendline)==0)  
		{
        		char m1[MAXLINE];
        		recv(sockfd, m1, MAXLINE,0);
        		cout<<m1;
       				if(flag!=0)
        				kill(0, SIGKILL); //sigint can be used.//but it still fails on large input.....
   			close(sockfd);
		        return 0;
   	       }
 else if (strcmp("GET",token)==0 || strcmp("HEAD",token)==0)  
   {
   	char port[MAXLINE], buffer[MAXLINE],char_num_blks[MAXLINE],char_num_last_blk[MAXLINE],message[MAXLINE];
	int data_port,datasock,lSize,num_blks,num_last_blk,i;
	FILE *fp;
	
	recv(sockfd, port, MAXLINE,0);
       //cout<<"p1";
	data_port=atoi(port);
	datasock=create_socket(data_port,argv[1]);
	token=strtok(NULL," ");
	recv(datasock,message,MAXLINE,0);
	//cout<<"re"<<message<<endl;
	if(strcmp("HTTP/1.1 400 Bad request",message)==0)
	{
	  cout<<message;
	  goto nextinput;
	
	}
	
	if(strcmp("1",message)==0)
		{   
			recv(datasock, char_num_blks, MAXLINE,0);
			num_blks=atoi(char_num_blks);
			for(i= 0; i < num_blks; i++) { 
				memset(buffer,0,MAXLINE);
				recv(datasock, buffer, MAXLINE,0);
				cout<<buffer;
				}
			recv(datasock, char_num_last_blk, MAXLINE,0);
			num_last_blk=atoi(char_num_last_blk);
			if (num_last_blk > 0) { 
				memset(buffer,0,MAXLINE);
				recv(datasock, buffer, MAXLINE,0);
										//fwrite(buffer,sizeof(char),num_last_blk,fp);
				cout<<buffer;
			}
			char file_tranfer_ok[100];
		        cout<<"--------------------------------------------------------------------------------\n";		     
		        cout<<"File Read complete."<<endl;
		        recv(datasock,file_tranfer_ok, MAXLINE,0);
		        cout<<file_tranfer_ok<<endl;
       			
       				if(h_flag==1)
                 			{
                 				char h_data[100];
                 				recv(datasock,h_data,MAXLINE,0);
                				cout<<h_data;
                 			}	
                      close(datasock);
		}
	if(strcmp("3",message)==0)
		{   
		        //cout<<"File dfg."<<endl;
		       recv(datasock, buffer, MAXLINE,0);
		       cout<<buffer<<endl;
                       close(datasock);
		}
	if(strcmp("0",message)==0)
		{
			char file_not_found[100];
			recv(datasock,file_not_found, MAXLINE,0);
			cout<<file_not_found;
			close(datasock);
		}
		
		
   }
   else if(strcmp("DELETE",token)==0)
   {
        char port[MAXLINE], buffer[MAXLINE],char_num_blks[MAXLINE],char_num_last_blk[MAXLINE],message[MAXLINE];
	int data_port,datasock,lSize,num_blks,num_last_blk,i;
	FILE *fp;
	
	recv(sockfd, port, MAXLINE,0);
       //cout<<"p1";
	data_port=atoi(port);
	datasock=create_socket(data_port,argv[1]);
	token=strtok(NULL," ");
	char dele[100];
	recv(datasock,dele,MAXLINE,0);
           cout<<dele<<endl;
           close(datasock);
   
  }
 else if(strcmp("PUT",token)==0)
  {
        char port[MAXLINE], buffer[MAXLINE],char_num_blks[MAXLINE],char_num_last_blk[MAXLINE],message[MAXLINE];
	int data_port,datasock,lSize,num_blks,num_last_blk,i;
	FILE *fp;
	
	recv(sockfd, port, MAXLINE,0);
	data_port=atoi(port);
	datasock=create_socket(data_port,argv[1]);
	char *tt;
	tt=strtok(NULL," \n");
	cout<<"file "<<tt;
	if ((fp=fopen(tt,"r"))!=NULL)
			{       
        		        cout<<"file found\n";
				//size of file
				fseek (fp , 0 , SEEK_END);
				lSize = ftell (fp);
				rewind (fp);
				num_blks = lSize/MAXLINE;
				num_last_blk = lSize%MAXLINE; 
				sprintf(char_num_blks,"%d",num_blks);
				send(datasock, char_num_blks, MAXLINE, 0);
			
				for(i= 0; i < num_blks; i++) { 
				memset(buffer,0,MAXLINE);
				fread (buffer,sizeof(char),MAXLINE,fp);
				send(datasock, buffer, MAXLINE, 0);
				}
				sprintf(char_num_last_blk,"%d",num_last_blk);
				send(datasock, char_num_last_blk, MAXLINE, 0);
		
				if (num_last_blk > 0) { 
				memset(buffer,0,MAXLINE);
				fread (buffer,sizeof(char),num_last_blk,fp);
				send(datasock, buffer, MAXLINE, 0);
				//cout<<buffer<<endl;
				}
			
				fclose(fp);
				close(datasock);
  				}
  		else
  		{
  		cout<<"File not FOund\n";
  		close(datasock);
  		}		
  
  
  
  }
   else if(1)
   {
   //not a valid command..........................................................
   	char port[MAXLINE], buffer[MAXLINE],char_num_blks[MAXLINE],char_num_last_blk[MAXLINE],message[MAXLINE];
	int data_port,datasock,lSize,num_blks,num_last_blk,i;
	FILE *fp;
	recv(sockfd, port, MAXLINE,0);
	data_port=atoi(port);
	datasock=create_socket(data_port,argv[1]);
 	char no_file_message[100];
	recv(datasock,no_file_message,MAXLINE,0);
   	cout<<no_file_message;
   	close(datasock);
   }
   nextinput:
  cout<<"\nftp>";

 
}
 exit(0);
}

/////////////////////////////////////////////////////////
int create_socket(int port,char *addr)
{
 int sockfd;
 struct sockaddr_in servaddr;


 //Create a socket for the client
 //If sockfd<0 there was an error in the creation of the socket
 if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) <0) {
  cerr<<"Problem in creating the socket"<<endl;
  exit(2);
 }

 //Creation of the socket
 memset(&servaddr, 0, sizeof(servaddr));
 servaddr.sin_family = AF_INET;
 servaddr.sin_addr.s_addr= inet_addr(addr);
 servaddr.sin_port =  htons(port); //convert to big-endian order

 //Connection of the client to the socket
 if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr))<0) {
  cerr<<"Problem in creating data channel"<<endl;
  exit(3);
 }

return(sockfd);
}
//first server
/*	//cout<<token;
	//cout<<strlen(token);
/*	string str=token;
	reverse(str.begin(),str.end()-1);
	char temp_array[str.length()];
	strcpy(temp_array, str.c_str());
	token=strtok(temp_array,"/ ");
	str=token;
	reverse(str.begin(),str.end());
	strcpy(token, str.c_str());
	// cout<<"\n token filename"<<token;*/
