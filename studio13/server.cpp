#include <iostream>
#include <sstream>
#include "ace/INET_Addr.h"
#include "ace/SOCK_Stream.h"
#include "ace/SOCK_Acceptor.h"
#include "ace/Reactor.h"
#include "ace/Mutex.h"
#include "ace/Acceptor.h"
using namespace std;


int client_count = 0;

class Server_Acceptor;
class Server_Svc_Handler;

class Server_Svc_Handler:public ACE_Svc_Handler <ACE_SOCK_STREAM,ACE_NULL_SYNCH>
{
	private:
		

	public:
		Server_Svc_Handler();
		~Server_Svc_Handler();
		virtual int handle_input(ACE_HANDLE h = ACE_INVALID_HANDLE);
		

};

Server_Svc_Handler::Server_Svc_Handler()
{
	cout << "Svc Handler constructor is called " << this << endl;

}

Server_Svc_Handler::~Server_Svc_Handler()
{
	ACE_Reactor::instance()->remove_handler(this,ACE_Event_Handler::READ_MASK);
	cout << "Svc Handler destructor is called " << this << endl;
}



int Server_Svc_Handler::handle_input(ACE_HANDLE h)
{
	char buffer[50];
	cout<<"In handle input"<<endl;
	int idzero = 0;
	if (peer().recv(buffer, sizeof buffer) > 0)
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
	   if(peer().send_n(out.str().c_str(), 10) < 0)
		return -1;
	//peer.close(); // if you do this you will break the pipe and reactor in client will shut down instead return -1 to deregister reactor from both //client and server
return 0;

}


class Server_Acceptor:public ACE_Acceptor<Server_Svc_Handler,ACE_SOCK_ACCEPTOR>
{
	private:
		ACE_INET_Addr serverAddress;

		
	public:
		Server_Acceptor();
		~Server_Acceptor();
		virtual int handle_input(ACE_HANDLE h = ACE_INVALID_HANDLE);
		int handle_signal(int signum, siginfo_t*,ucontext_t*);

		
  

};

Server_Acceptor::Server_Acceptor()
{

	cout << "Acceptor constructor is called " << this << endl;
	ACE_INET_Addr addr(11111); 
	
	if(acceptor().open(addr) == -1)
	{
		cerr << "Acceptor open fails" << endl;
	}
	 
}
Server_Acceptor::~Server_Acceptor()
{
	cout << "Acceptor destructor is called " << this << endl;
	ACE_Reactor::instance()->remove_handler(this,ACE_Event_Handler::ACCEPT_MASK);
	close();
}

int Server_Acceptor::handle_input(ACE_HANDLE h)
{
	cout<<"In handle input of Server Acceptor"<<endl;
	Server_Svc_Handler *sh = new Server_Svc_Handler(); 
	if(acceptor().accept(sh->peer()) == -1)
	{
		sh->close();
		cerr << "Acceptor accept fails" << endl;
		return -1;
	}
	ACE_Reactor::instance()->register_handler(sh,ACE_Event_Handler::READ_MASK);
	
	return 0;
}

int Server_Acceptor::handle_signal(int signum, siginfo_t*,ucontext_t*)
{
	switch(signum)
	{ 
		case SIGINT: 
		{
			cout << " You pressed Ctrl C " << endl;
			ACE_Reactor::instance()->end_reactor_event_loop();
			break;	
		}
	}
	return 0;
}


int main(int argc,char** argv)
{
 

 if(argc > 1)
    {
	cout << "Error on Server - String found ";
	cout << argv[1] << endl;
	return -1;
    }

Server_Acceptor *sa = new Server_Acceptor();
ACE_Reactor::instance()->register_handler(sa,ACE_Event_Handler::ACCEPT_MASK); 
ACE_Reactor::instance()->register_handler(SIGINT,sa);
ACE_Reactor::instance()->run_reactor_event_loop();

if(sa)
  delete sa; 
cout << "Server reached outside if" << endl;
return 0;

}
