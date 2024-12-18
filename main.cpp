#include <iostream>        // Для вывода в консоль
#include <fstream>         // Для работы с файлами
#include <winsock2.h> 
#include <unistd.h>        // Для функции close()
#include <cstring>         // Для memset и strlen




std::string readHtmlFile(const std::string &filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return "<html><body><h1>404 Not Found</h1><p>File not found.</p></body></html>";
    }

    std::string content((std::istreambuf_iterator<char>(file)), //считываем файл побайтово в одну строку content
                         std::istreambuf_iterator<char>()); //заканчиваем
    file.close();
    return content;
}

std::string handleRequest(const std::string &request) {
    // Получение первой строки запроса (метод, путь, HTTP-версия)
    std::string firstLine = request.substr(0, request.find("\r\n"));
    std::cout << "Request line: " << firstLine << std::endl;

    // Извлечение пути из запроса
    size_t start = firstLine.find(' ') + 1;
    size_t end = firstLine.find(' ', start);
    std::string path = firstLine.substr(start, end - start);
    std :: cout << "Path:" << path << std ::endl;
    // Если путь пустой или "/", возвращаем главную страницу
    if (path == "/" || path.empty()) {
        return readHtmlFile("mainPage.html");
    } 

    // Обработка других страниц
    else if (path == "/ArticlesPages/T-shirt.html") {
        return readHtmlFile("ArticlesPages/T-shirt.html");
    }
    else if (path == "/ArticlesPages/Cap.html") {
        return readHtmlFile("ArticlesPages/Cap.html");
    }
    else if (path == "/ArticlesPages/Gloves.html") {
        return readHtmlFile("ArticlesPages/Gloves.html");
    }
    else if (path == "/ArticlesPages/Polo.html") {
        return readHtmlFile("ArticlesPages/Polo.html");
    }
    else if (path == "/ArticlesPages/Scarf.html") {
        return readHtmlFile("ArticlesPages/Scarf.html");
    }
    else if (path == "/ArticlesPages/Socks.html") {
        return readHtmlFile("ArticlesPages/Socks.html");
    }
    else if (path == "/ArticlesPages/Sunglasses.html") {
        return readHtmlFile("ArticlesPages/Sunglasses.html");
    }
    else if (path == "/ArticlesPages/Sweater.html") {
        return readHtmlFile("ArticlesPages/Sweater.html");
    }else {
        // Если страница не найдена
        return "<html><body><h1>404 Not Found</h1><p>Page not found.</p></body></html>";
    }
}

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed" << std::endl;
        return 1;
    }
    const int PORT = 8080;
    const int BACKLOG = 5; //максимум подключений, если пытаются подключиться ещё, отправляют ошибку 

    int server_fd = socket(AF_INET, SOCK_STREAM, 0); //определили тип IP и TPS для сокета


    // Структура sockaddr_in
    //struct sockaddr_in {
    //sa_family_t    sin_family; тип IP
    //in_port_t      sin_port;   порт
    //struct in_addr sin_addr; структура IP
    //char           sin_zero[8]; нули
    //};


    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    //с помощью bind привязываем сокет к локальному IP адресу и порту
	//посылаем в аргументы bind(): 1)0, если сервер создался; 2) по сути указатель на IP и TPS; 3) размер структуры в байтах
	// bind выдаёт 0, если всё ок и -1 если не ок
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) != 0) { 
        perror("Bind failed");
        close(server_fd);
        return 1;
    }
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        close(server_fd);
        return 1;
    }
 

    // listen прослушивает запрос браузера и выдаёт 0, если сокет создан и количество подключений не превышает нормы
    if (listen(server_fd, BACKLOG) != 0) {
        perror("Listen failed");
        close(server_fd);
        return 1;
    }

    std::cout << "Server is listening on port " << PORT << "...\n";

    while (true) {
        // 5. принятие подключения
        int client_socket = accept(server_fd, nullptr, nullptr);
        if (client_socket < 0) {
            std :: cout << "Accept failed";
            continue;
        }


        // 6. чтение запроса
        char buffer[1024] = {0};
        recv(client_socket, buffer, sizeof(buffer), 0);
        std::cout << "Received request:\n" << buffer << "\n";

        // 7. обработка запроса
        std::string htmlContent = handleRequest(std:: string(buffer));
        std::cout << "HTML Content:\n" << htmlContent << std::endl;

        // 8. http-ответ
        std::string httpResponse = 
           "HTTP/1.1 200 OK\r\n"
           "Content-Type: text/html; charset=UTF-8\r\n"
           "Content-Length: " + std::to_string(htmlContent.size()) + "\r\n"  // Заголовок с длиной содержимого
           "\r\n" + htmlContent +
           "\r\n";

        // 9. отправка ответа клиенту
        send(client_socket, httpResponse.c_str(), httpResponse.size(), 0);
        std::cout << "HTTP Response Sent:\n" << httpResponse << std::endl;


        // 10. закрыть клиентский сокет
        close(client_socket);
    }

    // 11. закрыть серверный сокет
    close(server_fd);
    return 0;
}
// g++ -o webserver main.cpp -lwsock32 - запуск
//g++ main.cpp -o server -lws2_32 - тоже запуск