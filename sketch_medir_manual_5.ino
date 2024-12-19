#include <BluetoothSerial.h>
#include <SPIFFS.h>

BluetoothSerial SerialBT; // Objeto Bluetooth Serial

int valorLido1 = 0;
int valorLido2 = 0;
int i = 0;
float total = 0;

#define botao 14 // 35 cinza, 32 roxo, 33 azul, 25 verde, 26 amarelo, 27 laranja, 14 vemelho
#define led 27
#define pinpot1 26
#define pinpot2 25
#define comandoLimparPin 32   // Pino para o comando de limpar a memória
#define comandoBaixarPin 33   // Pino para o comando de baixar todos os valores

bool botaoPressionado = false;
bool comandoLimparPressionado = false;
bool comandoBaixarPressionado = false;

void appendFile(float total) {
 // File file = SPIFFS.open("/dados.txt", FILE_APPEND);
  File file = SPIFFS.open("/dados.txt", FILE_APPEND);
  if (!file) {
    Serial.println("Falha ao abrir o arquivo");
    SerialBT.println("Falha ao abrir o arquivo");
    return;
  }
   //file.print(data);
   //file.print(" ");   // se quiser os dados em linha usar estes dois prints
   file.println(total);  // esse print coloca os dados em coluna 
  
  file.close();
}

void readFile() {
  File file = SPIFFS.open("/dados.txt", FILE_READ);
  if (!file) {
    Serial.println("Falha ao abrir o arquivo");
    SerialBT.println("Falha ao abrir o arquivo");
    return;
  }

  Serial.println("Dados armazenados:");
  SerialBT.println("Dados armazenados:");
  while (file.available()) {
    String line = file.readStringUntil('\n');
    Serial.println(line);
    SerialBT.println(line);
  }
  //Serial.println("");
  //SerialBT.println("");
  file.close();
}

void deleteFile() {
  if (SPIFFS.remove("/dados.txt")) {
    Serial.println("Arquivo apagado com sucesso");
    SerialBT.println("Arquivo apagado com sucesso");
  } else {
    Serial.println("Falha ao apagar o arquivo");
    SerialBT.println("Falha ao apagar o arquivo");
  }
}

void setup() {
  pinMode(botao, INPUT_PULLUP);
  pinMode(led, OUTPUT);
  pinMode(comandoLimparPin, INPUT_PULLUP);
  pinMode(comandoBaixarPin, INPUT_PULLUP);

  Serial.begin(9600);
  SerialBT.begin("MEDITRIX-v5"); // Inicializa o Bluetooth e define o nome do dispositivo
  Serial.println("Bluetooth está pronto!");
  SPIFFS.begin(); // Inicializa a memória flash

  // Verifica se o arquivo de dados existe
  if (!SPIFFS.exists("/dados.txt")) {
    // Se o arquivo não existe, cria um novo arquivo vazio
    File file = SPIFFS.open("/dados.txt", FILE_WRITE);
    file.close();
  }
}

void loop() {
  while (i <= 30) {
    valorLido1 = analogRead(pinpot1);
    valorLido2 = analogRead(pinpot2);
    total += valorLido1 + valorLido2;
    i++;
    delay(8);
  }
  
  total /= 30.0;
  total -= 590;
   Serial.println(total); // este print serve para aferir o medidor
   // SerialBT.println(total); // este print serve para aferir o medidor
  if (total <= 3) {
    total = (total * 0.014);
  } 
  else if (total > 3 && total <= 4800) {
    total = (total / 220);
  } 
  else if (total > 4800 && total <= 6400) {
    total = (total / 225);
  }  
  else if (total > 6400 && total <= 8144) {
    total = (total / 230);
  } 
  else {
    SerialBT.println("Erro de leitura");
    Serial.println("Erro de leitura");
  }

  i = 0;
  

  if (SerialBT.available() || digitalRead(botao) == LOW || HIGH || digitalRead(comandoLimparPin) == LOW || HIGH || digitalRead(comandoBaixarPin) == HIGH ||LOW) {
    char command = SerialBT.read();

    // Verifica se o botão foi pressionado
    if (digitalRead(botao) == LOW && !botaoPressionado || command == '1') {
      botaoPressionado = true;
      appendFile(total);
      digitalWrite(led, HIGH);
      SerialBT.println("*** Medida Gravada ***");
      SerialBT.println("        " + String(total));
      Serial.println("*** Medida Gravada ***");
      Serial.println("        " + String(total));

      delay(40);
      digitalWrite(led, LOW);
    } else if (digitalRead(botao) == HIGH && botaoPressionado || command == '1') {
      botaoPressionado = false;
    }

    // Verifica se o comando de limpar foi pressionado
    if (digitalRead(comandoLimparPin) == LOW && !comandoLimparPressionado || command == '2') {
      deleteFile();
      comandoLimparPressionado = true;
      delay(40);
    } else if (digitalRead(comandoLimparPin) == HIGH && comandoLimparPressionado || command == '2') {
      comandoLimparPressionado = false;
    }

    // Verifica se o comando de baixar foi pressionado
    if (digitalRead(comandoBaixarPin) == LOW && !comandoBaixarPressionado || command == '3') {
      readFile();
      comandoBaixarPressionado = true;
      delay(40);
    } else if (digitalRead(comandoBaixarPin) == HIGH && comandoBaixarPressionado || command == '3') {
      comandoBaixarPressionado = false;
    }
  }
}
