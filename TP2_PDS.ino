//Configuración del Generador de Señal (PWM)
const int pwmPin = 25;      // Pin que SACA la señal (PWM)
const int freq = 100;       // Frecuencia en Hz 
const int resolution = 8; // 8-bit (0-255)

//Configuración del Procesador (ADC)
const int adcPin = 34;      // Pin que LEE la señal (ADC)

// Período de muestreo 
const int sample_delay_ms = 1; 

//Memoria del Filtro
float x_n1 = 0.0; // Muestra anterior x[n-1]
float x_n2 = 0.0; // Muestra x[n-2]

void setup() {
 Serial.begin(115200); 

  // --- Configurar el PWM ---
  ledcAttach(pwmPin, freq, resolution);
  // Ciclo de trabajo al 50% (onda cuadrada) para tener la mitad del ciclo bajo y la otra en alto
  ledcWrite(pwmPin, 128); // 128 es el 50% de 256 (resolución de 8 bits)
  
  Serial.println("Generador PWM iniciado. Procesador listo.");
  Serial.println("Abra el 'Serial Plotter' (Ctrl+Shift+L)");

}

void loop() {


// Guarda el tiempo actual
  unsigned long tiempoActual = millis();

 // --- Digitalizar la señal ---
  
  // Leer el valor del ADC (0-4095)
  int adcValue = analogRead(adcPin);
  
  // Convertir el valor a Voltaje (0 - 3.3V)
  // El ADC del ESP32 a 12 bits da 4096 pasos
  float x_n = (adcValue * 3.3) / 4095.0; // 

  // --- Implementar Filtro Promediador ---
  float y_promedio = (x_n + x_n1 + x_n2) / 3.0;

  // --- Implementar Ecuación ---
  float y_filtro_eq = x_n + (0.5 * x_n1) - (0.25 * x_n2);

  // --- Enviar todo al Serial Plotter para graficar ---
  Serial.print("Original:");
  Serial.print(x_n);
  Serial.print(",Promediado:");
  Serial.print(y_promedio);
  Serial.print(",Ecuacion:");
  Serial.println(y_filtro_eq);

  // --- Actualizar la memoria del filtro  ---
  x_n2 = x_n1; // El 'n-1' se vuelve 'n-2'
  x_n1 = x_n;  // El 'n' actual se vuelve 'n-1'
  
  // --- Controlar el Período de Muestreo  ---
  delay(sample_delay_ms);

}
