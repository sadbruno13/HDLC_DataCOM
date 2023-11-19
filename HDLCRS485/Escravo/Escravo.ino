// --------- Flag Inicio/Fim --------- //
const uint8_t StartEndFLAG = 0x7E;
bool flagStartEnd = false;
// ---------------------------------- //

// ------------ Endereços Escravos ------------ //
const byte ID = 1;
const byte BROADCAST = 3;
const byte enderecoMestre = 4;
// -------------------------------------------- //

// -------------- Dados Recebidos --------------- //
uint16_t idRecebido; // Endereço recebido
uint16_t controlFieldRecebido; // Controle recebido
String dadosMensagem = "";
uint16_t fcsRecebido; // FCS recebido
// --------------------------------------------- //


// ----------ControlField------ // Falta adicionar a parte lógica, para realmente utilizar o ControlField
const byte CONTROLFIELD = 1;
//const uint16_t CONTROL = 2; 
//const uint16_t CONTROL = 3; 
//const uint16_t CONTROL = 4;
// --------------------------- //

// -------------- Mensagens -------------- //
const char* mensagemMestre = "Ola mundo";
// -------------------------------------- //

// ------------- FCS -------------------- //
uint16_t FCS;
// -------------------------------------- //

// -------- Resposta Escravos  ---------- //
const char* msgOK = "1";
bool OK = false;
const char* msgNOK = "2";
bool NOK = false;
// ------------------------------------------- //


void setup() {
  Serial.begin(9600);
}

void loop() {
  receberFrame();
}
void receberFrame(){

  while(Serial.available() > 0){
    //Recebendo o byte, necessita verificar se este é igual a Flag de Inicio --- Começo de tudo
    char dadoRecebido = receberByte();

    if(dadoRecebido == StartEndFLAG){
      //Aqui já estamos recebendo a mensagem, precisamos garantir pro código uma maneira dele entender que já está na mensagem
      //Por isso, deu-se a criação da flagStart, ele verifica se entramos na mensagem, inicia-se com false e muda de status assim que a Flag de inicio é detectada
      if(flagStartEnd){
        //Se o código tiver fluído corretamente, neste momento, nós iremos possuir o Adress + ControlField + Data + FCS, todos os dados necessários para encontrar a mensagem
        //Como ainda não implementamos uma lógica pro ControlField, ele será inicialmente fixo, no momento, não fará tanta diferença
        //Ele ainda faz parte da nossa Header, juntamente do Endereço do escravo
        //Ambos possuem 1 byte, sendo 2 bytes (16 bits) no total.
        if(dadosMensagem.length() >= 2){
          const byte idEscravo = dadosMensagem[0]; //Selecionamos o ID do Escravo
          if( idEscravo == ID || idEscravo == BROADCAST){
            byte controlField = dadosMensagem[1];
            // Para encontrarmos a mensagem, primeiramente, necessitamos entender onde ela está posicionada
            // Header é composta por Endereço + Controlfield --> Totalizando 2 bytes
            // Nossa mensagem não possui tamanho fixo, por isso, pode variar o tamanho, porém, sabemos que ao final da nossa mensagem, teremos o FCS, que inicialmente setamos em 16 bits, ou 2 bytes.
            String mensagem = dadosMensagem.substring(2, dadosMensagem.length()-2);
            
            fcsRecebido = (dadosMensagem[dadosMensagem.length() - 2] << 8) | dadosMensagem[dadosMensagem.length() - 1]; // Separa p FCS recebido

            //Agora necessitamos validar nosso FCS, só assim poderemos saber a integridade da nossa mensagem
            bool validado = ValidarFCS(controlField, idEscravo, mensagem, fcsRecebido);
            if(validado){
              printDadosRecebidos(idEscravo, controlField, mensagem);
              //Reenviar confirmação de recebimento correto pro Mestre
              if(!OK && !NOK){
                EnviarOK(enderecoMestre, CONTROLFIELD, msgOK);
              }
            }
            else{
              EnviarNOK(enderecoMestre, CONTROLFIELD, msgNOK);
            }
          }
        }
        flagStartEnd = false; // Reinicia o processo, pois detectou a Flag de final
     }
      else{
        flagStartEnd = true;
        dadosMensagem = ""; //
      }
    }
    else if(flagStartEnd){
      //Em C++, uma string é, na verdade, um array de caracteres (char). O tipo de dado byte é equivalente a unsigned char, e ambos são tratados como caracteres em muitos contextos em C++.
      //Quando você faz dadosMensagem += dadoRecebido;, o compilador está realizando uma conversão implícita de byte para char. Isso acontece porque o operador += está definido para strings e 
      //é capaz de lidar com caracteres (char). O byte é basicamente um tipo de dado de 8 bits sem sinal, o que é o mesmo tamanho que um char.
      //Portanto, adicionar um byte a uma string funciona porque a linguagem C++ permite essa conversão automática entre tipos relacionados. Se você estivesse usando um array de bytes (uint8_t), 
      //não precisaria dessa conversão explícita, pois uint8_t e char são tipos de dados equivalentes.
      dadosMensagem += dadoRecebido;
      Serial.println(dadosMensagem);
    }
    
  }
  
}

byte receberByte(){
  byte dadoRecebido = Serial.read();
  return dadoRecebido;
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

bool ValidarFCS(uint8_t controlField, uint8_t endereco, String mensagem, uint16_t fcsRecebido){
  const char* mensagemChar = mensagem.c_str();
  uint16_t fcsCalculado = calculateFCS(controlField, endereco, mensagemChar);
  if (fcsCalculado == fcsRecebido){
    return true;
  }
  return false;
}

void printDadosRecebidos(byte ID, byte controlField, String mensagem) {
  Serial.print("Endereco do Escravo: ");
  Serial.println(ID);
  Serial.print("Mensagem Recebida: ");
  Serial.println(mensagem);
}

void EnviarOK(byte endereco, byte controlField, const char* mensagem){
  Serial.write(StartEndFLAG);
  Serial.write(endereco);
  Serial.write(controlField);
  Serial.write(mensagem);
  Serial.flush();

  FCS = calculateFCS(controlField, endereco, mensagem);
  EnviarFCSSerial(FCS); // Por ser 16 bits, necessitou dividir e enviar separadamente

  Serial.write(StartEndFLAG);

}

void EnviarNOK(byte endereco, byte controlField, const char* mensagem){
  Serial.write(StartEndFLAG);
  Serial.write(endereco);
  Serial.write(controlField);
  Serial.write(mensagem);
  Serial.flush();

  FCS = calculateFCS(controlField, endereco, mensagem);
  EnviarFCSSerial(FCS); // Por ser 16 bits, necessitou dividir e enviar separadamente

  Serial.write(StartEndFLAG);

}

void EnviarFCSSerial(uint16_t FCS){
  uint8_t lsb = FCS & 0xFF;         // byte menos significativo
  uint8_t msb = (FCS >> 8) & 0xFF;  // byte mais significativo
  Serial.write(msb); //Envia pelo mais significativo
  Serial.write(lsb); //Envia o menos significativo
}
