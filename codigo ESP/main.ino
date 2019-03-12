#include "TridentTD_analogWrite32.h"
#include "segbt.h"
#include "defines.h"

void setup()
{
  SerialBT.begin("Seguidor");
  Serial.begin(115200);
  
  pinMode(MOTOR_E1, OUTPUT);
  pinMode(MOTOR_E2, OUTPUT);
  pinMode(MOTOR_D1, OUTPUT);
  pinMode(MOTOR_D2, OUTPUT);
  
  iniciarValores();

  xTaskCreatePinnedToCore(
                          trasmissaoBluetooth,
                          "Trasmissao",
                          50000,
                          NULL,
                          1,
                          NULL,
                          0);

}

void loop()
{
//  calcularPID();
  movimentarRobo();
//  debugBT();
}

void debugBT(void)
{
  static int tempoAtual = millis();
  
  if(millis() - tempoAtual < 500)
    return;
    
  Serial.println("Variaveis do ESP ==========");
  Serial.println("Veloc Atual: " + String(dadosBT.velocAtual));
  Serial.println("Veloc Reta: " + String(dadosBT.velocReta));
  Serial.println("Veloc Curva: " + String(dadosBT.velocCurva));
  Serial.println("Tempo CPU: " + String(dadosBT.tempoCPU));
  Serial.println("Tempo Volta: " + String(dadosBT.tempoVolta));
  Serial.println("KP: " + String(dadosBT.KP));
  Serial.println("KI: " + String(dadosBT.KI));
  Serial.println("KD: " + String(dadosBT.KD));
  Serial.println("PID: " + String(dadosBT.PID));
  Serial.println("Erro: " + String(dadosBT.erro));
  Serial.println("Bateria: " + String(dadosBT.bateria));
  Serial.println("Tipo de Pista: " + String(dadosBT.tipoPista));
  Serial.println("Tempo Envio: " + String(dadosBT.tempoEnvio));
  Serial.println("===========================");

  tempoAtual = millis();
}

void iniciarValores(void)
{
  dadosBT.velocAtual = 100;
  dadosBT.velocReta = VBASE_RETA;
  dadosBT.velocCurva = VBASE_CURVA;
  dadosBT.tempoVolta = 100;
  dadosBT.tempoCPU = 100;
  dadosBT.KP = 110;
  dadosBT.KI = 100;
  dadosBT.KD = 100;
  dadosBT.tipoPista = 'R';
  dadosBT.erro = 100;
  dadosBT.PID = 100;
  dadosBT.tempoEnvio = 100;
  dadosBT.bateria = 100;
}

void movimentarRobo(void)
{
  if(dadosBT.manual.controleManualAtivo)
  {
    controleManual();
    return;
  }
  controleAutomatico('1', 1);
}

void controleManual(void)
{
  int velocBase = 0;
  
  if(dadosBT.manual.paraFrente)
    velocBase = dadosBT.velocReta;
  if(dadosBT.manual.paraTras)
  {
    velocBase = -dadosBT.velocReta;
    dadosBT.PID = -dadosBT.PID;
  }

  if(dadosBT.manual.curvaAutomatica)
  {
    acionarMotorDireito(velocBase + dadosBT.PID);
    acionarMotorEsquerdo(velocBase - dadosBT.PID);
    return;
  }

  if(dadosBT.manual.setaEsquerda)
  {
    acionarMotorDireito(velocBase + DESVIO_MANUAL);
    acionarMotorEsquerdo(velocBase - DESVIO_MANUAL);
    return;
  }
  if(dadosBT.manual.setaDireita)
  {
    acionarMotorDireito(velocBase - DESVIO_MANUAL);
    acionarMotorEsquerdo(velocBase + DESVIO_MANUAL);
    return;
  }
}

void controleAutomatico(char marc, int pid)
{
  int Vbase = VBASE_CURVA;

  if (marc == 'R' || marc == 'T')
    Vbase = VBASE_RETA;

  acionarMotorDireito(Vbase + pid);
  acionarMotorEsquerdo(Vbase + pid);
}

void acionarMotorDireito(int potencia)
{
  limitarPotencia(&potencia);
  if (!DEBUG)
  {
    if (potencia < 0) {
      digitalWrite(MOTOR_D1, LOW);
      TridentTD::analogWrite(MOTOR_D2, abs(potencia));
    }
    else {
      digitalWrite(MOTOR_D2, LOW);
      TridentTD::analogWrite(MOTOR_D1, abs(potencia));
    }
  }
}

void acionarMotorEsquerdo(int potencia)
{
  limitarPotencia(&potencia);
  if (!DEBUG)
    if (potencia < 0) {
      digitalWrite(MOTOR_E1, LOW);
      TridentTD::analogWrite(MOTOR_E2, abs(potencia));
    }
    else {
      digitalWrite(MOTOR_E2, LOW);
      TridentTD::analogWrite(MOTOR_E1, abs(potencia));
    }
}

/* limitador de potencia*/
void limitarPotencia(int *potencia)
{
  if (*potencia >= VMAX)
    *potencia = VMAX;
  if (*potencia <= VMIN)
    *potencia = VMIN;
}
