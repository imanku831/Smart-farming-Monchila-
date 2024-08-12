#define BLYNK_TEMPLATE_ID "TMPL6c1AW_AwR"
#define BLYNK_TEMPLATE_NAME "TA MBKM"
#define BLYNK_AUTH_TOKEN "WEyoF9knNTRbLVioBQh3mfU2fN6-qUnB"
#include "DHT.h"               // include library
#include <WiFi.h>              // include library
#include <BlynkSimpleEsp32.h>  // include library

char auth[] = "WEyoF9knNTRbLVioBQh3mfU2fN6-qUnB";
char ssid[] = "Buaya_TPI";
char pass[] = "Sharil30";

#define DHTPIN 4       // Digital pin connected to the DHT sensor
#define RedLed 15      // PIN LED Merah
#define YellowLed 2    // PIN LED Kuning
#define GreenLed 5     // PIN LED Hijau
#define SoilPin 34     // PIN SENSOR TANAH
#define relay 18       // PIN RELAY
#define DHTTYPE DHT11  //Tipe DHT11
DHT dht(DHTPIN, DHTTYPE);

BlynkTimer timer;

void setup() {
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);
  pinMode(RedLed, OUTPUT);     //Lampu Merah Sebagai Output
  pinMode(YellowLed, OUTPUT);  //Lampu Kuning Sebagai Output
  pinMode(GreenLed, OUTPUT);   //Lampu Hijau Sebagai Output
  pinMode(relay, OUTPUT);      //Relay Sebagai Output
  digitalWrite(relay, HIGH);

  dht.begin();
  timer.setInterval(2000L, sensor);
}

void sensor() {

  //SENSOR DHT
  float suhu = dht.readTemperature();     // Baca suhu dalam Celcius
  float kelembaban = dht.readHumidity();  // Baca kelembaban relatif
  if (isnan(kelembaban) || isnan(suhu)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  Serial.print("Suhu: ");
  Serial.print(suhu);
  Serial.println(" C");

  Serial.print("Kelembaban: ");
  Serial.print(kelembaban);
  Serial.println("%");
  Blynk.virtualWrite(V0, suhu);        //Menampilkan hasil suhu pada Blynk
  Blynk.virtualWrite(V1, kelembaban);  //Menampilkan hasil kelembaban pada Blynk
  delay(2000);

  // Kelembaban Tanah
  int soilMoistureValue = analogRead(SoilPin);  // menyimpan nilai analog dari sensor ke esp32
  Serial.print("Nilai Analog = ");
  Serial.println(soilMoistureValue);
  int soilmoisturepercent = map(soilMoistureValue, 4095, 0, 0, 100);  // nilai yg diperoleh dalam bentuk persen setelah dimaping
  delay(2000);

  Serial.print("Kondisi Tanah = ");
  Serial.print(soilmoisturepercent);  //hasil dari maping dalam bentuk persen
  Serial.println("%");

  if (soilmoisturepercent > 60 && soilmoisturepercent <= 100) {  //Ketika Nilai dari soilmoisturepercent >60
    Serial.println("Tanah basah");
    digitalWrite(RedLed, LOW);     //Lampu Merah Mati
    digitalWrite(YellowLed, LOW);  //Lampu Kuning Mati
    digitalWrite(GreenLed, HIGH);  //Lampu Hijau Menyalah
    digitalWrite(relay, LOW);   // Aktifkan relay (misalnya: mematikan pompa air)
    delay(2000);
  } else if (soilmoisturepercent > 30 && soilmoisturepercent <= 60) {  //Ketika Nilai dari soilmoisturepercent >30
    Serial.println("Tanah normal");
    digitalWrite(RedLed, LOW);      //Lampu Merah Mati
    digitalWrite(YellowLed, HIGH);  //Lampu Kuning Menyalah
    digitalWrite(GreenLed, LOW);    //Lampu Hijau Mati
    digitalWrite(relay, HIGH);   // Matikan relay (misalnya: menyalakan pompa air)
    delay(2000);
  } else if (soilmoisturepercent >= 0 && soilmoisturepercent <= 30) {  //Ketika Nilai dari soilmoisturepercent >=0
    Serial.println("Tanah Kering");
    digitalWrite(RedLed, HIGH);    //Lampu Merah Menyalah
    digitalWrite(YellowLed, LOW);  //Lampu Kuning Mati
    digitalWrite(GreenLed, LOW);   //Lampu Hijau Mati
    digitalWrite(relay, HIGH);  // Matikan relay (misalnya: menyalakan pompa air)
    delay(2000);
  }
  Blynk.virtualWrite(V2, soilmoisturepercent);  //Menampilkan hasil persentasi kondisi tanah pada Blynk
  Blynk.virtualWrite(V3, soilMoistureValue);    //Menampilkan hasil nilai analog pada Blynk
}
void loop() {
  Blynk.run();
  timer.run();
}

BLYNK_WRITE(V4) {
  int relayControl = param.asInt();  // Mengontrol Relay dengan Blynk
  if (relayControl == 1) {
    digitalWrite(relay, HIGH);  // Mengaktifkan relay
  } else {
    digitalWrite(relay, LOW);  // Mematikan relay
  }
}