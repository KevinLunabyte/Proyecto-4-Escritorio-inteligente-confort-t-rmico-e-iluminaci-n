#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define PIN_LDR A0
#define PIN_DHT 2
#define PIN_LAMPARA 9
#define PIN_VENTILADOR 10

#define TIPO_DHT DHT22

DHT dht(PIN_DHT, TIPO_DHT);
LiquidCrystal_I2C lcd(0x27, 16, 2);

int valorLDR = 0;
float temperatura = 0.0;
float humedad = 0.0;
bool estadoLampara = false;
int velocidadVentilador = 0;

const int UMBRAL_LUZ = 500;       
const float UMBRAL_TEMP = 26.0;   

void setup() {
  pinMode(PIN_LAMPARA, OUTPUT);
  pinMode(PIN_VENTILADOR, OUTPUT);
  pinMode(PIN_LDR, INPUT);
  
  dht.begin();
  lcd.init();
  lcd.backlight();
  
  lcd.setCursor(0, 0);
  lcd.print("Escritorio Smart");
  delay(2000);
  lcd.clear();
}

void loop() {
  leerSensores();
  tomarDecisiones();
  actuarCargas();
  mostrarPantalla();
  
  delay(1000); 
}

// 1. LECTURA
void leerSensores() {
  valorLDR = analogRead(PIN_LDR);
  temperatura = dht.readTemperature();
  humedad = dht.readHumidity();

  if (isnan(temperatura)) {
    temperatura = 0.0;
  }
}

// 2. DECISIÓN
void tomarDecisiones() {
  // Lámpara encendida si la luz es menor al umbral
  if (valorLDR < UMBRAL_LUZ) { 
    estadoLampara = true;
  } else {
    estadoLampara = false;
  }

  // Ventilador con umbral mínimo (120) para evitar zumbidos y zona ciega del motor
  if (temperatura > UMBRAL_TEMP) {
    int pwmCalculado = map(temperatura * 10, UMBRAL_TEMP * 10, 350, 120, 255);
    velocidadVentilador = constrain(pwmCalculado, 120, 255);
  } else {
    velocidadVentilador = 0;
  }
}

// 3. ACTUACIÓN
void actuarCargas() {
  digitalWrite(PIN_LAMPARA, estadoLampara ? HIGH : LOW);
  analogWrite(PIN_VENTILADOR, velocidadVentilador);
}

// 4. VISUALIZACIÓN
void mostrarPantalla() {
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(temperatura, 1);
  lcd.print("C L:");
  lcd.print(valorLDR);
  lcd.print("   ");

  lcd.setCursor(0, 1);
  lcd.print("Lamp:");
  lcd.print(estadoLampara ? "ON " : "OFF");
  lcd.print(" Fan:");
  int porcVentilador = map(velocidadVentilador, 0, 255, 0, 100);
  lcd.print(porcVentilador);
  lcd.print("%  ");
}