#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <WiFiUdp.h>
#include <Wire.h>
#include <SSD1306Wire.h>
#include <NTPClient.h>

#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>
#include "SoftwareSerial.h"

ESP8266WiFiMulti wifiMulti;
WiFiUDP UDP;
IPAddress timeServerIP;

#define DEVICE "ESP8266"
#define WIFI_SSID "TE-AMO-MAE"
#define WIFI_PASSWORD "amma021014"

// InfluxDB v2 server url, e.g. https://eu-central-1-1.aws.cloud2.influxdata.com (Use: InfluxDB UI -> Load Data -> Client Libraries)
#define INFLUXDB_URL "https://us-east-1-1.aws.cloud2.influxdata.com"
#define INFLUXDB_TOKEN "dBHma1mZugaEQ2EubT2ww0Txbqego5oIednARuMJ8z38CePiMSWPENWh78vN88lbQsHZuSRFYQu3gVWpwYquhg=="
#define INFLUXDB_ORG "igorcosta1410@hotmail.com"
#define INFLUXDB_BUCKET "InteligenciaCondominial"
#define TZ_INFO "WART4WARST,J1/0,J365/25"
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);

int PULSOS = 0;
int SEGURANCA = 1;
int contador = 0;
int x = 0;

String IP_PREDIO_S = "C0001";
String IP_PONTO_S = "";
String PULSOS_S = "";
String SEGURANCA_S = "";

//String IPs[] = {"y", "y", "y", "y", "y", "y", "y", "y", "y", "y", "y", "y", "y", "y", "y", "y", "y", "y"};
//String PUL[] = {"y", "y", "y", "y", "y", "y", "y", "y", "y", "y", "y", "y", "y", "y", "y", "y", "y", "y"};
//String SEG[] = {"y", "y", "y", "y", "y", "y", "y", "y", "y", "y", "y", "y", "y", "y", "y", "y", "y", "y"};

char IPs[16][16] = {'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y',
                    'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y',
                    'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y',
                    'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y',
                    'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y',
                    'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y',
                    'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y',
                    'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y',
                    'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y',
                    'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y',
                    'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y',
                    'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y',
                    'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y',
                    'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y',
                    'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y',
                    'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y'};
int mensagens = 0;

