#include <iostream>
#include <fstream>
#include <winsock2.h>
#include <unistd.h>
#include <cstring>
#include <string>

class SimpleHttpServer {
public:
    SimpleHttpServer(int port, int backlog = 5) : PORT(port), BACKLOG(backlog) {
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            throw std::runtime_error("WSAStartup failed");
        }
    }

    ~SimpleHttpServer() {
        close(server_fd);
        WSACleanup();
    }

    void start() {
        setupSocket();
        bindSocket();
        listenSocket();
        acceptClients();
    }

private:
    WSADATA wsaData;
    const int PORT;
    const int BACKLOG;
    int server_fd;

    void setupSocket() {
        server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd < 0) {
            throw std::runtime_error("Socket creation failed");
        }

        int opt = 1;
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) < 0) {
            throw std::runtime_error("setsockopt failed");
        }
    }

    void bindSocket() {
        struct sockaddr_in address;
        memset(&address, 0, sizeof(address));
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(PORT);

        if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) != 0) {
            throw std::runtime_error("Bind failed");
        }
    }

    void listenSocket() {
        if (listen(server_fd, BACKLOG) != 0) {
            throw std::runtime_error("Listen failed");
        }
        std::cout << "Server is listening on port " << PORT << "...\n";
    }

    void acceptClients() {
        while (true) {
            int client_socket = accept(server_fd, nullptr, nullptr);
            if (client_socket < 0) {
                std::cerr << "Accept failed\n";
                continue;
            }
            handleClient(client_socket);
        }
    }

    void handleClient(int client_socket) {
        char buffer[1024] = {0};
        recv(client_socket, buffer, sizeof(buffer), 0);
        std::string request(buffer);
        std::string response = generateHttpResponse(handleRequest(request));
        send(client_socket, response.c_str(), response.size(), 0);
        close(client_socket);
    }

    std::string handleRequest(const std::string& request) {
        std::string firstLine = request.substr(0, request.find("\r\n"));
        size_t start = firstLine.find(' ') + 1;
        size_t end = firstLine.find(' ', start);
        std::string path = firstLine.substr(start, end - start);

        if (path == "/" || path.empty()) {
            return readHtmlFile("mainPage.html");
        }
        if (path == "/ArticlesPages/T-shirt.html") return readHtmlFile("ArticlesPages/T-shirt.html");
        if (path == "/ArticlesPages/Cap.html") return readHtmlFile("ArticlesPages/Cap.html");
        if (path == "/ArticlesPages/Gloves.html") return readHtmlFile("ArticlesPages/Gloves.html");
        if (path == "/ArticlesPages/Polo.html") return readHtmlFile("ArticlesPages/Polo.html");
        if (path == "/ArticlesPages/Scarf.html") return readHtmlFile("ArticlesPages/Scarf.html");
        if (path == "/ArticlesPages/Socks.html") return readHtmlFile("ArticlesPages/Socks.html");
        if (path == "/ArticlesPages/Sunglasses.html") return readHtmlFile("ArticlesPages/Sunglasses.html");
        if (path == "/ArticlesPages/Sweater.html") return readHtmlFile("ArticlesPages/Sweater.html");
        return "<html><body><h1>404 Not Found</h1><p>Page not found.</p></body></html>";
    }

    std::string readHtmlFile(const std::string& filePath) {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            return "<html><body><h1>404 Not Found</h1><p>File not found.</p></body></html>";
        }
        return {std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
    }

    std::string generateHttpResponse(const std::string& content) {
        return "HTTP/1.1 200 OK\r\n"
               "Content-Type: text/html; charset=UTF-8\r\n"
               "Content-Length: " + std::to_string(content.size()) + "\r\n" +
               "\r\n"  + content;
    }
};

int main() {
    try {
        SimpleHttpServer serverC(8080);
        serverC.start();
    } catch (const std::exception& ex) {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    return 0;
}
