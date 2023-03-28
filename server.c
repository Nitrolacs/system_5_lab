//
// Created by stud on 28.03.23.
//

#include "server.h"

// Серверная часть
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 5555
#define MAXBUF 1024

// Функция для решения квадратного уравнения и вывода разложения на множители
void solve_quadratic(double a, double b, double c) {
    double d = b * b - 4 * a * c; // дискриминант
    if (d < 0) {
        printf("Уравнение не имеет действительных корней.\n");
    } else if (d == 0) {
        double x = -b / (2 * a); // единственный корень
        printf("Уравнение имеет один действительный корень: x = %.2f\n", x);
        printf("Разложение на множители: (%.2f)x + %.2f = 0\n", a, b);
    } else {
        double x1 = (-b + sqrt(d)) / (2 * a); // первый корень
        double x2 = (-b - sqrt(d)) / (2 * a); // второй корень
        printf("Уравнение имеет два действительных корня: x1 = %.2f, x2 = %.2f\n",
               x1, x2);
        printf("Разложение на множители: (%.2f)x^2 + (%.2f)x + %.2f = (%.2f)(x - %.2f)(x - %.2f)\n",
               a, b, c, a, x1, x2);
    }
}

// Функция для решения кубического уравнения и вывода разложения на множители
void solve_cubic(double a, double b, double c, double d) {
    // Используем формулу Кардано
    double p = (3 * a * c - b * b) / (3 * a * a); // первый коэффициент
    double q = (2 * b * b * b - 9 * a * b * c + 27 * a * a * d) /
               (27 * a * a * a); // второй коэффициент
    double r = q * q / 4 + p * p * p / 27; // радикал

    if (r > 0) { // один действительный корень и два комплексных корня
        double s = sqrt(r); // квадратный корень из радикала
        double u = cbrt(-q / 2 + s); // первый кубический корень
        double v = cbrt(-q / 2 - s); // второй кубический корень
        double x = u + v; // действительный корень
        printf("Уравнение имеет один действительный корень: x = %.2f\n", x);
        printf("Разложение на множители: (%.2f)x^3 + (%.2f)x^2 + (%.2f)x + %.2f = (%.2f)(x - %.2f)\n",
               a, b, c, d, a, x);
        printf("Два комплексных корня не выводятся.\n");
    } else if (r == 0) { // три действительных корня, из которых два равны
        double u = cbrt(-q / 2); // кубический корень
        double x1 = 2 * u; // первый корень
        double x2 = -u; // второй и третий корень
        printf("Уравнение имеет три действительных корня: x1 = %.2f, x2 = x3 = %.2f\n",
               x1, x2);
        printf("Разложение на множители: (%.2f)x^3 + (%.2f)x^2 + (%.2f)x + %.2f = (%.2f)(x - %.2f)(x - %.2f)^2\n",
               a, b, c, d, a, x1, x2);
    } else { // три различных действительных корня
        double phi = acos(-q / 2 * sqrt(-27 / (p * p * p))); // угол
        double x1 = 2 * sqrt(-p / 3) * cos(phi / 3); // первый корень
        double x2 =
                2 * sqrt(-p / 3) * cos((phi + 2 * M_PI) / 3); // второй корень
        double x3 =
                2 * sqrt(-p / 3) * cos((phi + 4 * M_PI) / 3); // третий корень
        printf("Уравнение имеет три различных действительных корня: x1 = %.2f, x2 = %.2f, x3 = %.2f\n",
               x1, x2, x3);
        printf("Разложение на множители: (%.2f)x^3 + (%.2f)x^2 + (%.2f)x + %.2f = (%.2f)(x - %.2f)(x - %.2f)(x - %.2f)\n",
               a, b, c, d, a, x1, x2, x3);
    }
}

// Главная функция сервера
int main() {
    int numbytes; // количество байтов, полученных или отправленных
    int sockfd; // дескриптор сокета
    struct sockaddr_in servaddr, cliaddr; // структуры адресов сервера и клиента
    char buffer[MAXBUF]; // буфер для приема и отправки данных
    socklen_t len; // длина адреса клиента

    // Создаем сокет с протоколом UDP
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    // Заполняем структуру адреса сервера
    servaddr.sin_family = AF_INET; // семейство адресов IPv4
    servaddr.sin_addr.s_addr = inet_addr(
            "127.0.0.1"); // автоматический выбор IP-адреса сервера
    servaddr.sin_port = htons(PORT); // порт сервера в сетевом порядке байтов
    memset(servaddr.sin_zero, '\0',
           sizeof servaddr.sin_zero); // обнуляем оставшуюся часть структуры

    // связываем сокет с адресом сервера
    if (bind(sockfd, (struct sockaddr *) &servaddr, sizeof servaddr) == -1) {
        perror("bind");
        exit(1);
    }

    printf("Server listening on %s:%d\n", inet_ntoa(servaddr.sin_addr),
           ntohs(servaddr.sin_port));

    // бесконечный цикл обработки запросов клиентов
    while (1) {
        // Принимаем данные от клиента
        len = sizeof(cliaddr); // длина адреса клиента

        // получаем сообщение от клиента в буфер buffer и запоминаем его адрес в cliaddr
        if ((numbytes = recvfrom(sockfd, buffer, MAXBUF, 0,
                                 (struct sockaddr *) &cliaddr,
                                 &len)) == -1) {
            perror("recvfrom");
            exit(1);
        }

        buffer[numbytes] = '\0'; // добавляем нулевой символ в конец сообщения

        printf("Получен запрос от %s:%d\n",
               inet_ntoa(cliaddr.sin_addr),
               ntohs(cliaddr.sin_port)); // выводим адрес клиента
        printf("Пакет длиной %d байтов\n", numbytes);
        printf("Пакет содержит \"%s\"\n", buffer);


        double a, b, c, d; // коэффициенты уравнения
        int n; // количество коэффициентов уравнения

        n = sscanf(buffer, "%lf %lf %lf %lf", &a, &b, &c,
                   &d); // читаем коэффициенты из сообщения


        if (n == 3) { // квадратное уравнение
            printf("Коэффициенты квадратного уравнения: a = %.2f, b = %.2f, c = %.2f\n",
                   a, b, c); // выводим коэффициенты
            solve_quadratic(a, b,
                            c); // решаем квадратное уравнение и выводим разложение на множители
        } else if (n == 4) { // кубическое уравнение
            printf("Коэффициенты кубического уравнения: a = %.2f, b = %.2f, c = %.2f, d = %.2f\n",
                   a, b, c,
                   d); // выводим коэффициенты
            solve_cubic(a, b, c,
                        d); // решаем кубическое уравнение и выводим разложение на множители
        } else {
            printf("Неверный формат запроса.\n"); // некорректное количество коэффициентов
        }
    }

    close(sockfd); // закрываем сокет

    return 0;
}