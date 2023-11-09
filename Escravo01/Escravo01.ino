const int transmitPin = 2;  // Pino TX do Arduino para comunicação serial
const int ledPin = 13;      // Pino LED para indicar transmissão
const int TEMPO_BIT = 3000;

const int vecAddress[8] = {0,1,0,1,0,1,0,1};
const int flagStart[8] = {0,1,1,1,1,1,1,0};
const int flagEnd[8] = {0,1,1,1,1,1,1,0};
const int controlField[8] = {0,0,0,0,0,0,1,1};
const int FCS[8] = {0,0,0,0,0,0,0,0};

//const int vecAddress[8] = {0,1,0,1,0,1,1,0}; Outro arduino
void setup() {
  pinMode(transmitPin, INPUT);  // Configura o pino como entrada
  Serial.begin(9600);  // Inicializa a comunicação serial
}

void loop() {
  if (Serial.available()){
    Serial.println(Serial.read());
  }

  delay(TEMPO_BIT);  // Aguarda um segundo antes de ler novamente
}
