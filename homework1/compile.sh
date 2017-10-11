rm server
rm client
g++ server.cpp -o server -std=c++11 -pthread 
g++ client.cpp -o client -std=c++11

