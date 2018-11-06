
#include <WiFi.h>
#include "StepperControl.h"
#include "BluetoothSerial.h"

const int stepsPerRevolution = 2048;

TaskHandle_t xHandleStepperMDE, xHandleStepperMDD, xHandleStepperMTD, xHandleStepperMTE, xHandleAlive, xHandleWifi;

//#define configTICK_RATE_HZ 1000
const int MDEIN1 = 26;
const int MDEIN2 = 25;
const int MDEIN3 = 33;
const int MDEIN4 = 32;
const int MDDIN1 = 19;
const int MDDIN2 = 21;
const int MDDIN3 = 22;
const int MDDIN4 = 23;
const int MTDIN1 = 15;
const int MTDIN2 = 4;
const int MTDIN3 = 5;
const int MTDIN4 = 18;
const int MTEIN1 = 13;
const int MTEIN2 = 12;
const int MTEIN3 = 14;
const int MTEIN4 = 27;


StepperControl StepperMDE(stepsPerRevolution, MDEIN1, MDEIN2, MDEIN3, MDEIN4);
StepperControl StepperMDD(stepsPerRevolution, MDDIN1, MDDIN2, MDDIN3, MDDIN4);
StepperControl StepperMTD(stepsPerRevolution, MTDIN1, MTDIN2, MTDIN3, MTDIN4);
StepperControl StepperMTE(stepsPerRevolution, MTEIN1, MTEIN2, MTEIN3, MTEIN4);

int LED_BUILTIN = 2;

int clienteconectado = 0, transferData = 0;

float ux = 0.0, uy = 0.0, theta = 0.0;
int Hz1 = 100, Hz2 = 100, Hz3 = 100, Hz4 = 100;
int atualizarContas = 0, enviarOdometriaClient = 0, enviarOdometriaSerial = 0, enviarOdometriaSerialBT = 0;
long odometriaDE = 0, odometriaDD = 0, odometriaTD = 0, odometriaTE = 0;
float WMDE, WMDD, WMTD, WMTE, HzDE, HzDD, HzTD, HzTE, velMaior, maiorValor, fatorHz = 1;
float R = 0.1795, L1 = 0.8, L2 = 0.653;

const char* ssid = "Robô-Omnidirecional";
const char *password = "ifes";
WiFiServer server(80);
BluetoothSerial SerialBT;
SemaphoreHandle_t xSemaphoreSwitch = NULL;
SemaphoreHandle_t xSemaphoreHz = NULL;

void setup() {


  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  Serial.println("Serial started");
  delay(100);

  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("");
  Serial.print("Acess Point created IP address: ");
  Serial.println(myIP);
  server.begin();
  Serial.println("Server started");

  SerialBT.begin("Robô-Omnidirecional");
  Serial.println("Serial Bluetooth started");

  StepperMDE.checkStepper();
  StepperMDD.checkStepper();
  StepperMTD.checkStepper();
  StepperMTE.checkStepper();

  xTaskCreate(taskWifi, "taskWifi", 2000, NULL, 30, &xHandleWifi);
  xTaskCreate(taskSerialCom, "taskSerialCom", 1000, NULL, 19, NULL);
  xTaskCreate(taskBluetooth, "taskBluetooth", 1000, NULL, 20, NULL);
  xTaskCreate(alive, "alive", 1000, NULL, 3, &xHandleAlive);
  xTaskCreate(taskStepperMDE, "taskStepperMDE", 1000, NULL, 100, &xHandleStepperMDE);
  xTaskCreate(taskStepperMDD, "taskStepperMDD", 1000, NULL, 100, &xHandleStepperMDD);
  xTaskCreate(taskStepperMTD, "taskStepperMTD", 1000, NULL, 100, &xHandleStepperMTD);
  xTaskCreate(taskStepperMTE, "taskStepperMTE", 1000, NULL, 100, &xHandleStepperMTE);
  xSemaphoreSwitch = xSemaphoreCreateMutex();
  xSemaphoreHz = xSemaphoreCreateMutex();
  //vTaskSuspend( xHandleStepperMDE );
  //vTaskSuspend( xHandleStepperMDD );
  //vTaskSuspend( xHandleStepperMTD );
  //vTaskSuspend( xHandleStepperMTE );
  StepperMDE.stopMotor();
  StepperMDD.stopMotor();
  StepperMTD.stopMotor();
  StepperMTE.stopMotor();

  Serial.println("");
  Serial.println("Robô Omnidirecional inicializado com sucesso!");
  Serial.println("");
}
WiFiClient client = server.available();
////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
  delay(1000);
}
////////////////////////////////////////////////////////////////////////////////////////////////////


