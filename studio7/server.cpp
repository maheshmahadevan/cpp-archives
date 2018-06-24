#include <iostream>
#include <sstream>
#include "ace/INET_Addr.h"
#include "ace/SOCK_Stream.h"
#include "ace/SOCK_Acceptor.h"
#include "ace/Reactor.h"
#include "ace/Mutex.h"
using namespace std;


int client_count = 0;

class Server_Acceptor;
class Server_Svc_Handler;

class Server_Svc_Handler:public ACE_Event_Handler
{
	private:
		ACE_SOCK_Stream peer;

	public:
		Server_Svc_Handler();
		~Server_Svc_Handler();
		ACE_SOCK_Stream &getPeer() { return peer;}
		void setPeer(ACE_SOCK_Stream p) { this->peer = p; }
		virtual int handle_input(ACE_HANDLE h = ACE_INVALID_HANDLE);
		ACE_HANDLE get_handle(void) const{return this->peer.get_handle();}

};

Server_Svc_Handler::Server_Svc_Handler()
{

}

Server_Svc_Handler::~Server_Svc_Handler()
{

}



int Server_Svc_Handler::handle_input(ACE_HANDLE h)
{
	char buffer[50];
	cout<<"In handle input"<<endl;
	int idzero = 0;
	if (peer.recv(buffer, sizeof buffer) > 0)
	{		
		cout<<buffer<<endl;
		char* token = strtok(buffer,":");
		while(token != NULL)
		{
			//cout<<token<<endl;
			if(strcmp(token,"id") == 0)
			{
				char* id = strtok(NULL,":");
				//cout<< id << endl;
				
				if(strcmp(id,"0") == 0)
				{
					client_count++;
					idzero = 1;	
				}				
				
				//cout << out.str().c_str() << idzero<<endl;
				//if(peer.send_n(out.str().c_str(), 10) > 0)
					//return 0;
				cout << "Client id :" << id << endl;
			}
			
			if(strcmp(token,"data") == 0)
			{
				char* data = strtok(NULL,":");
				cout << data << endl;

			}
			
			token = strtok(NULL,":");
		}
		//return 0;
		
	}
  else
	{
		cout << "Receive Fail" << endl;
		return -1;	
	}
//ACE_Reactor::instance()->register_handler(this,ACE_Event_Handler::WRITE_MASK);
	cout<<"In handle output"<<endl;
	stringstream out;
	out << client_count;
	if(idzero == 1)
	   if(peer.send_n(out.str().c_str(), 10) < 0)
		return -1;
	//peer.close(); // if you do this you will break the pipe and reactor in client will shut down instead return -1 to deregister reactor from both //client and server
return -1;

}


class Server_Acceptor:public ACE_Event_Handler{
	private:
		ACE_INET_Addr serverAddress;
		int portNumber;
		ACE_SOCK_Acceptor acceptor;
		ACE_SOCK_Stream peer;	
	public:
		Server_Acceptor();
		~Server_Acceptor();
		virtual int handle_input(ACE_HANDLE h = ACE_INVALID_HANDLE);
		ACE_HANDLE get_handle(void) const{return this->acceptor.get_handle();}
		ACE_SOCK_Stream &getPeer() { return peer;}
		void setPeer(ACE_SOCK_Stream p) { this->peer = p; }
  

};

Server_Acceptor::Server_Acceptor()
{
	portNumber = 11111;
	if (serverAddress.set (portNumber) == -1) 
	{
		cerr << "addr set fails" << endl;
	}
	if(acceptor.open(serverAddress) == -1)
	{
		cerr << "Acceptor open fails" << endl;
	}
	 
}
Server_Acceptor::~Server_Acceptor()
{

}

int Server_Acceptor::handle_input(ACE_HANDLE h)
{
	cout<<"In handle input of Server Acceptor"<<endl;
	Server_Svc_Handler *sh = new Server_Svc_Handler(); 
	if(acceptor.accept(sh->getPeer()) == -1)
	{
		cerr << "Acceptor accept fails" << endl;
		return -1;
	}
	ACE_Reactor::instance()->register_handler(sh,ACE_Event_Handler::READ_MASK);
	
	return 0;
}



int main(int argc,char** argv)
{
  // int portNumber = 11111;
 // char* address = "127.0.1.1";  
  //ACE_INET_Addr serverAddress;
  //ACE_SOCK_Stream peer;	
  //ACE_SOCK_Acceptor acceptor;

 if(argc > 1)
    {
	cout << "Error on Server - String found ";
	cout << argv[1] << endl;
	return -1;
    }
/*
 if (serverAddress.set (portNumber) == -1) 
	{
		cerr << "addr set fails" << endl;
		return -1;
	}
 if(acceptor.open(serverAddress) == -1)
	{
		cerr << "Acceptor open fails" << endl;
		return -1;
	}*/
Server_Acceptor *sa = new Server_Acceptor();
ACE_Reactor::instance()->register_handler(sa,ACE_Event_Handler::ACCEPT_MASK); 
//int i = 0;
//int client_count = 0;
while(true)
{
  //ACE_SOCK_Stream peer;	
  //int idzero = 0;
  //stringstream out;	
  //cout << "Starting loop " << ++i << endl;
/*
   if(acceptor.accept(peer) == -1)
	{
		cerr << "Acceptor accept fails" << endl;
		return -1;
	}*/
  
  ACE_Reactor::instance()->handle_events();
 // peer = sa->getPeer();
 // char buffer[50];
  //cout<<"Next client call accepted"<<endl;
/*
  if (peer.recv(buffer, sizeof buffer) > 0)
	{		
		cout<<buffer<<endl;
		char* token = strtok(buffer,":");
		while(token != NULL)
		{
			//cout<<token<<endl;
			if(strcmp(token,"id") == 0)
			{
				char* id = strtok(NULL,":");
				//cout<< id << endl;
				
				if(strcmp(id,"0") == 0)
				{
					client_count++;
					idzero = 1;	
				}				
				out << client_count;
				//cout << out.str().c_str() << idzero<<endl;
				//if(peer.send_n(out.str().c_str(), 10) > 0)
					//return 0;
				cout << "Client id :" << id << endl;
			}
			
			if(strcmp(token,"data") == 0)
			{
				char* data = strtok(NULL,":");
				cout << data << endl;

			}
			
			token = strtok(NULL,":");
		}
		
	}
  else
	{
		cout << "Receive Fail" << endl;
		return -1;	
	}*/
 /* if(idzero == 1)
	if(peer.send_n(out.str().c_str(), 10) < 0)
		return -1;
  peer.close();*/
 //cout << "ending loop " << i << endl;

}
cout << "Server reached outside if" << endl;
return 0;

}
