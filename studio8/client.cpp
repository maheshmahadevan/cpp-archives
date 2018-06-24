#include <iostream>
#include <sstream>
#include "ace/INET_Addr.h"
#include "ace/SOCK_Connector.h"
#include "ace/SOCK_Stream.h"
#include "ace/Reactor.h"
#include "ace/Timer_Queue.h" 
#include "ace/Timer_Queue_Adapters.h"
#include "ace/Timer_Heap.h"
#include "ace/Connector.h"
using namespace std;

class Client;
class RWEventHandler;

 ACE_Time_Value timeOfDay = ACE_OS::gettimeofday();

class RWEventHandler:public ACE_Svc_Handler <ACE_SOCK_STREAM,ACE_NULL_SYNCH>
{
	private:
		char* buffer;
		int id ;
		Client* c;	

	public:
		RWEventHandler();
		~RWEventHandler();
		virtual int handle_timeout (const ACE_Time_Value &, const void*);
		int getId() { return id;}
		void setId(int id) { this->id = id; }
		void setBuffer(char* b) { this->buffer = b;}
		void setClient(Client* cl ) {this->c = cl;}
		Client* getClient() {return c;}
		typedef ACE_Svc_Handler <ACE_SOCK_STREAM,ACE_NULL_SYNCH> base;
};


RWEventHandler::RWEventHandler() 
{
	id =0;
	cout << "Event handler constructor called " << this << endl;
}

RWEventHandler::~RWEventHandler()
{
	ACE_Reactor::instance()->cancel_timer(this);
	cout << "Event Handler destructor called" <<  this << endl;

	//close();	
	
}

class Client:public ACE_Connector<RWEventHandler,ACE_SOCK_CONNECTOR>{

   private:
	int portNumber;
	char* buffer;
	int id ;
	ACE_INET_Addr serverAddress;
  	ACE_SOCK_Connector connector; 
	ACE_SOCK_Stream peer;	//tmp peer
	
   public:
	Client(char*);
	~Client();
	int handle_signal(int signum, siginfo_t*,ucontext_t*);
	int makeConnection();
	int getPortNumber() {return portNumber;}
	int getId() { return id;}
	void setId(int id) { this->id = id; }
	typedef	ACE_Connector<RWEventHandler,ACE_SOCK_CONNECTOR> base;



};

Client::Client(char* buffer)
{
	portNumber = 11111;
	id = 0;
	this->buffer = buffer;
	cout << "Client constructor called" << this << endl;
	       
}

Client::~Client()
{
	//ACE_Reactor::instance()->remove_handler(this,ACE_Event_Handler::WRITE_MASK);
	cout << "Client destructor called " << this << endl;
	close();
	ACE_Reactor::instance()->end_reactor_event_loop();

	
}

int Client::makeConnection()
{
	const ACE_Time_Value timeout(3,0); //3 secs 
	 RWEventHandler *sh = new RWEventHandler();
	 ACE_INET_Addr addr(11111);

   	if(connect(sh,addr)==-1) 
	{
		cout<< "Connection failed" << endl;
		return -1;
	}
	
	sh->setClient(this);
	sh->setBuffer(buffer);
   	ACE_Reactor::instance()->schedule_timer(sh,(const void*)0,timeout,timeout);
	//ACE_Reactor::instance()->run_reactor_event_loop();


	return 0;
	
}


int RWEventHandler::handle_timeout (const ACE_Time_Value &, const void*)
{ 
	stringstream out;
	out<< getClient()->getId();
	string buf = "id:";
	buf.append(out.str());
	buf.append(":data:");
	buf.append(buffer);	
	cout<<"Sending data"<<endl;
	if(peer().send_n(buf.c_str(), 50) < 0)
	{		
		return -1;
		
	}
	char newid[50];
	cout<<"Receving"<<endl;
	if (getClient()->getId()==0 && peer().recv(newid, sizeof newid) > 0)
	{
		id = atoi(newid);
		//cout << newid << endl;
		getClient()->setId(id);		
	}
	
	return 0;
}

int Client::handle_signal(int signum, siginfo_t*,ucontext_t*)
{
	switch(signum)
	{ 
		case SIGINT: 
		{
			cout << " You pressed Ctrl C " << endl;
			//ACE_Reactor::instance()->cancel_timer(this);
			ACE_Reactor::instance()->end_reactor_event_loop();
			break;	
		}
	}
	return 0;

}

int main(int argc,char** argv)
{

  const ACE_Time_Value timeout(3,0); //3 secs 
 

   
  	
  if(argc == 1)
    {
	cout << "No command line arguments found." <<endl;
	return -1;
    }
   	
  char* buffer;
  int i = 1;
  string buf;
  while(i < argc )
  {
     buf+=argv[i];
     buf+=" ";
     i++;
   }
   buffer = &buf[0];
   cout << buffer << endl;
  Client c(buffer);
  c.makeConnection();
   
  //ACE_Reactor::instance()->schedule_timer(&c,(const void*)0,timeout,timeout);
  //part 1 
  ACE_Reactor::instance()->register_handler(SIGINT,&c);	
  ACE_Reactor::instance()->run_reactor_event_loop();	
   	

		

cout << "Client reached outside if" << endl;
return 0;

}
