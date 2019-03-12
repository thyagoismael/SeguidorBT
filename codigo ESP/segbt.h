#ifndef SEGBT_H
#define SEGBT_H

#include "BluetoothSerial.h"

typedef struct dadosParaExecutarControleManual
{
  bool controleManualAtivo;
  bool paraFrente, paraTras;
  bool setaDireita, setaEsquerda;
  bool curvaAutomatica;
}contrManual;

typedef struct dadosTransmitidosPeloBT
{
  int velocAtual,velocReta,velocCurva;
  int tempoCPU, tempoVolta;
  int KP, KI, KD;
  float PID, erro, bateria;
  int tempoEnvio;
  char tipoPista;
  contrManual manual;
}seguidorBT;


BluetoothSerial SerialBT; // instancia para transmissao bluetooth
seguidorBT dadosBT; // variavel que sera salvo todos os dados a serem transmitidos



void trasmissaoBluetooth(void *z)
{
//  Serial.println("Task criada");
  while(true)
  {
    receberDadosBT();
    enviarDadosBT();
  }
}


void receberDadosBT(void)
{
  String strVariavel = "", strValor = "";
  
  lerEntrada(&strVariavel);
  lerEntrada(&strValor);

//  Serial.println("Comando recebido =====");
//  Serial.println("Variavel: " + strVariavel);
//  Serial.println("Valor: " + strValor);
//  Serial.println("======================");

  if(!strVariavel.compareTo("VR"))
    dadosBT.velocReta = strValor.toInt();
  if(!strVariavel.compareTo("VC"))
    dadosBT.velocCurva = strValor.toInt();
  if(!strVariavel.compareTo("KP"))
    dadosBT.KP = strValor.toInt();
  if(!strVariavel.compareTo("KI"))
    dadosBT.KI = strValor.toInt();
  if(!strVariavel.compareTo("KD"))
    dadosBT.KD = strValor.toInt();
  if(!strVariavel.compareTo("TM"))
    dadosBT.tempoEnvio = strValor.toInt();

//  controle manual
  if(!strVariavel.compareTo("CMA"))
    dadosBT.manual.controleManualAtivo = true;
  if(!strVariavel.compareTo("CMI"))
    dadosBT.manual.controleManualAtivo = false;
  if(!strVariavel.compareTo("CA"))
    dadosBT.manual.curvaAutomatica = true;
  if(!strVariavel.compareTo("SE"))
    dadosBT.manual.setaEsquerda = true;
  if(!strVariavel.compareTo("SD"))
    dadosBT.manual.setaDireita = true;
  if(!strVariavel.compareTo("FT"))
    dadosBT.manual.paraFrente = true;
  if(!strVariavel.compareTo("TS"))
    dadosBT.manual.paraTras = true;
}

void enviarDadosBT(void)
{
  static int tempoAtual = millis();
  
  if(millis() - tempoAtual < dadosBT.tempoEnvio)
    return;

  SerialBT.print("VA:" + String(dadosBT.velocAtual) + ":");
  SerialBT.print("VR:" + String(dadosBT.velocReta) + ":");
  SerialBT.print("VC:" + String(dadosBT.velocCurva) + ":");
  SerialBT.print("KP:" + String(dadosBT.KP) + ":");
  SerialBT.print("KI:" + String(dadosBT.KI) + ":");
  SerialBT.print("KD:" + String(dadosBT.KD) + ":");
  SerialBT.print("TV:" + String(dadosBT.tempoVolta) + ":");
  SerialBT.print("TC:" + String(dadosBT.tempoCPU) + ":");
  SerialBT.print("TP:" + String(dadosBT.tipoPista) + ":");
  SerialBT.print("ER:" + String(dadosBT.erro) + ":");
  SerialBT.print("PI:" + String(dadosBT.PID) + ":");
  SerialBT.print("BT:" + String(dadosBT.bateria) + ":");

  tempoAtual = millis();
}

void lerEntrada(String *strEntrada)
{
  char novoChar;

  while(SerialBT.available())
  {
    novoChar = SerialBT.read();
    if(novoChar == ':')
      return;
    *strEntrada += novoChar;
  }
}

#endif