void taskWifi( void * parameter )
{
  char incomingByte = NULL;
  const TickType_t xDelay = 100 / portTICK_PERIOD_MS;
  TickType_t xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();
  for (;;)
  {
    vTaskDelayUntil( &xLastWakeTime, xDelay );// aplica um delay com controle de atraso

    conectarClienteWIFI();

    if (client.connected())
    {
      while (client.available())
      {
        enviarOdometriaClient = 1;
        incomingByte = client.read();
        switchcase(incomingByte, 0);
      }
    }
    calcularHz();
    if (enviarOdometriaClient == 1) {

      client.write('#');
      client.println(odometriaDE);
      client.println(odometriaDD);
      client.println(odometriaTD);
      client.println(odometriaTE);
      client.write('!');
    }
    enviarOdometriaClient = 0;
  }

  Serial.println("Ending WIFI");
  vTaskDelete( NULL );
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
void taskSerialCom( void * parameter )
{
  char incomingByte = NULL;
  const TickType_t xDelay = 100 / portTICK_PERIOD_MS;
  TickType_t xLastWakeTime;

  xLastWakeTime = xTaskGetTickCount();
  for (;;)
  {
    vTaskDelayUntil( &xLastWakeTime, xDelay );// aplica um delay com controle de atraso

    while (Serial.available() > 0)
    {
      enviarOdometriaSerial = 1;
      incomingByte = Serial.read();
      switchcase(incomingByte, 1);
    }
    calcularHz();
    if (enviarOdometriaSerial == 1) {
      Serial.println('#');
      Serial.println(odometriaDE);
      Serial.println(odometriaDD);
      Serial.println(odometriaTD);
      Serial.println(odometriaTE);
      Serial.println('!');
    }
    enviarOdometriaSerial = 0;

  }
  Serial.println("Ending Serial COM");
  vTaskDelete( NULL );

}
////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

void taskBluetooth( void * parameter )
{
  char incomingByte = NULL;
  const TickType_t xDelay = 100 / portTICK_PERIOD_MS;
  TickType_t xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();
  for (;;)
  {
    vTaskDelayUntil( &xLastWakeTime, xDelay );// aplica um delay com controle de atraso

    while (SerialBT.available() > 0)
    {
      enviarOdometriaSerialBT = 1;
      incomingByte = SerialBT.read();
      switchcase(incomingByte, 2);
    }
    calcularHz();
    if (enviarOdometriaSerialBT == 1) {
      SerialBT.println('#');
      SerialBT.println(odometriaDE);
      SerialBT.println(odometriaDD);
      SerialBT.println(odometriaTD);
      SerialBT.println(odometriaTE);
      SerialBT.println('!');
    }
    enviarOdometriaSerialBT = 0;
  }
  Serial.println("Ending Bluetooth");
  vTaskDelete( NULL );

}
////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
void alive( void * parameter)
{
  for (;;)
  {
    if (transferData == 0)
    {
      if (clienteconectado == 0)
      {
        digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
        vTaskDelay(50);
        digitalWrite(LED_BUILTIN, LOW); // turn the LED off by making the voltage LOW
        vTaskDelay(4950);
      }
      else
      {
        digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
        vTaskDelay(50);
        digitalWrite(LED_BUILTIN, LOW); // turn the LED off by making the voltage LOW
        vTaskDelay(50);
        digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
        vTaskDelay(50);
        digitalWrite(LED_BUILTIN, LOW); // turn the LED off by making the voltage LOW
        vTaskDelay(850);
      }
    }
    else
    {
      digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
      vTaskDelay(50);
      digitalWrite(LED_BUILTIN, LOW); // turn the LED off by making the voltage LOW
      vTaskDelay(50);
    }
  }
  Serial.println("Ending alive");
  vTaskDelete( NULL );
}
////////////////////////////////////////////taskStepperMDE////////////////////////////////////////////////////////
////////////////////////////////////////////taskStepperMDE////////////////////////////////////////////////////////
////////////////////////////////////////////taskStepperMDE////////////////////////////////////////////////////////


void taskStepperMDE( void * parameter)
{
  int hertz = Hz1; // entrada de velocidade do motor em hertz
  int sentido = 1;
  float amostragem = 1000.0 / (abs(hertz) * 4.0); //conversão de hertz em amostragem
  TickType_t xDelay = amostragem / portTICK_PERIOD_MS;
  int cont = 1, limitMax = 0;
  TickType_t xLastWakeTime;

  xLastWakeTime = xTaskGetTickCount();
  for (;;)
  {
    vTaskDelayUntil( &xLastWakeTime, xDelay );// aplica um delay com controle de atraso
    hertz = HzDE; // velocidade em hertz desejada para o motor

    if (hertz != 0) {// proteção contra velocidades nulas

      if (hertz > 120)  //define limites de velocidade superior
        hertz = 120;

      if (hertz < -120) //define limites de velocidade inferior
        hertz = -120;

      if (hertz > 0) { //define o sentido de giro e salva a odometria
        sentido = 1;
        odometriaDE--;
      }
      else {
        sentido = -1;
        odometriaDE++;
      }

      StepperMDE.step(sentido); // alterna o chaveamento do bobinamento

      amostragem = 1000.0 / (abs(hertz) * 4.0); // encontra o valor em milissegundos que deve ser aplicado no delay

      limitMax = floor(10 * (amostragem - floor(amostragem))); // pega o valor da primeira casa decimal

      if (cont <= limitMax) // define os tempos de comutação do motor
        xDelay = floor(amostragem) + 1;//delay extra pra compensar o arredondamento
      else
        xDelay = floor(amostragem);

      cont++;
      if (cont > 10) // controla o limite do contador, máximo 10 interações
        cont = 1;
    }
    else {// protege o RTOS quando velocidade do motor for zero, amostragem tende a infinito
      StepperMDE.stopMotor(); // desenergiza o motor
      amostragem = 2;
      //vTaskSuspend( xHandleStepperMDE );
    }
  }
  StepperMDE.stopMotor(); // desenergiza o motor
  Serial.println("Motor 1 desligado"); // avisa pelo seria q a tarefa foi encerrada
  vTaskDelete( NULL );//deleta a tarefa e libera o espaço utilizado
}

////////////////////////////////////////////taskStepperMDD////////////////////////////////////////////////////////
////////////////////////////////////////////taskStepperMDD////////////////////////////////////////////////////////
////////////////////////////////////////////taskStepperMDD////////////////////////////////////////////////////////


void taskStepperMDD( void * parameter)
{

  int hertz = Hz2;
  int sentido = 1;
  float amostragem = 1000.0 / (abs(hertz) * 4.0);
  TickType_t xDelay = amostragem / portTICK_PERIOD_MS;
  int cont = 1, limitMax = 0;
  TickType_t xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();
  for (;;)
  {
    vTaskDelayUntil( &xLastWakeTime, xDelay );// aplica um delay com controle de atraso
    hertz = HzDD; // velocidade em hertz desejada par ao motor
    if (hertz != 0) {
      if (hertz > 120)  //define limites de velocidade
        hertz = 120;

      if (hertz < -120)
        hertz = -120;


      if (hertz > 0) { //define o sentido de giro e salva a odometria
        sentido = 1;
        odometriaDD++;
      }
      else {
        sentido = -1;
        odometriaDD--;
      }

      StepperMDD.step(sentido); // alterna a polaridade energizada nas bobinas do motor

      amostragem = 1000.0 / (abs(hertz) * 4.0); // encontra o valor em milisegundos que deve ser aplicado no delay

      limitMax = floor(10 * (amostragem - floor(amostragem))); //peda o valor da primeira casa decimal

      if (cont <= limitMax) // define os delais do motor
        xDelay = floor(amostragem) + 1;//delay extra pra compençar o arredondamento
      else
        xDelay = floor(amostragem);

      cont++;
      if (cont > 10) // controla o limite do contador, maximo 10 interaçoes
        cont = 1;
    }
    else {//proteje o rtos quando velocidade do motor for zero, amostrazem tende a infinito
      StepperMDD.stopMotor(); // desernegiza o motor
      amostragem = 2;
      //vTaskSuspend( xHandleStepperMDD);
    }
  }

  StepperMDD.stopMotor(); // desernegiza o motor
  Serial.println("Ending motor 2"); // avisa pelo seria q a tarefa foi encerrada
  vTaskDelete( NULL );//deleta a tarefa e libera o espaço utilizado
}

////////////////////////////////////////////taskStepperMTD////////////////////////////////////////////////////////
////////////////////////////////////////////taskStepperMTD////////////////////////////////////////////////////////
////////////////////////////////////////////taskStepperMTD////////////////////////////////////////////////////////


void taskStepperMTD( void * parameter)
{

  int hertz = Hz3;
  int sentido = 1;
  float amostragem = 1000.0 / (abs(hertz) * 4.0);
  TickType_t xDelay = amostragem / portTICK_PERIOD_MS;
  int cont = 1, limitMax = 0;
  TickType_t xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();
  for (;;)
  {
    vTaskDelayUntil( &xLastWakeTime, xDelay );// aplica um delay com controle de atraso
    hertz = HzTD; // velocidade em hertz desejada par ao motor
    if (hertz != 0) {
      if (hertz > 120)  //define limites de velocidade
        hertz = 120;

      if (hertz < -120)
        hertz = -120;


      if (hertz > 0) { //define o sentido de giro e salva a odometria
        sentido = 1;
        odometriaTD++;
      }
      else {
        sentido = -1;
        odometriaTD--;
      }

      StepperMTD.step(sentido); // alterna a polaridade energizada nas bobinas do motor

      amostragem = 1000.0 / (abs(hertz) * 4.0); // encontra o valor em milisegundos que deve ser aplicado no delay

      limitMax = floor(10 * (amostragem - floor(amostragem))); //peda o valor da primeira casa decimal

      if (cont <= limitMax) // define os delais do motor
        xDelay = floor(amostragem) + 1;//delay extra pra compençar o arredondamento
      else
        xDelay = floor(amostragem);

      cont++;
      if (cont > 10) // controla o limite do contador, maximo 10 interaçoes
        cont = 1;
    }
    else {//proteje o rtos quando velocidade do motor for zero, amostrazem tende a infinito
      StepperMTD.stopMotor(); // desernegiza o motor
      amostragem = 2;
      //vTaskSuspend( xHandleStepperMTD );
    }
  }

  StepperMTD.stopMotor(); // desernegiza o motor
  Serial.println("Ending motor 3"); // avisa pelo seria q a tarefa foi encerrada
  vTaskDelete( NULL );//deleta a tarefa e libera o espaço utilizado
}

////////////////////////////////////////////taskStepperMTE////////////////////////////////////////////////////////
////////////////////////////////////////////taskStepperMTE////////////////////////////////////////////////////////
////////////////////////////////////////////taskStepperMTE////////////////////////////////////////////////////////


void taskStepperMTE( void * parameter)
{
  int hertz = Hz4;
  int sentido = 1;
  float amostragem = 1000.0 / (abs(hertz) * 4.0);
  TickType_t xDelay = amostragem / portTICK_PERIOD_MS;
  int cont = 1, limitMax = 0;
  TickType_t xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();
  for (;;)
  {
    vTaskDelayUntil( &xLastWakeTime, xDelay );
    hertz = HzTE; // velocidade em hertz desejada par ao motor
    if (hertz != 0) {
      if (hertz > 120)  //define limites de velocidade
        hertz = 120;

      if (hertz < -120)
        hertz = -120;


      if (hertz > 0) { //define o sentido de giro e salva a odometria
        sentido = 1;
        odometriaTE--;
      }
      else {
        sentido = -1;
        odometriaTE++;
      }

      StepperMTE.step(sentido); // alterna a polaridade energizada nas bobinas do motor

      amostragem = 1000.0 / (abs(hertz) * 4.0); // encontra o valor em milisegundos que deve ser aplicado no delay

      limitMax = floor(10 * (amostragem - floor(amostragem))); //peda o valor da primeira casa decimal

      if (cont <= limitMax) // define os delais do motor
        xDelay = floor(amostragem) + 1;//delay extra pra compençar o arredondamento
      else
        xDelay = floor(amostragem);

      cont++;
      if (cont > 10) // controla o limite do contador, maximo 10 interaçoes
        cont = 1;
    }
    else {//proteje o rtos quando velocidade do motor for zero, amostrazem tende a infinito
      StepperMTE.stopMotor(); // desernegiza o motor
      amostragem = 2;
      //vTaskSuspend( xHandleStepperMTE );
    }
  }

  StepperMTE.stopMotor(); // desernegiza o motor
  Serial.println("Ending motor 4"); // avisa pelo seria q a tarefa foi encerrada
  vTaskDelete( NULL );//deleta a tarefa e libera o espaço utilizado
}

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

void conectarClienteWIFI(void)
{
  if (clienteconectado == 0)
  {
    client = server.available();   // listen for incoming clients
    if (client )
    {
      Serial.println("Cliente Conectado.");           // print a message out the serial port
      client.write('(');
      clienteconectado = 1;
    }
  }
}

void switchcase(char a, int tipo)
{
  if ( xSemaphoreSwitch != NULL )
  {
    if ( xSemaphoreTake( xSemaphoreSwitch, ( TickType_t ) 10 ) == pdTRUE )
    {

      switch (a)
      {
        case '1': vTaskResume(xHandleStepperMDE); break;
        case '2': vTaskResume(xHandleStepperMDD); break;
        case '3': vTaskResume(xHandleStepperMTD); break;
        case '4': vTaskResume(xHandleStepperMTE); break;
        case '!': vTaskSuspend( xHandleStepperMDE ); StepperMDE.stopMotor(); break;
        case '@': vTaskSuspend( xHandleStepperMDD ); StepperMDD.stopMotor(); break;
        case '#': vTaskSuspend( xHandleStepperMTD ); StepperMTD.stopMotor(); break;
        case '$': vTaskSuspend( xHandleStepperMTE ); StepperMTE.stopMotor(); break;
        case 'x':
          if (tipo == 0)ux = client.parseFloat();
          if (tipo == 1)ux = Serial.parseFloat();
          if (tipo == 2)ux = SerialBT.parseFloat();
          ux = ux * 100.0; atualizarContas = 1;
          break;
        case 'y':
          if (tipo == 0)uy = client.parseFloat();
          if (tipo == 1)uy = Serial.parseFloat();
          if (tipo == 2)uy = SerialBT.parseFloat();
          uy = uy * 100.0; atualizarContas = 1;
          break;
        case 't':
          if (tipo == 0)theta = client.parseFloat();
          if (tipo == 1)theta = Serial.parseFloat();
          if (tipo == 2)theta = SerialBT.parseFloat();
          theta = theta * 100.0; atualizarContas = 1;
          break;
        case 'd': client.write(')');clienteconectado = 0;Serial.println("Cliente Desconectado."); client.stop();  break;
        case 'b':
        StepperMDE.quantas_bobinas_ativadas(1);
        StepperMDD.quantas_bobinas_ativadas(1);
        StepperMTD.quantas_bobinas_ativadas(1);
        StepperMTE.quantas_bobinas_ativadas(1);
        break;
        case 'B':
        StepperMDE.quantas_bobinas_ativadas(2);
        StepperMDD.quantas_bobinas_ativadas(2);
        StepperMTD.quantas_bobinas_ativadas(2);
        StepperMTE.quantas_bobinas_ativadas(2);
        break;
        default: break;
      }

      xSemaphoreGive( xSemaphoreSwitch );
    }
    else
    {
      Serial.println("0x0000");
    }
  }
}

void calcularHz(void)
{
  if (atualizarContas == 1)
  {
    atualizarContas = 0;
    if ( xSemaphoreHz != NULL )
    {
      if ( xSemaphoreTake( xSemaphoreHz, ( TickType_t ) 10 ) == pdTRUE )
      {
        WMDE = (1 / R) * (- ux + uy + (L1 + L2) * theta);
        WMDD = (1 / R) * (  ux + uy + (L1 + L2) * theta);
        WMTD = (1 / R) * (  ux - uy + (L1 + L2) * theta);
        WMTE = (1 / R) * (- ux - uy + (L1 + L2) * theta);

        maiorValor = abs(WMDE);
        if (abs(WMDD) > maiorValor)maiorValor = abs(WMDD);
        if (abs(WMTD) > maiorValor)maiorValor = abs(WMTD);
        if (abs(WMTE) > maiorValor)maiorValor = abs(WMTE);

        if (maiorValor != 0)
        {
          WMDE = WMDE / maiorValor;
          WMDD = WMDD / maiorValor;
          WMTD = WMTD / maiorValor;
          WMTE = WMTE / maiorValor;

          velMaior = abs(ux);
          if (abs(uy) > velMaior)velMaior = abs(uy);
          if (abs(theta) > velMaior)velMaior = abs(theta);

          HzDE = WMDE * velMaior * fatorHz;
          HzDD = WMDD * velMaior * fatorHz;
          HzTD = WMTD * velMaior * fatorHz;
          HzTE = WMTE * velMaior * fatorHz;

          // if (StepperMDE.statusMotor() == 0 && HzDE != 0)vTaskResume(xHandleStepperMDE);
          // if (StepperMDD.statusMotor() == 0 && HzDD != 0)vTaskResume(xHandleStepperMDD);
          // if (StepperMTD.statusMotor() == 0 && HzTD != 0)vTaskResume(xHandleStepperMTD);
          // if (StepperMTE.statusMotor() == 0 && HzTE != 0)vTaskResume(xHandleStepperMTE);
        }
        else
        {
          HzDE = 0;
          HzDD = 0;
          HzTD = 0;
          HzTE = 0;
        }

        xSemaphoreGive( xSemaphoreHz );
      }
      else
      {
        Serial.println("0x0001");
      }
    }
  }
}

