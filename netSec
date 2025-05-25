#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <string>
#include <locale>
#include <codecvt>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <array>
#include <map>
#include <windows.h>
#include <thread>
#include <mutex>
#include <ctime>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

mutex dosya_mutex;
ofstream dosya("sonuclar_windows.txt", ios::out | ios::trunc);

const int portlar[] = { 21, 22, 23, 25, 53, 80, 110, 143, 443, 3306, 3389, 853, 8080, 8443, 5900 };
const int port_sayisi = sizeof(portlar) / sizeof(portlar[0]);

string servis_ismi(int port) {
    map<int, string> port_harita = {
        {21, "FTP"}, {22, "SSH"}, {23, "Telnet"}, {25, "SMTP"},
        {53, "DNS"}, {80, "HTTP"}, {110, "POP3"}, {143, "IMAP"},
        {443, "HTTPS"}, {3306, "MySQL"}, {3389, "RDP"},
        {853, "DoH"}, {8080, "HTTP-Alt"}, {8443, "HTTPS-Alt"}, {5900, "VNC"}
    };
    if (port_harita.count(port)) return port_harita[port];
    return "Bilinmeyen";
}

string os_tahmini_banner(const string& banner);
string banner_al(const string& ip, int port);
string zaman_damgasi();
void udp_tara(const string& ip, int port);

string zaman_damgasi() {
    time_t simdi = time(0);
    tm* ltm = localtime(&simdi);
    char zaman[64];
    strftime(zaman, sizeof(zaman), "%Y-%m-%d %H:%M:%S", ltm);
    return string(zaman);
}

string os_tahmini_banner(const string& banner) {
    string banner_lc = banner;
    for (char& c : banner_lc) c = tolower(c);
    if (banner_lc.find("ubuntu") != string::npos) return "Ubuntu tabanlı Linux";
    if (banner_lc.find("debian") != string::npos) return "Debian tabanlı Linux";
    if (banner_lc.find("centos") != string::npos) return "CentOS tabanlı Linux";
    if (banner_lc.find("windows") != string::npos) return "Muhtemelen Windows";
    return "";
}

string banner_al(const string& ip, int port) {
    SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == INVALID_SOCKET) return "";

    sockaddr_in target;
    target.sin_family = AF_INET;
    target.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &target.sin_addr);

    DWORD timeout = 200;
    setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));

    if (connect(s, (sockaddr*)&target, sizeof(target)) == SOCKET_ERROR) {
        closesocket(s);
        return "";
    }

    map<string, string> protokol_mesaji = {
        {"HTTP", "HEAD / HTTP/1.0\r\n\r\n"},
        {"SSH", "SSH-2.0-Scanner\r\n"},
        {"SMTP", "EHLO test\r\n"},
        {"FTP", "USER anonymous\r\n"}
    };

    string servis = servis_ismi(port);
    string istek = protokol_mesaji.count(servis) ? protokol_mesaji[servis] : "\r\n";

    send(s, istek.c_str(), static_cast<int>(istek.size()), 0);

    char buffer[512] = { 0 };
    int bytes = recv(s, buffer, sizeof(buffer), 0);
    closesocket(s);

    if (bytes > 0)
        return string(buffer, bytes);
    else
        return "";
}

void udp_tara(const string& ip, int port) {
    SOCKET s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (s == INVALID_SOCKET) return;

    sockaddr_in hedef;
    hedef.sin_family = AF_INET;
    hedef.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &hedef.sin_addr);

    const char* test_mesaji = "Test";
    sendto(s, test_mesaji, static_cast<int>(strlen(test_mesaji)), 0, (sockaddr*)&hedef, sizeof(hedef));

    char buffer[256];
    sockaddr_in from;
    int fromlen = sizeof(from);
    DWORD timeout = 200;
    setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));

    int sonuc = recvfrom(s, buffer, sizeof(buffer), 0, (sockaddr*)&from, &fromlen);

    lock_guard<mutex> kilit(dosya_mutex);
    if (sonuc >= 0) {
        cout << zaman_damgasi() << " | UDP Port " << port << " açık\n";
        dosya << zaman_damgasi() << " | UDP Port " << port << " açık\n";
    }
    else {
        cout << zaman_damgasi() << " | UDP Port " << port << " cevap yok – kapalı veya filtrelenmiş olabilir\n";
        dosya << zaman_damgasi() << " | UDP Port " << port << " cevap yok – kapalı veya filtrelenmiş olabilir\n";
    }
    closesocket(s);
}

int main() {
    setlocale(LC_ALL, "Turkish");
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "WSA başlatılamadı.\n";
        return 1;
    }

    string input;
    cout << "IP ya da domain girin: ";
    getline(cin, input);

    addrinfo hints = {}, * res;
    hints.ai_family = AF_INET;
    if (getaddrinfo(input.c_str(), nullptr, &hints, &res) != 0) {
        cerr << "Hostname/IP çözümlenemedi!\n";
        return 1;
    }

    sockaddr_in* ipv4 = (sockaddr_in*)res->ai_addr;
    string ip = inet_ntoa(ipv4->sin_addr);
    freeaddrinfo(res);

    cout << "\n[+] OS tespiti yapılıyor...\n";

    string tahmini_os = "Tespit edilemedi (Ping cevapsız veya TTL alınamadı)";

    for (int i = 0; i < port_sayisi; ++i) {
        int port = portlar[i];
        if (port == 22 || port == 80 || port == 443) {
            string banner = banner_al(ip, port);
            if (!banner.empty()) {
                string os_ipucu = os_tahmini_banner(banner);
                if (!os_ipucu.empty()) {
                    tahmini_os = os_ipucu;
                    break;
                }
            }
        }
    }

    cout << "\n[+] Tahmini isletim sistemi: " << tahmini_os << "\n";

    thread threadler[sizeof(portlar) / sizeof(portlar[0]) * 2];
    int idx = 0;
    for (int i = 0; i < port_sayisi; ++i) {
        int port = portlar[i];
        threadler[idx++] = thread([=]() {
            SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (s == INVALID_SOCKET) return;

            sockaddr_in hedef;
            hedef.sin_family = AF_INET;
            hedef.sin_port = htons(port);
            inet_pton(AF_INET, ip.c_str(), &hedef.sin_addr);

            DWORD timeout = 200;
            setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));

            if (connect(s, (sockaddr*)&hedef, sizeof(hedef)) == 0) {
                string servis = servis_ismi(port);
                string banner = banner_al(ip, port);

                lock_guard<mutex> kilit(dosya_mutex);
                cout << "Port " << port << " açık. [Servis: " << servis << "] ";
                dosya << "Port " << port << " açık. [Servis: " << servis << "] ";
                if (!banner.empty()) {
                    cout << "[Banner: " << banner.substr(0, 60) << "]\n";
                    dosya << "[Banner: " << banner.substr(0, 60) << "]\n";
                }
                else {
                    cout << "[Banner alınamadı: port açık ama servis cevap vermiyor olabilir]\n";
                    dosya << "[Banner alınamadı: port açık ama servis cevap vermiyor olabilir]\n";
                }
            }
            closesocket(s);
            });
    }

    for (int i = 0; i < port_sayisi; ++i) {
        threadler[idx++] = thread([=]() {
            udp_tara(ip, portlar[i]);
            });
    }

    for (int i = 0; i < idx; ++i) threadler[i].join();

    dosya.close();
    WSACleanup();
    return 0;
}
