1. Temel Bilgiler 

 1.1. Problem Tanımı ve Amaç 

  IP adresi verilen uzak makinede çalışan işletim sistemini tespit edebilen,  
  
  sunucu üzerindeki açık portların ve bu portlar üzerinde çalışan servislerin  
  
  isimlerini listeleyen bir program. Projenin gereklilikleri: 
  
  Hangi işletim sistemini kullandığını banner verisiyle tahmin etmek, 
  
  Hangi TCP ve UDP portlarının açık olduğunu eş zamanlı olarak taramak, 
  
  Açık portlar üzerinde çalışan servis isimlerini tespit etmek, 
  
  Tüm bu bilgileri zaman damgalarıyla birlikte dosyaya ve ekrana yazmaktır. 

1.2. Proje Bölümleri 

  Giriş Birimi: IP/domain alımı ve çözümleme 
  
  TCP Port Taraması: Her bir TCP portuna bağlantı kurulup servis/bilgi çekilmesi 
  
  UDP Port Taraması: UDP ile basit test mesajı gönderilmesi ve yanıt beklenmesi 
  
  Banner Analizi: TCP bağlantısından gelen banner verisinin alınması 
  
  OS Tahmini: Banner içeriğine göre olası işletim sistemi çıkarımı 
  
  Veri Kaydı: Sonuçların sonuclar_windows.txt adlı dosyaya kaydı 
  
  Eşzamanlılık: Tüm taramaların thread'ler ile paralel gerçekleştirilmesi 

1.3. Kullanılan Teknoloji ve Araçlar 

  Programlama Dili: C++ 
  
  Geliştirme Ortamı (IDE): Microsoft Visual Studio (Windows) 
  
  Kütüphaneler: 
  
  +winsock2.h & ws2tcpip.h (ağ programlama için) 
  
  +thread, mutex (çoklu iş parçacığı yönetimi için) 
  
  +fstream, map, ctime (veri işleme ve dosya kaydı) 

1.4. Donanımsal Gereksinimler 

  +İşlemci: x86_64 mimaride çalışan herhangi bir modern işlemci 
  
  +Bellek: Minimum 8 GB RAM 
  
  +Depolama: En az 8 GB boş disk alanı 
  
  +İşletim Sistemi: Windows 10/11 (geliştirme ortamı), istenirse sanal makineler üzerinden Linux test desteği 

 

2. Projeye Dair Teknik Gereklilikler 

  2.1. Projenin Algoritması: 
  
   Kullanıcıdan IP/domain alınır ve çözümleme yapılır. 
   
   Belirli TCP portlara bağlantı sağlanır, servis banner'ı alınmaya çalışılır. 
   
   Banner’da işletim sistemi bilgisi aranır ve OS tahmini yapılır. 
   
   Aynı anda tüm portlar hem TCP hem UDP üzerinden taranır. 
   
   Elde edilen sonuçlar dosyaya ve ekrana yazılır. 

 2.2. Proje Adımları ve Kritik Kısımlar 

   Winsock Başlatma (WSAStartup): Windows ortamında ağ fonksiyonlarının kullanılabilmesi için zorunludur. 
   
   Banner Alımı: Servis hakkında bilgi içeren metinlerin alınması, OS tahmini için kritiktir. 
   
   Paralel Port Taraması: Thread kullanımı ile işlem süresi önemli ölçüde azaltılmıştır. 
   
   Zaman Damgası ve Loglama: Dosya erişim çakışmalarının önlenmesi için mutex kullanılmıştır. 


 

2. Projenin Genel Görünümü  

Bu proje temel port tarayıcı yazılımlarının (örn. Nmap) temelini oluşturan teknikleri uygulamalı olarak ortaya koyar. Aynı anda TCP ve UDP tarama yapabilmesi, servis banner bilgilerini okuyup OS tahmini yapabilmesi gibi özellikleriyle sadece teorik bir çalışma değil, gerçek dünya problemlerine yönelik bir çözüm sunmaktadır. Projede ağ programlama, eşzamanlılık, protokol analizi ve sistem güvenliği gibi birçok konu entegre bir biçimde işlenmiştir. 
