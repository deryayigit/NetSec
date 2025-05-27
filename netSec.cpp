#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

// Gerekli kütüphaneler
#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <fstream>
#include <map>
#include <thread>
#include <mutex>
#include <ctime>
#include <cstdlib>
#include <cstdio>

#pragma comment(lib, "ws2_32.lib") // Winsock kütüphanesini bağla

using namespace std;

mutex dosya_mutex;
ofstream dosya("sonuclar_windows.txt", ios::out | ios::trunc); // Log dosyası

// ----------------------------- SERVİS ADI HARİTASI -----------------------------
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

// ----------------------------- ZAMAN DAMGASI -----------------------------
string zaman_damgasi() {
    time_t simdi = time(0);
    tm* ltm = localtime(&simdi);
    char zaman[64];
    strftime(zaman, sizeof(zaman), "%Y-%m-%d %H:%M:%S", ltm);
    return string(zaman);
}

// ----------------------------- OS TAHMİNİ (BANNER ANALİZİ) -----------------------------
string os_tahmini_banner(const string& banner) {
    string banner_lc = banner;
    for (char& c : banner_lc) c = tolower(c);
    if (banner_lc.find("ubuntu") != string::npos) return "Ubuntu tabanlı Linux";
    if (banner_lc.find("debian") != string::npos) return "Debian tabanlı Linux";
    if (banner_lc.find("centos") != string::npos) return "CentOS tabanlı Linux";
    if (banner_lc.find("windows") != string::npos) return "Muhtemelen Windows";
    return "";
}

// ----------------------------- TCP BANNER ALMA (Application Layer) -----------------------------
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

// ----------------------------- TCP PORT TESPİTİ -----------------------------
void tcp_tara(const string& ip, int port) {
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
        cout << zaman_damgasi() << " | TCP Port: " << port << " açık\n";
        dosya << zaman_damgasi() << " | TCP Port: " << port << " açık\n";

        if (!banner.empty()) {
            cout << "\t[Banner: " << banner.substr(0, 60) << "]\n";
            dosya << "\t[Banner: " << banner.substr(0, 60) << "]\n";
        }
        else {
            cout << "\t[Banner alınamadı: port açık ama servis cevap vermiyor olabilir]\n";
            dosya << "\t[Banner alınamadı: port açık ama servis cevap vermiyor olabilir]\n";
        }
    }
    closesocket(s);
}

// ----------------------------- UDP PORT TESPİTİ -----------------------------
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

// ----------------------------- ANA PROGRAM -----------------------------
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

    string ip = input;
    in_addr addr;
    if (inet_pton(AF_INET, input.c_str(), &addr) != 1) {
        addrinfo hints = {}, * res;
        hints.ai_family = AF_INET;
        if (getaddrinfo(input.c_str(), nullptr, &hints, &res) != 0) {
            cerr << "Hostname/IP çözülemedi!\n";
            return 1;
        }
        sockaddr_in* ipv4 = (sockaddr_in*)res->ai_addr;
        ip = inet_ntoa(ipv4->sin_addr);
        freeaddrinfo(res);
    }

    cout << "\n[+] OS tespiti yapılıyor...\n";
    string tahmini_os = "Tespit edilemedi (Ping cevapsız veya TTL alınamadı)";
    string komut = "ping -n 1 " + ip;
    FILE* pipe = _popen(komut.c_str(), "r");
    string cikti;
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), pipe)) cikti += buffer;
    _pclose(pipe);

    size_t pos = cikti.find("TTL=");
    if (pos != string::npos) {
        int ttl = stoi(cikti.substr(pos + 4));
        if (ttl >= 128) tahmini_os = "Muhtemelen Windows";
        else if (ttl >= 64) tahmini_os = "Muhtemelen Linux";
        else if (ttl > 0) tahmini_os = "Muhtemelen Embedded/Router/IoT";
    }
    cout << "\n[+] Tahmini isletim sistemi: " << tahmini_os << "\n";

    thread threadler[port_sayisi * 2];
    int idx = 0;
    for (int i = 0; i < port_sayisi; ++i) {
        int port = portlar[i];
        threadler[idx++] = thread([=]() {
            tcp_tara(ip, port);
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
