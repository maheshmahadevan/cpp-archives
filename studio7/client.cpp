#include <iostream>
#include <sstream>
#include "ace/INET_Addr.h"
#include "ace/SOCK_Connector.h"
#include "ace/SOCK_Stream.h"
#include "ace/Reactor.h"
#include "ace/Timer_Queue.h" 
#include "ace/Timer_Queue_Adapters.h"
#include "ace/Timer_Heap.h"
using namespace std;

class Client;
class RWEventHandler;

 ACE_Time_Value timeOfDay = ACE_OS::gettimeofday();

class RWEventHandler:public ACE_Event_Handler
{
	private:
		ACE_SOCK_Stream peer;
		char* buffer;
		int id ;
		Client* c;	

	public:
		RWEventHandler();
		~RWEventHandler();
		virtual int handle_output (ACE_HANDLE);
		int getId() { return id;}
		void setId(int id) { this->id = id; }
		ACE_SOCK_Stream &getPeer() { return peer;}
		void setPeer(ACE_SOCK_Stream p) { this->peer = p; }
		void setBuffer(char* b) { this->buffer = b;}
		ACE_HANDLE get_handle(void) const{return this->peer.get_handle();}
		void setClient(Client* cl ) {this->c = cl;}
		Client* getClient() {return c;}
};


RWEventHandler::RWEventHandler() 
{
	id =0;
}

RWEventHandler::~RWEventHandler()
{

	
}

class Client:public ACE_Event_Handler{

   private:
	int portNumber;
	char* buffer;
	int id ;
	ACE_INET_Addr serverAddress;
  	ACE_SOCK_Connector connector; 
	ACE_SOCK_Stream peer;	//tmp peer
	RWEventHandler rweh;
   public:
	Client(char*);
	~Client();
	virtual int handle_timeout (const ACE_Time_Value &, const void*);
	int connect();
	ACE_INET_Addr  &getServerAddress() { return serverAddress;}
	ACE_SOCK_Connector &getConnector() { return connector;}
	ACE_SOCK_Stream &getPeer() { return peer;}
	void setPeer(ACE_SOCK_Stream p) { this->peer = p; }
	int getPortNumber() {return portNumber;}
	int getId() { return id;}
	void setId(int id) { this->id = id; }



};

Client::Client(char* buffer)
{
	portNumber = 11111;
	id = 0;
	this->buffer = buffer;
	       
}

Client::~Client()
{

	
}

int Client::handle_timeout (const ACE_Time_Value &, const void*)
{
	cout<<"In handle timeout"<<endl;
	connect();
	return 0;
}



int Client::connect()
{
	ACE_SOCK_Stream peer;	//tmp peer
	cout<<"In connect"<<endl;
	if(getServerAddress().set(getPortNumber()) == -1)
	{
		cout << "Error setting the server address" <<endl;	
		return -1;
	
	}
	setPeer(peer);
   	if(getConnector().connect(peer,getServerAddress()) == -1)
	{
		cout << "Error Connecting to the server address" <<endl;	
		return -1;
	
	}
	this->rweh.setPeer(peer);
	this->rweh.setBuffer(buffer);
	this->rweh.setClient(this);
   	ACE_Reactor::instance()->register_handler(&rweh,ACE_Event_Handler::WRITE_MASK);
	//ACE_Reactor::instance()->run_reactor_event_loop();
	
}


int RWEventHandler::handle_output(ACE_HANDLE h)
{ 
	stringstream out;
	out<< getClient()->getId();
	string buf = "id:";
	buf.append(out.str());
	buf.append(":data:");
	buf.append(buffer);	
	cout<<"Sending data"<<endl;
	if(peer.send_n(buf.c_str(), 50) < 0)
	{		
		return -1;
		
	}
	char newid[50];
	cout<<"Receving"<<endl;
	if (getClient()->getId()==0 && peer.recv(newid, sizeof newid) > 0)
	{
		id = atoi(newid);
		//cout << newid << endl;
		getClient()->setId(id);		
	}
	
	return -1;// deregister as work for this handler is done
}


int main(int argc,char** argv)
{

  const ACE_Time_Value timeout(3,0); //3 secs 
  ACE_Thread_Timer_Queue_Adapter<ACE_Timer_Heap> tqa;
  tqa.activate();

   
  	
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
 
   
  ACE_Reactor::instance()->schedule_timer(&c,(const void*)0,timeout,timeout);
   
	
  ACE_Reactor::instance()->run_reactor_event_loop();	
   	

		

cout << "Client reached outside if" << endl;
return 0;

}
