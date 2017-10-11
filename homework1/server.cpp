#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include<bits/stdc++.h>
#include "thread_pool.h"

using namespace std;

#define xx cout<<"server : " 
struct dictionary {
  map<string, string> hs;
  void init(string filename) {
    ifstream fin(filename);
    string a,b;
    while(fin >> a >> b) {
      hs[a] = b;
      //cout << a << ' ' << b << endl;
    }
    xx << hs.size() << " words in " << filename << endl;
    return ;
  }
  int get_explaination(string a, string& b) {
    //cout << "find " << a << " length :" << a.length() << " in dictionary" << endl;
    if(hs.find(a) != hs.end()) {
      b = hs[a];
      return 1;
    } else {
      return 0;
    }
  }
}dic;

void handle(int client, string que) {
  //cout << "handle start ! " << endl;
  string ans = "no such word in dictionary !!";
  dic.get_explaination(que, ans);
  send(client, ans.c_str(), ans.length(), 0);
  close(client);
}

int main(int argc, char **argv)
{
  struct sockaddr_in server_addr,client_addr;
  socklen_t clientlen = sizeof(client_addr);
  int option, port, reuse;
  int server, client;
  char *buf;
  int buflen;
  int nread;

  port = 9999;
  if(argc != 2 || (argc == 3 && argv[2][0] == '&')) {
    cout << "Usage: server [port]" << endl;
    return 0;
  }
  port = atoi(argv[1]);

  memset(&server_addr,0,sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  server = socket(PF_INET,SOCK_STREAM,0);
  if (!server) {
    xx << "socket error!" << endl;
    return 0;
  }
  reuse = 1;
  if (setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
    xx << "socketopt error!" << endl;
    return 0;
  }
  if (bind(server,(const struct sockaddr *)&server_addr,sizeof(server_addr)) < 0) {
    xx << "bind error!" << endl;
    return 0;
  }
  if (listen(server,SOMAXCONN) < 0) {
    xx << "listen error!" << endl;
    return 0;
  }

  xx << "socket init done!" << endl;
  ThreadPool threadpool(20);
  dic.init("dictionary.in");
  buflen = 1024;
  buf = new char[buflen+1];
  threadpool.start();

  while ((client = accept(server,(struct sockaddr *)&client_addr,&clientlen)) > 0) {
    memset(buf,0,buflen);
    nread = recv(client,buf,buflen,0);
    xx <<"received word: " <<  buf << endl;
    if (nread == 0) {
      xx << "no words received " << endl;
      break;
    }
    string query(buf, nread);
    threadpool.add(bind(handle, client, query));
    //send(client, buf, nread, 0);
  }
  threadpool.stop();
  close(server);
  return 0;
}
