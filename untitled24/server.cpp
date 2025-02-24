#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cstring>
#include <fstream>

#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#define closesocket close
#endif

#define PORT 8087
#define BUFFER_SIZE 4096

void initializeWinsock() {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed\n";
        exit(EXIT_FAILURE);
    }
#endif
}
/*
std::string generateHTML() {
    std::ifstream file("/home/anastasiia/CLionProjects/untitled24/test_MARVEL/front_end/index.html");
    if (!file.is_open()) {
        std::cerr << "Could not open the HTML file." << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}
*/

std::string generateHTML() {
    std::ostringstream html;
    html << "<!DOCTYPE html>";
    html << "<html lang=\"en\">";
    html << "<head>";
    html << "<meta charset=\"UTF-8\">";
    html << "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
    html << "<title>Simple C++ Web Page</title>";
    html << "</head>";
    html << "<body>";
    html << "<h1>this is a new web page!</h1>";
    html << "<p>This page is generated dynamically using C++.</p>";
    html << "<p>Server is running and responding to HTTP requests.</p>";
    html << "<ul>";

    html << "<li><a href=\"#\">Next page</a></li>";
    html << "<li><a href=\"#\">Location</a></li>";
    html << "<li><a href=\"#\">Contact</a></li>";
    html << "</ul>";

    html << "<p>Current timestamp: " << time(0) << "</p>";
    html << "</body>";
    html << "</html>";

    return html.str();
}







void startServer() {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Failed to create socket\n";
        return;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Bind failed\n";
        closesocket(serverSocket);
        return;
    }

    if (listen(serverSocket, 10) < 0) {
        std::cerr << "Listen failed\n";
        closesocket(serverSocket);
        return;
    }

    std::cout << "Server running on http://127.0.0.1:" << PORT << std::endl;

    while (true) {
        sockaddr_in clientAddr;
#ifdef _WIN32
        int clientSize = sizeof(clientAddr);
#else
        socklen_t clientSize = sizeof(clientAddr);
#endif
        int clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientSize);

        if (clientSocket < 0) {
            std::cerr << "Client connection failed\n";
            continue;
        }

        char buffer[BUFFER_SIZE] = { 0 };
        recv(clientSocket, buffer, BUFFER_SIZE, 0);
        std::cout << "Received request:\n" << buffer << std::endl;

        std::string html = generateHTML();
        std::ostringstream response;
        response << "HTTP/1.1 200 OK\r\n";
        response << "Content-Type: text/html\r\n";
        response << "Content-Length: " << html.length() << "\r\n";
        response << "Connection: close\r\n\r\n";
        response << html;

        send(clientSocket, response.str().c_str(), response.str().length(), 0);
        closesocket(clientSocket);
    }

    closesocket(serverSocket);
#ifdef _WIN32
    WSACleanup();
#endif
}

int main() {
    initializeWinsock();
    startServer();
    return 0;
}