#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <DHT.h>


const int ledRed = D1;
const int ledYellow = D2;
const int ledGreen = D3;

#define FIREBASE_AUTH "AIzaSyC_3scIbdPKsVH3gQbHPH-KMf1WRfiB9ME";
#define FIREBASE_HOST "dht11-f4334-default-rtdb.europe-west1.firebasedatabase.app";

// Wi-Fi mreža
const char* ssid = "Zemii";
const char* password = "kalikali";

#define DHTPIN D4 // GPIO pin connected to the DHT11
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);


float temperature;
float humidity;

float heatBottom;
float heatTop;

bool mode;

// Firebase konfiguracija
FirebaseConfig config;
FirebaseAuth auth;

// Firebase objekat
FirebaseData firebaseData;

void setup() {
  Serial.begin(115200);
  dht.begin();

  pinMode(ledRed, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  pinMode(ledYellow, OUTPUT);

  // Povezivanje na Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Povezivanje na Wi-Fi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nWi-Fi povezan!");
  Serial.print("IP adresa: ");
  Serial.println(WiFi.localIP());

config.api_key = FIREBASE_AUTH;
  config.database_url = FIREBASE_HOST;
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("ok");
  } else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  Firebase.begin(&config, &auth); 
}

void loop() {
  // Dodaj logiku za kontinuirano slanje podataka, ako je potrebno
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
  setTemp(temperature);
  setHumidity(humidity);
 
  getMode();

  if(mode==true)
  {
    getHeatBottom();
    getHeatTop();
    
    if(temperature<heatBottom)
    {
      digitalWrite(ledYellow,LOW);
      digitalWrite(ledGreen,LOW);
      digitalWrite(ledRed,LOW);
    
      digitalWrite(ledGreen,HIGH);
      
      Serial.print("Temperatura: "); Serial.println(temperature);
      Serial.print("HeatBottom: "); Serial.println(heatBottom);
      Serial.print("HeatTop: "); Serial.println(heatTop);
    }
    if(temperature>=heatBottom && temperature<heatTop)
    {
      digitalWrite(ledYellow,LOW);
      digitalWrite(ledGreen,LOW);
      digitalWrite(ledRed,LOW);

      digitalWrite(ledYellow,HIGH);
       
      Serial.print("Temperatura: "); Serial.println(temperature);
      Serial.print("HeatBottom: "); Serial.println(heatBottom);
      Serial.print("HeatTop: "); Serial.println(heatTop);
    }
    if(temperature>=heatTop)
    {
      digitalWrite(ledYellow,LOW);
      digitalWrite(ledGreen,LOW);
      digitalWrite(ledRed,LOW);

      digitalWrite(ledRed,HIGH); 
       Serial.println("Stanje: Temperatura je ispod donje granice.");
      Serial.print("Temperatura: "); Serial.println(temperature);
      Serial.print("HeatBottom: "); Serial.println(heatBottom);
      Serial.print("HeatTop: "); Serial.println(heatTop);
    }

  }
  else{
    digitalWrite(ledYellow,LOW);
    digitalWrite(ledGreen,LOW);
    digitalWrite(ledRed,LOW);
  }

  delay(500);
 
}

void getTemp() {
  if (Firebase.getFloat(firebaseData, "/sensor/temperature")) {
    temperature = firebaseData.floatData();
    Serial.print("Temperature: ");
    Serial.println(temperature);
  } else {
    Serial.println(firebaseData.errorReason());
  }
}
void setTemp(float temp) {
  if (Firebase.setFloat(firebaseData, "/sensor/temperature", temp)) {
    Serial.println("Temperature sent to Firebase.");
  } else {
    Serial.print("Firebase setFloat failed: ");
    Serial.println(firebaseData.errorReason());
  }
}
void getHumidity() {
  if (Firebase.getFloat(firebaseData, "/sensor/humidity")) {
    humidity = firebaseData.floatData();
    Serial.print("Humidity: ");
    Serial.println(humidity);
  } else {
    Serial.println(firebaseData.errorReason());
  }
}
void setHumidity(float Humidity) {
  if (Firebase.setFloat(firebaseData, "/sensor/humidity", Humidity)) {
    Serial.println("Humidity sent to Firebase.");
  } else {
    Serial.print("Firebase setFloat failed: ");
    Serial.println(firebaseData.errorReason());
  }
}
void getMode() {
  if (Firebase.getFloat(firebaseData, "/settings/auto-mode")) {
    mode = firebaseData.boolData();
    Serial.print("Automode: ");
    Serial.println(mode);
  } else {
    Serial.println(firebaseData.errorReason());
  }
}

void getHeatBottom() {
  if (Firebase.getFloat(firebaseData, "/settings/heatBottom")) {
    heatBottom = firebaseData.floatData();
  } else {
    Serial.println(firebaseData.errorReason());
  }
}

void getHeatTop() {
  if (Firebase.getFloat(firebaseData, "/settings/heatTop")) {
    heatTop = firebaseData.floatData();
  } else {
    Serial.println(firebaseData.errorReason());
  }
}