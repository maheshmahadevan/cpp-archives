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

class Client:public ACE_Event_Handler{

   private:
	int portNumber;
	char* buffer;
	int id ;
	ACE_INET_Addr serverAddress;
  	ACE_SOCK_Connector connector; 
   public:
	Client(char*);
	~Client();
	virtual int handle_timeout (const ACE_Time_Value &, const void*);
	ACE_INET_Addr  &getServerAddress() { return serverAddress;}
	ACE_SOCK_Connector &getConnector() { return connector;}
	int getPortNumber();
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


int Client::getPortNumber()
{
	return portNumber;
}

int Client::handle_timeout (const ACE_Time_Value &, const void*)
{
	//cout<<"In handle timeout"<<endl;
	ACE_SOCK_Stream peer;	//tmp peer
	if(getServerAddress().set(getPortNumber()) == -1)
	{
		cout << "Error setting the server address" <<endl;	
		return -1;
	
	}
   	if(getConnector().connect(peer,getServerAddress()) == -1)
	{
		cout << "Error Connecting to the server address" <<endl;	
		return -1;
	
	}
        //cout<<buffer<<endl;

	stringstream out;
	out<< getId();
	string buf = "id:";
	buf.append(out.str());
	buf.append(":data:");
	buf.append(buffer);	

	if(peer.send_n(buf.c_str(), 50) > 0)
	{		
		//cout<<"sent data"<<endl;
		
	}
	else
		return -1;
	
	char newid[10];
	if (id ==0 && peer.recv(newid, sizeof buffer) > 0)
	{
		id = atoi(newid);
	}
	return 0;

}

int main(int argc,char** argv)
{

  const ACE_Time_Value timeout(3,0); //3 secs 
  ACE_Thread_Timer_Queue_Adapter<ACE_Timer_Heap> tqa;
  tqa.activate();

  ACE_Time_Value timeOfDay = ACE_OS::gettimeofday();

  
  	
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
   c.handle_timeout(timeout,(const void*)1);
   tqa.schedule(&c,(const void*)0,timeOfDay+timeout,timeout);
   ACE_Thread_Manager::instance()->wait();

		

cout << "Client reached outside if" << endl;
return 0;

}
