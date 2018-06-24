#include <iostream>
#include <sstream>
#include "ace/INET_Addr.h"
#include "ace/SOCK_Stream.h"
#include "ace/SOCK_Acceptor.h"
using namespace std;


int main(int argc,char** argv)
{
   int portNumber = 11111;
 // char* address = "127.0.1.1";  
  ACE_INET_Addr serverAddress;
  //ACE_SOCK_Stream peer;	
  ACE_SOCK_Acceptor acceptor;

 if(argc > 1)
    {
	cout << "Error on Server - String found ";
	cout << argv[1] << endl;
	return -1;
    }

 if (serverAddress.set (portNumber) == -1) 
	{
		cerr << "addr set fails" << endl;
		return -1;
	}
 if(acceptor.open(serverAddress) == -1)
	{
		cerr << "Acceptor open fails" << endl;
		return -1;
	}
int i = 0;
int client_count = 0;
while(true)
{
  ACE_SOCK_Stream peer;	
  int idzero = 0;
  stringstream out;	
  //cout << "Starting loop " << ++i << endl;

   if(acceptor.accept(peer) == -1)
	{
		cerr << "Acceptor accept fails" << endl;
		return -1;
	}

  char buffer[50];
  //cout<<"Next client call accepted"<<endl;

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
	}
  if(idzero == 1)
	if(peer.send_n(out.str().c_str(), 10) < 0)
		return -1;
  peer.close();
 //cout << "ending loop " << i << endl;

}
cout << "Server reached outside if" << endl;
return 0;

}