const int BUFFER_SIZE = 64;
char buf[BUFFER_SIZE];
int confiabilidade[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int confiabilidade2 = 0;

Point IP(IP_PREDIO_S);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
SSD1306Wire  display(0x3c, D1, D2); //D1 -> SDA e D2 -> SCK
bool conexaoWifi = 0;
bool conexaoServidor = 0;
String currentDate = "";
String formattedTime = "";


void LimparBuffer() {
  while(Serial.available() > 0) {
    Serial.read();
  }
}

void optionsDisplay(int tela) {
  if(tela == 0) {
    int16_t i;
    display.clear();
    for(i=0; i<display.width(); i+=4) {
      display.drawLine(0, 0, i, display.height()-1);
      display.display(); // Update screen with each newly-drawn line
      delay(1);
    }
    for(i=0; i<display.height(); i+=4) {
      display.drawLine(0, 0, display.width()-1, i);
      display.display();
      delay(1);
    }
    delay(150);

    display.clear();
    for(i=0; i<display.width(); i+=4) {
      display.drawLine(0, display.height()-1, i, 0);
      display.display();
      delay(1);
    }
    for(i=display.height()-1; i>=0; i-=4) {
      display.drawLine(0, display.height()-1, display.width()-1, i);
      display.display();
      delay(1);
    }
    delay(150);
  
    display.clear();
    for(i=display.width()-1; i>=0; i-=4) {
      display.drawLine(display.width()-1, display.height()-1, i, 0);
      display.display();
      delay(1);
    }
    for(i=display.height()-1; i>=0; i-=4) {
      display.drawLine(display.width()-1, display.height()-1, 0, i);
      display.display();
      delay(1);
    }
    delay(150);

    display.clear();
    for(i=0; i<display.height(); i+=4) {
      display.drawLine(display.width()-1, 0, 0, i);
      display.display();
      delay(1);
    }
    for(i=0; i<display.width(); i+=4) {
      display.drawLine(display.width()-1, 0, i, display.height()-1);
      display.display();
      delay(1);
    }
    delay(500);
  }
  else if(tela == 1) {
    display.clear();
    display.drawRect(0, 0, 128, 16);
    display.drawRect(0, 17, 128, 47);
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.setFont(ArialMT_Plain_10);
    display.drawString(13, 02, "Wifi:");
    if(conexaoWifi == 1) display.fillCircle(31, 07, 4);
    else display.drawCircle(31, 07, 4);
    display.drawString(59, 02, "Servidor:");
    if(conexaoServidor == 1) display.fillCircle(88, 07, 4);
    else display.drawCircle(88, 07, 4);
    display.setFont(ArialMT_Plain_16);
    display.drawString(60, 22, "Conectando no");
    display.drawString(60, 40, "Wifi...");
    display.display();
  }
  else if(tela == 2) {
    display.clear();
    display.drawRect(0, 0, 128, 16);
    display.drawRect(0, 17, 128, 47);
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.setFont(ArialMT_Plain_10);
    display.drawString(13, 02, "Wifi:");
    if(conexaoWifi == 1) display.fillCircle(31, 07, 4);
    else display.drawCircle(31, 07, 4);
    display.drawString(59, 02, "Servidor:");
    if(conexaoServidor == 1) display.fillCircle(88, 07, 4);
    else display.drawCircle(88, 07, 4);
    display.setFont(ArialMT_Plain_16);
    display.drawString(60, 22, "Conectando no");
    display.drawString(60, 40, "Servidor...");
    display.display();
  }
  else if(tela == 3) {
    display.clear();
    display.drawRect(0, 0, 128, 16);
    display.drawRect(0, 17, 128, 47);
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.setFont(ArialMT_Plain_10);
    display.drawString(13, 02, "Wifi:");
    if(conexaoWifi == 1) display.fillCircle(31, 07, 4);
    else display.drawCircle(31, 07, 4);
    display.drawString(59, 02, "Servidor:");
    if(conexaoServidor == 1) display.fillCircle(88, 07, 4);
    else display.drawCircle(88, 07, 4);
    display.setFont(ArialMT_Plain_16);
    display.drawString(60, 22, "Atualizando");
    display.drawString(60, 40, "Servidor...");
    display.display();
  }
  else if(tela == 4) {
    display.clear();
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.setFont(ArialMT_Plain_10);
    display.drawString(120, 02, "H");
    display.display();
  }
  else if(tela == 5){
    display.clear();
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.setFont(ArialMT_Plain_10);
    display.drawString(120, 02, "H");
    display.drawString(20, 02, IP_PONTO_S);
    display.drawString(65, 02, PULSOS_S);
    display.drawString(100, 02, "S"+SEGURANCA_S);
    
    display.setFont(ArialMT_Plain_10);
    display.drawString(49, 20, "Última Atualização: ");
    display.setFont(ArialMT_Plain_16);
    display.drawString(66, 33, formattedTime);
    display.setFont(ArialMT_Plain_10);
    display.drawString(66, 50, currentDate);
    display.display();
  }
  else if(tela == 6) {
    display.clear();
    display.drawRect(0, 0, 128, 16);
    display.drawRect(0, 17, 128, 47);
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.setFont(ArialMT_Plain_10);
    display.drawString(13, 02, "Wifi:");
    if(conexaoWifi == 1) display.fillCircle(31, 07, 4);
    else display.drawCircle(31, 07, 4);
    display.drawString(59, 02, "Servidor:");
    if(conexaoServidor == 1) display.fillCircle(88, 07, 4);
    else display.drawCircle(88, 07, 4);
    display.setFont(ArialMT_Plain_16);
    display.drawString(60, 22, "Conexão");
    display.drawString(60, 40, "Falhou!");
    display.display();
  }
}

void startUDP() {
  timeClient.begin();
  timeClient.setTimeOffset(-10800);
  timeClient.update();
  time_t epochTime = timeClient.getEpochTime();
  
  formattedTime = timeClient.getFormattedTime();
  Serial.println(formattedTime);  
  
  int currentHour = timeClient.getHours(); 
  int currentMinute = timeClient.getMinutes();
  int currentSecond = timeClient.getSeconds();

  struct tm *ptm = gmtime ((time_t *)&epochTime); 
  int monthDay = ptm->tm_mday;
  int currentMonth = ptm->tm_mon+1;
  int currentYear = ptm->tm_year+1900;

  currentDate = String(currentYear) + "-" + String(currentMonth) + "-" + String(monthDay);
  Serial.println(currentDate);
}

void startDisplay() {
  display.init();
  display.flipScreenVertically();
}

void startInfluxdb() {
  optionsDisplay(2);
  IP.addTag("IP", IP_PONTO_S);
  timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");
  
  if (client.validateConnection()) {
    Serial.print("Connected to InfluxDB: ");
    Serial.println(client.getServerUrl());
    conexaoServidor = 1;
  } 
  else {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(client.getLastErrorMessage());
    optionsDisplay(6);
    delay(2000);
    conexaoServidor = 0;
  }
  delay(10);
}

void startWifi() {
  optionsDisplay(1);
  WiFi.mode(WIFI_STA);
  wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("Connecting to wifi");
  int l = 0;
  while (wifiMulti.run() != WL_CONNECTED && l < 5) {
    l++;
    delay(50);
  }
  if(l < 4) {
    conexaoWifi = 1;
    startUDP();
    startInfluxdb();
  }
  else {
    optionsDisplay(6);
    delay(2000);
    conexaoWifi = 0;
  }
}

void atualizarStatus() {
  if (WiFi.status() == WL_CONNECTED) {
    conexaoWifi = 1;     
  }else {
    conexaoWifi = 0;
  }
  if (client.validateConnection()) {
    conexaoServidor = 1;
  }else {
    conexaoServidor = 0;
  }
  delay(20);
}

void wifiOff() {
  WiFi.mode(WIFI_OFF);
  delay(10);
  WiFi.forceSleepBegin();
  delay(10);
  optionsDisplay(5);
  conexaoWifi = 0;
  conexaoServidor = 0;
}

void wifiOn() {
  WiFi.forceSleepWake();
  delay(10);
  startWifi();
}

void setup() {
  Serial.begin(1200);
  delay(20);

  startDisplay(); 
  optionsDisplay(0);
  
  wifiOn();
  delay(50);
  wifiOff();
}

int contador_64 = 0;

void read_RF() {
  if (Serial.available() > 0) {
    char myStrings[6][16] = {'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y',
                             'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y',
                             'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y',
                             'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y',
                             'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y',
                             'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y'};

    int rlen = Serial.readBytes(buf, BUFFER_SIZE);
    int x1 = 0;
    for(int i = 0; i < rlen; i++) {
      Serial.print(buf[i]);
      if (buf[i] == 'M') {
        for(int j = 0; j < 16; j++) {
          if (i+j < 64) {
            myStrings[x1][j] = buf[i+j];
            //Serial.print(myStrings[x1][j]);
          }
          else {
            j = 16;
            i = rlen;
          }
        }
        x1++;
      }
    }
    Serial.println(" ");

    if (x1 > 1) {
      int soma[6][16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
                          
      for(int z = 0; z < 16; z++) {
        for(int i = 0; i < 6; i++) {
          for(int j = 0; j < 6; j++) {
            if (myStrings[i][z] != 'y' && myStrings[i][z] == myStrings[j][z] && (myStrings[i][z] == '0' || myStrings[i][z] == '1' ||
                myStrings[i][z] == '2' || myStrings[i][z] == '3' || myStrings[i][z] == '4' || myStrings[i][z] == '5' ||
                myStrings[i][z] == '6' || myStrings[i][z] == '7' || myStrings[i][z] == '8' || myStrings[i][z] == '9' ||
                myStrings[i][z] == 'a' || myStrings[i][z] == 'b' || myStrings[i][z] == 'c' || myStrings[i][z] == 'd' ||
                myStrings[i][z] == 'e' || myStrings[i][z] == 'f' || myStrings[i][z] == 'M' || myStrings[i][z] == 'C' ||
                myStrings[i][z] == 'S' || myStrings[i][z] == 'F')) {
              soma[i][z] = soma[i][z] + 1;
            }
          }
        }
      }
      
      int msg[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
      int maior = 0;
      for (int z = 0; z < 16; z++) {
        maior = soma[0][z];
        for (int i = 0; i < 6; i++) {
           if (soma[i][z] > maior) {
               maior = soma[i][z];
               msg[z] = i;
           }
        }
      }

      for (int j = 0; j < 16; j++) {
        if (myStrings[msg[j]][j] != '0' && myStrings[msg[j]][j] != '1' && myStrings[msg[j]][j] != '2' && myStrings[msg[j]][j] != '3' && 
            myStrings[msg[j]][j] != '4' && myStrings[msg[j]][j] != '5' && myStrings[msg[j]][j] != '6' && myStrings[msg[j]][j] != '7' && 
            myStrings[msg[j]][j] != '8' && myStrings[msg[j]][j] != '9' && myStrings[msg[j]][j] != 'a' && myStrings[msg[j]][j] != 'b' && 
            myStrings[msg[j]][j] != 'c' && myStrings[msg[j]][j] != 'd' && myStrings[msg[j]][j] != 'e' && myStrings[msg[j]][j] != 'f' &&
            myStrings[msg[j]][j] != 'M' && myStrings[msg[j]][j] != 'S' && myStrings[msg[j]][j] != 'F' && myStrings[msg[j]][j] != 'C') {
        }
        else {
          confiabilidade[j] = 1;
          IPs[mensagens][j] = myStrings[msg[j]][j];
        }
        if (j == 15) mensagens++;
      }

      int conf = 0;
      for (int j = 0; j < 16; j++) {
        if (confiabilidade[j] == 1) conf++;
      }
      Serial.println(conf);

      if (conf == 16) {
        int soma[16][16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
                                    
        for(int z = 0; z < 16; z++) {
          for(int i = 0; i < mensagens; i++) {
            for(int j = 0; j < mensagens; j++) {
              if(IPs[z][i] != 'y' && IPs[z][i] == IPs[z][j] && i != j) {
                soma[z][i] = soma[z][i] + 1;
              }
            }
          }
        }
        
        int msg[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        int maior = 0;
        for (int z = 0; z < 16; z++) {
          maior = soma[0][z];
          for (int i = 0; i < mensagens; i++) {
             if (soma[i][z] > maior) {
                 maior = soma[i][z];
                 msg[z] = i;
             }
          }
        }

        IP_PONTO_S = "";
        PULSOS_S = "";
        SEGURANCA_S = "";
        
        int safe = 0;
        for(int j = 0; j < 5; j++) {
          if(IPs[msg[j]][j] != '0' && IPs[msg[j]][j] != '1' && IPs[msg[j]][j] != '2' && IPs[msg[j]][j] != '3' && 
             IPs[msg[j]][j] != '4' && IPs[msg[j]][j] != '5' && IPs[msg[j]][j] != '6' && IPs[msg[j]][j] != '7' && 
             IPs[msg[j]][j] != '8' && IPs[msg[j]][j] != '9' && IPs[msg[j]][j] != 'a' && IPs[msg[j]][j] != 'b' && 
             IPs[msg[j]][j] != 'c' && IPs[msg[j]][j] != 'd' && IPs[msg[j]][j] != 'e' && IPs[msg[j]][j] != 'f' && 
             IPs[msg[j]][j] != 'M') safe++;
          IP_PONTO_S.concat(IPs[msg[j]][j]);
        }
        for(int j = 6; j < 13; j++) {
          if(IPs[msg[j]][j] != '0' && IPs[msg[j]][j] != '1' && IPs[msg[j]][j] != '2' && IPs[msg[j]][j] != '3' && 
             IPs[msg[j]][j] != '4' && IPs[msg[j]][j] != '5' && IPs[msg[j]][j] != '6' && IPs[msg[j]][j] != '7' && 
             IPs[msg[j]][j] != '8' && IPs[msg[j]][j] != '9' && IPs[msg[j]][j] != 'a' && IPs[msg[j]][j] != 'b' && 
             IPs[msg[j]][j] != 'c' && IPs[msg[j]][j] != 'd' && IPs[msg[j]][j] != 'e' && IPs[msg[j]][j] != 'f') safe++;
          PULSOS_S.concat(IPs[msg[j]][j]);
        }
        if(IPs[msg[14]][14] != '0' && IPs[msg[14]][14] != '1') safe++;
        SEGURANCA_S.concat(IPs[msg[14]][14]);

        if(safe == 0) {
          for(int i = 0; i < 16; i++) confiabilidade[i] = 0;
          for(int i = 0; i < 16; i++) for(int j = 0; j < 16; j++) IPs[i][j] = 'y';
          confiabilidade2 = 1;
          mensagens = 0; 
          Serial.println(IP_PONTO_S);
          Serial.println(PULSOS_S);
          Serial.println(SEGURANCA_S); 
        }   
      }

      if(mensagens == 16) {
        mensagens = 0;
      }
  
      /*if (myStrings[msg[0]][0] == 'M' && myStrings[msg[5]][5] == 'C' && myStrings[msg[13]][13] == 'S' && myStrings[msg[15]][15] == 'F' &&
         (myStrings[msg[14]][14] == '0' || myStrings[msg[14]][14] == '1')) {
        confiabilidade = 1;
        IP_PONTO_S = "";
        PULSOS_S = "";
        SEGURANCA_S = "";
        for (int j = 0; j < 16; j++) {
          if (myStrings[msg[j]][j] != '0' && myStrings[msg[j]][j] != '1' && myStrings[msg[j]][j] != '2' && myStrings[msg[j]][j] != '3' && 
              myStrings[msg[j]][j] != '4' && myStrings[msg[j]][j] != '5' && myStrings[msg[j]][j] != '6' && myStrings[msg[j]][j] != '7' && 
              myStrings[msg[j]][j] != '8' && myStrings[msg[j]][j] != '9' && myStrings[msg[j]][j] != 'a' && myStrings[msg[j]][j] != 'b' && 
              myStrings[msg[j]][j] != 'c' && myStrings[msg[j]][j] != 'd' && myStrings[msg[j]][j] != 'e' && myStrings[msg[j]][j] != 'f' &&
              myStrings[msg[j]][j] != 'M' && myStrings[msg[j]][j] != 'S' && myStrings[msg[j]][j] != 'F' && myStrings[msg[j]][j] != 'C') {
              confiabilidade = 0;
              j = 17;
          }
          if (j < 5) {
            IP_PONTO_S.concat(myStrings[msg[j]][j]);
          }
          else if (j > 5 && j < 13 ) {
            PULSOS_S.concat(myStrings[msg[j]][j]);
          }
          else if (j == 14) {
            SEGURANCA_S.concat(myStrings[msg[j]][j]);
          }
        }
        if (confiabilidade == 1) {
          IPs[mensagens] = IP_PONTO_S;
          PUL[mensagens] = PULSOS_S;
          SEG[mensagens] = SEGURANCA_S;
          x = x1;
          mensagens++;
        }
      //}
      }
      LimparBuffer();
    }
    else {
      if (mensagens < 2 && contador_64 > 15) {
        mensagens = 0;
        contador_64 = 0;
      }
      if (mensagens > 2 && contador_64 > 5) {
        int i[mensagens];
        int p[mensagens];
        int s[mensagens];
        for (int z = 0; z < mensagens; z++) {
          i[z] = 0;
          p[z] = 0;
          s[z] = 0;
          for (int y = 0; y < mensagens; y++) {
            if (IPs[z] == IPs[y]) {
              i[z] = i[z] + 1;
            }
            if (PUL[z] == PUL[y]) {
              p[z] = p[z] + 1;
            }
            if (SEG[z] == SEG[y]) {
              s[z] = s[z] + 1;
            }
          }        
        }      
        int maior1 = 0;
        int maior2 = 0;
        int maior3 = 0;
        int seg1 = -1;
        int seg2 = -1;
        int seg3 = -1;
        for (int z = 0; z < mensagens; z++) {
           if (i[z] > maior1) {
             maior1 = i[z];
             if ((maior1 > 1 && mensagens > 1) || (maior1 > 0 && mensagens == 1)) {
               seg1 = z;
             }
           }
           if (p[z] > maior2) {
             maior2 = p[z];
             if ((maior2 > 1 && mensagens > 1) || (maior2 > 0 && mensagens == 1)) {
               seg2 = z;
             }
           }
           if (s[z] > maior3) {
             maior3 = s[z];
             if ((maior3 > 1 && mensagens > 1) || (maior3 > 0 && mensagens == 1)) {
               seg3 = z;
             }
           }
        }
        if (seg1 != -1 && seg2 != -1 && seg3 != -1) {
          IP_PONTO_S = IPs[seg1];
          PULSOS_S = PUL[seg2];
          SEGURANCA_S = SEG[seg3];
          confiabilidade2 = 1;
        }
        confiabilidade[0] = 0;
        mensagens = 0;
        contador_64 = 0;
      }*/
    }
    LimparBuffer();
  }
}

void check_signal() {
  if ((contador > 100) && (confiabilidade2 == 1)){
    wifiOn();
    if (conexaoWifi == 1 && conexaoServidor == 1) {
      optionsDisplay(3);
      IP.clearTags();
      IP.clearFields();
      IP.addTag("IP", IP_PONTO_S);
      timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");
      IP.addField("Consumo", int(strtol(PULSOS_S.c_str(), 0, 16)));
      IP.addField("Sistema", SEGURANCA_S.toInt());

      Serial.print("Writing: ");
      Serial.println(IP.toLineProtocol());
    
      if (!client.writePoint(IP)) {
        Serial.print("InfluxDB write failed: ");
        Serial.println(client.getLastErrorMessage());
      }
      startUDP();
      atualizarStatus();
    }  
    wifiOff();
    confiabilidade2 = 0;
    contador = 0;
  }
}

void loop() {
  read_RF();
  //check_signal();
    
  if (contador > 5000) contador = 200;

  contador++;
}
