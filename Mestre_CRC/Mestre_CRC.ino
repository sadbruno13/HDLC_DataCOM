const int TEMPO_BIT = 500;

const int vecAddress[8] = {0,1,0,1,0,1,0,1};
const int flagStart[8] = {0,1,1,1,1,1,1,0};
const int flagEnd[8] = {0,1,1,1,1,1,1,0};
const int controlField[8] = {0,0,0,0,0,0,1,1};

int tamanhoMensagem = 0;

int* mensagemParaBits(String mensagem) {
  tamanhoMensagem = mensagem.length();
  int* mensagem_bits = (int*)malloc(8 * tamanhoMensagem * sizeof(int));

  if (mensagem_bits == NULL) {
    Serial.println("Erro na alocação de memória para mensagem_bits.");
    return NULL;
  }

  for (int i = 0; i < tamanhoMensagem; i++) {
    char caractere = mensagem[i];
    for (int j = 0; j < 8; j++) {
      mensagem_bits[(i * 8) + j] = (caractere >> (7 - j)) & 1;
    }
  }

  return mensagem_bits;
}

uint32_t calcularCRC(int* data, int size) {
  uint32_t crc = 0xFFFFFFFF;

  for (int i = 0; i < size; i++) {
    crc ^= data[i] << 24;

    for (int j = 0; j < 8; j++) {
      if (crc & 0x80000000) {
        crc = (crc << 1) ^ 0x04C11DB7; // Polynomial used in CRC32
      } else {
        crc <<= 1;
      }
    }
  }

  return crc;
}

int* montarFrame(String minhaMensagem) {
  int* bitsDaMensagem = mensagemParaBits(minhaMensagem);
  if (bitsDaMensagem == NULL) {
    Serial.println("Erro na conversão da mensagem para bits.");
    return NULL;
  }

  int tamanhoFrame = 8 + 8 + tamanhoMensagem * 8 + 32 + 8 + 8 + 8; // Adiciona 32 bits para CRC
  int* oprFrame = (int*)malloc(tamanhoFrame * sizeof(int));

  if (oprFrame == NULL) {
    Serial.println("Erro na alocação de memória para oprFrame.");
    free(bitsDaMensagem); // Libera a memória alocada para bitsDaMensagem
    return NULL;
  }

  int framePos = 0;

  // Adiciona flagStart ao quadro
  for (int i = 0; i < 8; i++) {
    oprFrame[framePos++] = flagStart[i];
  }

  // Adiciona vecAddress ao quadro
  for (int i = 0; i < 8; i++) {
    oprFrame[framePos++] = vecAddress[i];
  }

  // Adiciona controlField ao quadro
  for (int i = 0; i < 8; i++) {
    oprFrame[framePos++] = controlField[i];
  }

  // Adiciona os bits da mensagem ao quadro
  for (int i = 0; i < tamanhoMensagem * 8; i++) {
    oprFrame[framePos++] = bitsDaMensagem[i];
  }

  // Calcula e adiciona o CRC ao quadro
  uint32_t crcValue = calcularCRC(bitsDaMensagem, tamanhoMensagem * 8);
  for (int i = 0; i < 32; i++) {
    oprFrame[framePos++] = (crcValue >> (31 - i)) & 1;
  }

  // Adiciona FCS ao quadro
  //for (int i = 0; i < 8; i++) {
  //  oprFrame[framePos++] = 0; // Defina o FCS conforme necessário
  //}

  // Adiciona flagEnd ao quadro
  for (int i = 0; i < 8; i++) {
    oprFrame[framePos++] = flagEnd[i];
  }

  free(bitsDaMensagem); // Libera a memória alocada para bitsDaMensagem
  return oprFrame;
}

void setup() {
  Serial.begin(9600);
}

void loop() {
  String minhaMensagem = "oi";
  int* quadro = montarFrame(minhaMensagem);

  if (quadro != NULL) {
    // Exibindo os valores do quadro na porta serial para verificação
    int tamanhoFrame = 8 + 8 + tamanhoMensagem * 8 + 32 + 8 + 8 + 8;
    for (int i = 0; i < tamanhoFrame; i++) {
      Serial.write(quadro[i]);
      delay(TEMPO_BIT);
    }

    free(quadro); // Libera a memória alocada para quadro
  }
  delay(300000000);
}
