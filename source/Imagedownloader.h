/*
// Imagedownloader.h
// 
// Description:
//
// This file enables evaluating files in the network (e.g. on the web) by referencing them using URL 
// without explicitly downloading them. Note that these functions use sockets which are differently 
// handled in windows and UNIX OS. This is why some code parts are seperated by compiler switches
//
*/

#ifndef _IMAGEDOWNLOADER
#define _IMAGEDOWNLOADER

#include <iostream> 
#include <fstream> 
#include <stdexcept> 
#include <sstream> 

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__)
#include <winsock2.h>
#define _WINOS
#else
#include <sys/types.h>
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netdb.h> 
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <errno.h> 
#endif 

std::runtime_error CreateSocketError()
{
    std::ostringstream temp;
#ifndef _WINOS
    temp << "Socket-Fehler #" << errno << ": " << strerror(errno);
#else
    int error = WSAGetLastError();
    temp << "Socket-Fehler #" << error;
    char* msg;
    if(FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                     NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                     reinterpret_cast<char*>(&msg), 0, NULL))
    {
        try
        {
            temp << ": " << msg;
            LocalFree(msg);
        }
        catch(...)
        {
            LocalFree(msg);
            throw;
        }
    }
#endif
    return std::runtime_error(temp.str());
}

void SendAll(int socket, const char* const buf, const int size)
{
    int bytesSent = 0; 
    do
    {
        int result = send(socket, buf + bytesSent, size - bytesSent, 0);
        if(result < 0) 
        {
            throw CreateSocketError();
        }
        bytesSent += result;
    } while(bytesSent < size);
}

std::string globalBuffer; 


void GetLine(int socket, std::stringstream& line)
{
    char buf[1024];
    int recvSize;
    std::string::size_type pos;
    while((pos = globalBuffer.find('\n')) == std::string::npos)
    {
        if((recvSize = recv(socket, buf, sizeof(buf), 0)) <= 0)
        {
            throw CreateSocketError();
        }
        globalBuffer.append(buf, recvSize);
    }
    line << globalBuffer.substr(0, pos);
    globalBuffer.erase(0, pos + 1);
}

int Recv(int socket, char *buf, int len, unsigned int flags) 
{
    if(!globalBuffer.empty())
    {
        int copySize = len <= globalBuffer.size() ? len : globalBuffer.size();
        globalBuffer.copy(reinterpret_cast<char*>(buf), copySize);
        globalBuffer.erase(0, copySize);
        return copySize;
    }
    else
    {
        return recv(socket, buf, len, flags);
    }
}


void RemoveHttp(std::string& URL)
{
    size_t pos = URL.find("http://");
    if(pos != std::string::npos)
    {
        URL.erase(0, 7);
    }
}


std::string GetFileEnding(std::string& URL)
{
    using namespace std;
    size_t pos = URL.rfind(".");
    if(pos == string::npos)
    {
        return "";
    }
    URL.erase(0, pos);
    string ending = ".";

    for(string::iterator it = URL.begin() + 1; it != URL.end(); ++it)
    {
        if(isalpha(*it))
        {
            ending += *it;
        }
        else
        {
            break;
        }
    }
    return ending;
}


std::string RemoveHostname(std::string& URL)
{
    size_t pos = URL.find("/");
    if(pos == std::string::npos)
    {
        std::string temp = URL;
        URL = "/";
        return temp;
    }
    std::string temp = URL.substr(0, pos);
    URL.erase(0, pos);
    return temp;
}

