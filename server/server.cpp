#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <fstream>
#include <unistd.h>
#include <chrono> 
#include <ctime> 
#include<string>
#include <bits/stdc++.h> 
#include <sys/stat.h>
using namespace std;

int create_socket(int);
int accept_conn(int);

#ifdef WINDOWS
    #include <direct.h>
    #define GetCurrentDir _getcwd
#else
    #include <unistd.h>
    #define GetCurrentDir getcwd
 #endif

#define MAXLINE 4096 /*max text line length*/
#define LISTENQ 8 /*maximum number of client connections*/
string getFileCreationTime(char *path) {
    struct stat attr;
    stat(path, &attr);
    
    printf("Last modified time: %s", ctime(&attr.st_mtime));
    string time=ctime(&attr.st_mtime);
    //char ctime[time.length()+1];
    //strcpy(ctime, time.c_str());
    return ctime(&attr.st_mtime);
}
int main (int argc, char **argv)
{
 int listenfd, connfd, n;
 pid_t childpid;
 socklen_t clilen;
 char buf[MAXLINE];
 struct sockaddr_in cliaddr, servaddr;

 if (argc !=2) {						//validating the input
  cerr<<"Usage: ./a.out <port number>"<<endl;
  exit(1);
 }
 

 //Create a socket for the soclet
 //If sockfd<0 there was an error in the creation of the socket
 if ((listenfd = socket (AF_INET, SOCK_STREAM, 0)) <0) {
  cerr<<"Problem in creating the socket"<<endl;
  exit(2);
 }


 //preparation of the socket address
memset(&servaddr, 0, sizeof(servaddr));
 servaddr.sin_family = AF_INET;
 servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
// if(atoi(argv[1])<=1024){
//	cerr<<"Port number must be greater than 1024"<<endl;
//	exit(2);
 //}
 servaddr.sin_port = htons(atoi(argv[1]));

 //bind the socket
 bind (listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

 //listen to the socket by creating a connection queue, then wait for clients
 listen (listenfd, LISTENQ);

 cout<<"Server running...waiting for connections."<<endl;
//وَيَسِّرْ لِىٓ أَمْرِى
 for ( ; ; ) {

  clilen = sizeof(cliaddr);
  //accept a connection
  connfd = accept (listenfd, (struct sockaddr *) &cliaddr, &clilen);
  cout<<"Received request..."<<endl;

  if ( (childpid = fork ()) == 0 ) {//if it’s 0, it’s child process

  cout<<"Child created for dealing with client requests"<<endl;
  //close listening socket
  close (listenfd);
  int data_port=1024;	
					//for data connection
  while ( (n = recv(connfd, buf, MAXLINE,0)) > 0)  {
  
  int file_name_flag=0;
   cout<<"String received from client: "<<buf;
   char *tok2,*token,*dummy;
   dummy=buf;
   token=strtok(dummy," ");
    int h_flag=0;
   
   if (strcmp("GOODBYE\n",buf)==0)  
   {
   	cout<<"The client has quit\n";
   	
        send(connfd,"THANK YOU ALL CONNECTIONS SUCCESSFULLY TERMINATED",MAXLINE,0);
        break; 
   }
   if(strcmp("HEAD",token)==0)
   {
    h_flag=1;
   
   }
  if (strcmp("GET",token)==0 || strcmp("HEAD",token)==0)  
   {   
        
	char port[MAXLINE],buffer[MAXLINE],char_num_blks[MAXLINE],char_num_last_blk[MAXLINE];
	int datasock,datasock1,lSize,num_blks,num_last_blk,i;
	FILE *fp;
	tok2=token=strtok(NULL," \n");
	cout<<"Filename given is: "<<token<<endl;
					//accepting connnection by client
        
         
        data_port=8000;
        sprintf(port,"%d",data_port);
        datasock1=create_socket(data_port);				//creating socket for data connection
	send(connfd, port,MAXLINE,0);					//sending port no. to client
	datasock=accept_conn(datasock1);	 
        close(datasock1);
        char *http=strtok(NULL,"\n");
        if(strcmp("HTTP/1.1",http)!=0)
        {
          char bad_req[]="HTTP/1.1 400 Bad request";
       
         
         send(datasock,bad_req,MAXLINE,0);
			close(datasock);
	goto  nextoption;					  
        }
         cout<<"size token"<<sizeof(token)/sizeof(token[0]);
	if(token[1]== 0 && token[0]=='/')
	{
           file_name_flag=1;
           strcpy(token,"index.html");
           strcpy(tok2,"index.html");
           cout<<"----------";
         } 
        else
        {      
	string str=token;
	
	strcpy(tok2,token);
	reverse(str.begin(),str.end());
	char temp_array[str.length()];
	strcpy(temp_array, str.c_str());
	token=strtok(temp_array,"/ ");
	str=token;
	reverse(str.begin(),str.end());
	strcpy(token, str.c_str());

	//*tok2++;
//	cout<<tok2;
	/*if(tok2[0]=='/' && sizeof(tok2)==1)
	{
	  cout<<"index";
	   strcpy(tok2,"index.html");
	}
	if(tok2[0]=='/' && sizeof(tok2)>1 )
	{
	*tok2++;
	cout<<token;
	}*/
	// cout<<"\n token filename"<<token;
	//cout<<tok2;
	//cout<<"\nhjjjhj"<<token;
	//cout<<"\nDFgdfg\n";
	//cout<<"\nt1"<<token[0]<<"\nt2"<<token[1];
	cout<<"\ntokfile"<<tok2;
         }   
		if( token[0]=='1' && token[1]=='1' && (fp=fopen(tok2,"r"))!=NULL) 	
  		{    
  			cout<<"not accesiblen\n";
			send(datasock,"3",MAXLINE,0);
			char file_not_accesible[]="HTTP/1.1 403";
			send(datasock,file_not_accesible,MAXLINE,0);
			close(datasock);	
 		}
		else
    		{	
	
			if ((fp=fopen(tok2,"r"))!=NULL)
			{       
        		        cout<<"file found\n";
				//size of file
				send(datasock,"1",MAXLINE,0);
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
				//file read complete
				char file_transfer_ok[]="HTTP/1.1 200 ok";
				send(datasock, file_transfer_ok, MAXLINE, 0);
				
				//if instruction is head
				if(h_flag==1)
				{
				char file_mod_time[100];
				string ftime=getFileCreationTime(tok2);
				cout<<ftime;
				char file_time[ftime.length()];
				strcpy(file_time, ftime.c_str());
		       		send(datasock,file_time, MAXLINE, 0);
				}
				close(datasock);
                
		
				}
				else
				{ 
				cout<<"file not found\n";
				send(datasock,"0",MAXLINE,0);
				char hello[]="HTTP/1.1 404 file not found";
				send(datasock,hello,MAXLINE,0);
				fflush(stdin);
				close(datasock);
				}
   		}
   }
   if(strcmp("DELETE",token)==0)
   {
        char port[MAXLINE],buffer[MAXLINE],char_num_blks[MAXLINE],char_num_last_blk[MAXLINE];
	int datasock,datasock1,lSize,num_blks,num_last_blk,i;
	FILE *fp;	 
	 
	data_port=1080;
	fflush(stdin);      
	
	sprintf(port,"%d",data_port);
	datasock1=create_socket(data_port);				//creating socket for data connection
	send(connfd, port,MAXLINE,0);					//sending port no. to client
	datasock=accept_conn(datasock1);	
        close(datasock1);
        token=strtok(NULL," \n");
        char *dhttp;
        dhttp=strtok(NULL,"\n");
        cout<<"file patrh"<<token<<"EE"<<dhttp;
        if(strcmp("HTTP/1.1",dhttp)!=0)
        {
        char bad_req[]="HTTP/1.1 400 BAd request";
        send(datasock,bad_req,MAXLINE,0);
        close(datasock); 
        goto nextoption;
             
        }
      
        if (access(token, F_OK) == 0)
        {
         remove(token);
         char fdel[]="File deleteded";
        send(datasock,fdel,MAXLINE,0);
        
        }
        else
        {
        char bad_met[]="HTTP/1.1 405” Method Not allowed";
        send(datasock,bad_met,MAXLINE,0);
        
        }
          close(datasock); 
   }
   if(strcmp("PUT",token)==0)
   {
  	char port[MAXLINE],buffer[MAXLINE],char_num_blks[MAXLINE],char_num_last_blk[MAXLINE];
	int datasock,datasock1,lSize,num_blks,num_last_blk,i;
	FILE *fp;	 
	 
	data_port=1080;
	fflush(stdin);      
	
	sprintf(port,"%d",data_port);
	datasock1=create_socket(data_port);				//creating socket for data connection
	send(connfd, port,MAXLINE,0);					//sending port no. to client
	datasock=accept_conn(datasock1);	
        close(datasock1);
        char *tt;
         tt=strtok(NULL," \n");
         cout<<"put fn"<<tt;
                        if ((fp=fopen(tt,"w"))!=NULL) 
                        {  
                        recv(datasock, char_num_blks, MAXLINE,0);
			num_blks=atoi(char_num_blks);
			for(i= 0; i < num_blks; i++) { 
				memset(buffer,0,MAXLINE);
				recv(datasock, buffer, MAXLINE,0);
				fwrite(buffer,sizeof(char),MAXLINE,fp);
				//cout<<buffer<<endl;
			}
			recv(datasock, char_num_last_blk, MAXLINE,0);
			num_last_blk=atoi(char_num_last_blk);
			if (num_last_blk > 0) { 
				memset(buffer,0,MAXLINE);				
				recv(datasock, buffer, MAXLINE,0);
				fwrite(buffer,sizeof(char),num_last_blk,fp);
				//cout<<buffer<<endl;
			}
			}
			fclose(fp);
                       close(datasock);
   
   
   
   }
   else
   {
   //bad command from client....................
	char port[MAXLINE],buffer[MAXLINE],char_num_blks[MAXLINE],char_num_last_blk[MAXLINE];
	int datasock,datasock1,lSize,num_blks,num_last_blk,i;
	FILE *fp;	 
	 
	data_port=1080;
	fflush(stdin);      
	
	sprintf(port,"%d",data_port);
	datasock1=create_socket(data_port);				//creating socket for data connection
	send(connfd, port,MAXLINE,0);					//sending port no. to client
	datasock=accept_conn(datasock1);	
        close(datasock1);
        fflush(stdin);
        char hello1[]="HTTP/1.1 405 Bad Method from client";
        send(datasock,hello1,MAXLINE,0);
        fflush(stdin);
        close(datasock);
   }
   nextoption:
cout<<"\n>>";
  }



  exit(0);
 }
 //close socket of the server
 close(connfd);
}
}

int create_socket(int port)
{ int opt=1;
int listenfd;
struct sockaddr_in dataservaddr;


//Create a socket for the soclet
//If sockfd<0 there was an error in the creation of the socket
if ((listenfd = socket (AF_INET, SOCK_STREAM, 0)) <0) {
cerr<<"Problem in creating the data socket"<<endl;
exit(2);
}

if(setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR | SO_REUSEPORT,&opt,sizeof(opt)))
   { cerr<<"setsockopt error";
     exit(2);
    }
//preparation of the socket address
memset(&dataservaddr, 0, sizeof(dataservaddr));
dataservaddr.sin_family = AF_INET;
dataservaddr.sin_addr.s_addr = htonl(INADDR_ANY);
dataservaddr.sin_port = htons(port);

if ((bind (listenfd, (struct sockaddr *) &dataservaddr, sizeof(dataservaddr))) <0) {
cerr<<"Problem in binding the data socket"<<endl;
exit(2);
}

 //listen to the socket by creating a connection queue, then wait for clients
 listen (listenfd, 1);

return(listenfd);
}

int accept_conn(int sock)
{
int dataconnfd;
socklen_t dataclilen;
struct sockaddr_in datacliaddr;

dataclilen = sizeof(datacliaddr);
  //accept a connection
if ((dataconnfd = accept (sock, (struct sockaddr *) &datacliaddr, &dataclilen)) <0) {
cerr<<"Problem in accepting the data socket"<<endl;
exit(2);
}

return(dataconnfd);
}

