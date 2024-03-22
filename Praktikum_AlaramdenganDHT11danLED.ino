#include <ESP8266WiFi.h> //memasukan library ESP8266
#include "DHT.h" //memasukan library dht11

//mendefinisikan sensor yang dipakai (DHT11, DHT21, dan DHT22)
#define DHTTYPE DHT11 //tipe yang dipilih DHT 11
#define LED D3

//Nama wifi yang akan dikHidupeksikan
const char* ssid = "NYONYA SIMPING";
//masukan password wifinya
const char* password = "12345678910abc";

WiFiServer server(80);

// DHT Sensor pada pin D2 di Wemos
const int DHTPin = D2;
//inisialisasi library DHTpin
DHT dht(DHTPin, DHTTYPE);

void setup() {
  Serial.begin(9600);
  delay(20);
  dht.begin();
  pinMode(LED, OUTPUT);


  Serial.println();
  // Mengkoneksikan ke wifi
  Serial.print("Menghubungkan ke jaringan... ");
  Serial.println(ssid);

  //Mulai koneksikan dengan via wifi
  WiFi.begin(ssid, password);

  //syarat kondisi pengkoneksian
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Jaringan WiFi terkoneksi");

  // memulai
  server.begin();
  Serial.println("Koneksi Server dimulai");

  Serial.print("alamat IP yang untuk pengaksesan: ");
  //penulisan alamat ip
  Serial.print("http://");
  //IP address
  Serial.print(WiFi.localIP());
  Serial.println("/");
}

void loop() {
  // mengecek jika client sudah terkoneksi
  WiFiClient client = server.available();
  //jika tidak client yang terkoneksi
  if (!client) {
    return;
  }

  Serial.println("Koneksi baru");
  //Jika sudah ada client baru maka
  while (!client.available()) {
    delay(5);
  }

  if (client) {

    boolean blank_line = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();

        if (c == '\n' && blank_line) {
          // Pembacaan sensor juga bisa sampai 2 detik 'lama' (sensornya sangat lambat)
          float h = dht.readHumidity();
          // Baca suhu sebagai Celsius (default)
          float t = dht.readTemperature();
          // Baca suhu sebagai Fahrenheit (apakah Fahrenheit = benar)
          float f = dht.readTemperature(true);
          // Periksa apakah ada yang membaca gagal dan keluar lebih awal (coba lagi)
          if (isnan(h) || isnan(t) || isnan(f)) {
            Serial.println("Failed to read from DHT sensor!");
          }
          else {
            Serial.print("Kelembaban : ");
            Serial.print(h);
            Serial.println("%");

            Serial.print("Suhu : ");
            Serial.print(t);
            Serial.print(" *C ");
            Serial.println(f);
          }

          if (t > 30) {
            digitalWrite(LED, HIGH);  // LED menyala saat suhu di atas 30°C

          } else {
            digitalWrite(LED, LOW);
          }

          // Menuliskan dalam format HTML
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");
          client.println();

          // Halaman web Anda yang sebenarnya menampilkan suhu dan kelembaban
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          client.println("<head></head><body><center><h1>Alarm Suhu Mengunakan DHT11 Dan LED</h1>");
          client.println("<p>Jika suhu melebihi 30*C maka LED akan Menyala</p>");
          client.println("<h3>Suhu dalam Celcius: ");
          client.println(t); //celsiusTemp
          client.println("*C</h3><h3>Suhu ke dalam Fahrenheit: ");
          client.println(f); //fahrenheitTemp
          client.println("*F</h3><h3>Kelembaban: ");
          client.println(h);
          client.println("%</h3><h3>Kondisi LED: ");
          if (digitalRead(LED) == HIGH) {
            client.println("LED menyala");
          } else {
            client.println("LED mati");
          }
          client.println("</h3></center>");
          client.println("</body></html>");
          break;
        }
        if (c == '\n') {
          // Saat mulai membaca baris baru
          blank_line = true;
        }
        else if (c != '\r') {
          // Ketika menemukan karakter pada baris saat ini
          blank_line = false;
        }
      }
    }
  }
  // Menutup koneksi klien
  delay(5);
}