std::string download_image(const char* url, std::string filename)
{
    using namespace std;

 
	string URL = url;
    
    

#ifdef _WINOS
    WSADATA w;
    if(int result = WSAStartup(MAKEWORD(2,2), &w) != 0)
    {
        cout << "Winsock 2 can not be started, error #" << result << endl;
        return "";
    }
#endif

    RemoveHttp(URL);

    string hostname = RemoveHostname(URL);

    hostent* phe = gethostbyname(hostname.c_str());

    if(phe == NULL)
    {
        cout << "Host can not be resolved!" << endl;
        return "";
    }

    if(phe->h_addrtype != AF_INET)
    {
        cout << "invalid address type!" << endl;
        return "";
    }

    if(phe->h_length != 4)
    {
        cout << "invalid IP-Type!" << endl;
        return "";
    }

    int Socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(Socket == -1)
    {
        cout << "Socket cannot be created!" << endl;
        return "";
    }

    sockaddr_in service;
    service.sin_family = AF_INET;
    service.sin_port = htons(80); 

    char** p = phe->h_addr_list; 
    int result; 
    do
    {
        if(*p == NULL) 
        {
            cout << "connection not successful!" << endl;
            return "";
        }

        service.sin_addr.s_addr = *reinterpret_cast<unsigned long*>(*p);
        ++p;
        result = connect(Socket, reinterpret_cast<sockaddr*>(&service), sizeof(service));
    }
    while(result == -1);


    string request = "GET ";
    request += URL;    // 
    request += " HTTP/1.1\n";
    request += "Host: " + hostname + "\nConnection: close\n\n";

    try
    {
        SendAll(Socket, request.c_str(), request.size());
        int code = 100; 
        string Protokoll;
        stringstream firstLine; 
        while(code == 100)
        {
            GetLine(Socket, firstLine);
            firstLine >> Protokoll;
            firstLine >> code;
            if(code == 100)
            {
                GetLine(Socket, firstLine); 
            }
        }
        

        if(code != 200)
        {
            firstLine.ignore(); 
            string msg;
            getline(firstLine, msg);
            cout << "Error #" << code << " - " << msg << endl;
            return "";
        }

        bool chunked = false;
        const int noSizeGiven = -1;
        int size = noSizeGiven;

        while(true)
        {
            stringstream sstream;
            GetLine(Socket, sstream);
            if(sstream.str() == "\r") 
            {
                break;
            }
            string left; 
            sstream >> left;
            sstream.ignore(); 
            if(left == "Content-Length:")
            {
                sstream >> size;
            }
            if(left == "Transfer-Encoding:")
            {
                string transferEncoding;
                sstream >> transferEncoding;
                if(transferEncoding == "chunked")
                {
                    chunked = true;
                }
            }
        }

		int ind = URL.find("nii.gz");
   if( ind != string::npos){
      filename = filename + ".gz";
   }

        fstream fout(filename.c_str(), ios::binary | ios::out);
        if(!fout)
        {
            cout << "Could Not Create File!" << endl;
            return "";
        }
        int recvSize = 0; 
        char buf[1024];
        int bytesRecv = -1; 

        if(size != noSizeGiven) 
        {
           // cout << "0%";
            while(recvSize < size)
            {
                if((bytesRecv = Recv(Socket, buf, sizeof(buf), 0)) <= 0)
                {
                    throw CreateSocketError();
                }
                recvSize += bytesRecv;
                fout.write(buf, bytesRecv);
               // cout << "\r" << recvSize * 100 / size << "%" << flush; 
            }
        }
        else
        {
            if(!chunked)
            {
                cout << "Downloading... (Unknown Filesize)" << endl;
                while(bytesRecv != 0) 
                {
                    if((bytesRecv = Recv(Socket, buf, sizeof(buf), 0)) < 0)
                    {
                        throw CreateSocketError();
                    }
                    fout.write(buf, bytesRecv);
                }
            }
            else
            {
                cout << "Downloading... (Chunked)" << endl;
                while(true)
                {
                    stringstream sstream;
                    GetLine(Socket, sstream);
                    int chunkSize = -1;
                    sstream >> hex >> chunkSize; 
                    if(chunkSize <= 0)
                    {
                        break;
                    }
                    cout << "Downloading Part (" << chunkSize << " Bytes)... " << endl;
                    recvSize = 0; 
                    while(recvSize < chunkSize)
                    {
                        int bytesToRecv = chunkSize - recvSize;
                        if((bytesRecv = Recv(Socket, buf, bytesToRecv > sizeof(buf) ? sizeof(buf) : bytesToRecv, 0)) <= 0)
                        {
                            throw CreateSocketError();
                        }
                        recvSize += bytesRecv;
                        fout.write(buf, bytesRecv);
                        cout << "\r" << recvSize * 100 / chunkSize << "%" << flush;
                    }
                    cout << endl;
                    for(int i = 0; i < 2; ++i)
                    {
                        char temp;
                        recv(Socket, &temp, 1, 0);
                    }
                }
            }
        }
        
    }
    catch(exception& e)
    {
        cout << endl;
        cerr << e.what() << endl;
    }
#ifndef  _WINOS
    close(Socket); 
#else
    closesocket(Socket); 
#endif
	return filename;
}

#endif