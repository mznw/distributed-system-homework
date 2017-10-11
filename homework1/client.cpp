#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include <bits/stdc++.h>

using namespace std;

#define xx cout<<"client :" 

int main(int argc, char **argv)
{
  int option;
  int port = 9999;
  string host = "localhost";
  if(argc != 4) {
    xx << "Usage: client [-i IP address] [-p port] [word]" << endl;
  }

  while ((option = getopt(argc,argv,"i:p:")) != -1) {
    switch (option) {
      case 'p':
        port = atoi(optarg);
        break;
      case 'i':
        host = optarg;
        break;
      default:
        xx << "client [-i IP address] [-p port] [word]" << endl;
        return 0;
    }
  }
  struct hostent *hostEntry;
  hostEntry = gethostbyname(host.c_str());
  if (!hostEntry) {
    xx << "No such host name: " << host << endl;
    exit(-1);
  }
  xx <<"host : " <<  host << endl;
  xx <<"port : " <<  port << endl;
  struct sockaddr_in server_addr;
  memset(&server_addr,0,sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  memcpy(&server_addr.sin_addr, hostEntry->h_addr_list[0], hostEntry->h_length);

  int server = socket(PF_INET,SOCK_STREAM,0);
  if (server < 0) {
    xx << "socket error" << endl;
    return 0;
  }

  if (connect(server,(const struct sockaddr *)&server_addr,sizeof(server_addr)) < 0) {
    xx << "connect error" << endl;
    return 0;
  }

  int buflen = 1024;
  char* buf = new char[buflen+1];

  string line = argv[argc-1];;
  xx << "search : " << line << endl;
  xx << "length : " << line.length() << endl;
  send(server, line.c_str(), line.length(), 0);

  memset(buf,0,buflen);
  recv(server,buf,buflen,0);
  xx << "The result of searching word " << line << " is : " ;
  cout << buf << endl;
  close(server);
  return 0;
}

