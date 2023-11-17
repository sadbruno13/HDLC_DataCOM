// --------- Flag Inicio/Fim --------- //
const uint8_t StartEndFLAG = 0x7E;
// ---------------------------------- //

// ------------ Endereços Escravos ------------ //
const byte ID_1 = 1;
const byte ID_2 = 2;
const byte BROADCAST = 3;
// -------------------------------------------- //

// ----------ControlField------ //
const byte CONTROLFIELD = 1;
//const uint16_t CONTROL = 2; 
//const uint16_t CONTROL = 3; 
//const uint16_t CONTROL = 4;
// --------------------------- //

// -------------- Mensagens -------------- //
const char* mensagemMestre = "Ola";
// -------------------------------------- //

// ------------- FCS -------------------- //
uint16_t FCS;
// -------------------------------------- //

// -------- Resposta Escravos (ACK/NAK) ------- //
const byte OK = 0x06;
const byte NOK = 0x15;
// ------------------------------------------- //


// ------- Outras Constantes ------- //
const unsigned long TIMEOUT = 15000;
uint16_t IDSLC;
//uint16_t selectedControl;
// -------------------------------- //

void setup() {
  Serial.begin(9600);
}

void loop() {
  EnviarMensagem(ID_2, CONTROLFIELD, mensagemMestre);
  delay(15000);
}

void EnviarMensagem(byte endereco, byte controlField, const char* mensagem){
  
  Serial.write(StartEndFLAG);
  Serial.write(endereco);
  Serial.write(controlField);
  Serial.write(mensagem);
  Serial.flush();

  FCS = calculateFCS(controlField, endereco, mensagem);
  EnviarFCSSerial(FCS); // Por ser 16 bits, necessitou dividir e enviar separadamente

  Serial.write(StartEndFLAG);

}

// Função para calcular o FCS usando CRC
uint16_t calculateFCS(uint8_t controlField, uint8_t endereco, const char* mensagem) {
  uint16_t fcs = 0xFFFF; // Valor inicial do FCS

  // Calcula o FCS para o campo de controle
  fcs = crc16Update(fcs, controlField);

  // Calcula o FCS para o campo de endereço
  fcs = crc16Update(fcs, endereco);

  // Calcula o FCS para a mensagem
  while (*mensagem) {
    fcs = crc16Update(fcs, *mensagem++);
  }

  return ~fcs; // Complemento de 1 do FCS final
}

// Função auxiliar para atualizar o valor do FCS usando CRC
uint16_t crc16Update(uint16_t fcs, uint8_t data) {
  fcs ^= data;
  for (int i = 0; i < 8; i++) {
    if (fcs & 1) {
      fcs = (fcs >> 1) ^ 0xA001; // Polinômio CRC-16
    } else {
      fcs >>= 1;
    }
  }
  return fcs;
}

void EnviarFCSSerial(uint16_t FCS){
  uint8_t lsb = FCS & 0xFF;         // byte menos significativo
  uint8_t msb = (FCS >> 8) & 0xFF;  // byte mais significativo
  Serial.write(msb); //Envia pelo mais significativo
  Serial.write(lsb); //Envia o menos significativo
}
