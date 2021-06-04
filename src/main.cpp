#include <Arduino.h>
#include <AWL.h>
#include <IntervalSwitch.h>
#include <AWLServer.h>
#include <Wire.h>

#define VERSION "1.0.0"

#define NIVEL1_PIN 9
#define NIVEL2_PIN 10
#define NIVEL_RESERVOIR_PIN 11
#define RELE_PIN 12

#define BUZZER_ENABLED_PIN A0
#define BUZZER_SOUND_PIN A1
#define RELE_TIMEOUT_PIN A2

void levelToZeroCommandCallback(acuaris::CommandData *data);
void levelToFullCommandCallback(acuaris::CommandData *data);
void releTimeOutCommandCallback(acuaris::CommandData *data);
void histeresisCommandCallback(acuaris::CommandData *data);
void maintenanceCommandCallback(acuaris::CommandData *data);

acuaris::ResponseData statusMainLevelQueryCallback();
acuaris::ResponseData statusSafetyLevelQueryCallback();
acuaris::ResponseData statusReservoirLevelQueryCallback();
acuaris::ResponseData statusReleQueryCallback();
acuaris::ResponseData PercentLevelQueryCallback();
acuaris::ResponseData DistanceLevelQueryCallback();
acuaris::ResponseData statusMainQueryCallback();
acuaris::ResponseData settingZeroLevelQueryCallback();
acuaris::ResponseData settingFullLevelQueryCallback();
acuaris::ResponseData releTimeOutQueryCallback();
acuaris::ResponseData histeresisQueryCallback();
acuaris::ResponseData maintenanceQueryCallback();

void printStatus(bool *on);

acuaris::awl::AWL awl = acuaris::awl::AWL(NIVEL1_PIN, NIVEL2_PIN, NIVEL_RESERVOIR_PIN, RELE_PIN);

acuaris::AWLServer awlServer;
acuaris::GenericAWLCommandHandler levelToZeroCommand(20, levelToZeroCommandCallback);
acuaris::GenericAWLCommandHandler levelToFullCommand(21, levelToFullCommandCallback);
acuaris::GenericAWLCommandHandler releTimeOutCommand(22, releTimeOutCommandCallback);
acuaris::GenericAWLCommandHandler histeresisCommand(23, histeresisCommandCallback);
acuaris::GenericAWLCommandHandler maintenanceCommand(24, maintenanceCommandCallback);

acuaris::GenericAWLQueryHandler statusMainLevelQuery(1, statusMainLevelQueryCallback);
acuaris::GenericAWLQueryHandler statusSafetyLevelQuery(2, statusSafetyLevelQueryCallback);
acuaris::GenericAWLQueryHandler statusReservoirLevelQuery(3, statusReservoirLevelQueryCallback);
acuaris::GenericAWLQueryHandler statusReleQuery(4, statusReleQueryCallback);
acuaris::GenericAWLQueryHandler PercentLevelQuery(5, PercentLevelQueryCallback);
acuaris::GenericAWLQueryHandler DistanceLevelQuery(6, DistanceLevelQueryCallback);
acuaris::GenericAWLQueryHandler statusMainQuery(7, statusMainQueryCallback);
acuaris::GenericAWLQueryHandler settingZeroLevelQuery(20, settingZeroLevelQueryCallback);
acuaris::GenericAWLQueryHandler settingFullLevelQuery(21, settingFullLevelQueryCallback);
acuaris::GenericAWLQueryHandler releTimeOutQuery(22, releTimeOutQueryCallback);
acuaris::GenericAWLQueryHandler histeresisQuery(23, histeresisQueryCallback);
acuaris::GenericAWLQueryHandler maintenanceQuery(24, maintenanceQueryCallback);


int printOn = 1;
int printOff = 2000;
acuaris::utils::IntervalSwitch printStatusInterval(&printOn, &printOff, printStatus);

void setup()
{
  Serial.begin(9600);
  Serial.println("Init AWL System...");
  awl.begin();

  Serial.println("Init I2C Comunication...");
  
  awlServer.addCommandHandler(levelToZeroCommand);
  awlServer.addCommandHandler(levelToFullCommand);
  awlServer.addCommandHandler(releTimeOutCommand);
  awlServer.addCommandHandler(histeresisCommand);
  awlServer.addCommandHandler(maintenanceCommand);

  awlServer.addQueryHandler(statusMainLevelQuery);
  awlServer.addQueryHandler(statusSafetyLevelQuery);
  awlServer.addQueryHandler(statusReservoirLevelQuery);
  awlServer.addQueryHandler(statusReleQuery);
  awlServer.addQueryHandler(PercentLevelQuery);
  awlServer.addQueryHandler(DistanceLevelQuery);
  awlServer.addQueryHandler(statusMainQuery);
  awlServer.addQueryHandler(settingZeroLevelQuery);
  awlServer.addQueryHandler(settingFullLevelQuery);
  awlServer.addQueryHandler(releTimeOutQuery);
  awlServer.addQueryHandler(histeresisQuery);
  awlServer.addQueryHandler(maintenanceQuery);

  awlServer.begin();
}

void loop()
{
  printStatusInterval.loop();
  awl.checkLevel();
}

void levelToZeroCommandCallback(acuaris::CommandData *data)
{
  if (data->size() != 2)
    return;
  int mm = data->operator[](0) | data->operator[](1) << 8;
  awl.reservoirZeroSetter(mm);
}
void levelToFullCommandCallback(acuaris::CommandData *data)
{
  if (data->size() != 2)
    return;
  int mm = data->operator[](0) | data->operator[](1) << 8;
  awl.reservoirFullSetter(mm);
}
void releTimeOutCommandCallback(acuaris::CommandData *data)
{
  if (data->size() != 2)
    return;
  int seconds = data->operator[](0) | data->operator[](1) << 8;
  awl.runStop(seconds);
}
void histeresisCommandCallback(acuaris::CommandData *data)
{
  if (data->size() != 2)
    return;
  int ms = data->operator[](0) | data->operator[](1) << 8;
  awl.histeresisSetter(ms);
}
void maintenanceCommandCallback(acuaris::CommandData *data)
{
  if (data->size() != 1)
    return;

  bool maintenance = false;
  if (1 == data->operator[](0))
  {
    maintenance = true;
  }
  awl.maintenance(maintenance);
}

acuaris::ResponseData statusMainLevelQueryCallback() 
{
  acuaris::ResponseData data = awlServer.emptyResponseData();
  data.push_back(awl.getStLevel1());
  return data;
}
acuaris::ResponseData statusSafetyLevelQueryCallback()
{
  acuaris::ResponseData data = awlServer.emptyResponseData();
  data.push_back(awl.getStLevel2());
  return data;
}
acuaris::ResponseData statusReservoirLevelQueryCallback()
{
  acuaris::ResponseData data = awlServer.emptyResponseData();
  data.push_back(awl.getStReservoirLevel1());
  return data;
}
acuaris::ResponseData statusReleQueryCallback()
{
  acuaris::ResponseData data = awlServer.emptyResponseData();
  data.push_back(awl.isRunning());
  return data;
}
acuaris::ResponseData PercentLevelQueryCallback()
{
  acuaris::ResponseData data = awlServer.emptyResponseData();
  uint16_t value = awl.levelPercent();
  data.push_back(lowByte(value));
  data.push_back(highByte(value));
  return data;
}
acuaris::ResponseData DistanceLevelQueryCallback()
{
  Serial.println("Distance: ");
  acuaris::ResponseData data = awlServer.emptyResponseData();
  uint16_t value = awl.levelRaw();
  data.push_back(lowByte(value));
  data.push_back(highByte(value));
  return data;
}
acuaris::ResponseData statusMainQueryCallback()
{
  Serial.print("Status :");
  acuaris::ResponseData data = awlServer.emptyResponseData();
  uint16_t value = awl.status();
  Serial.println(value);
  Serial.print(lowByte(value));
  Serial.print("");
  Serial.println(highByte(value));
  data.push_back(lowByte(value));
  data.push_back(highByte(value));
  return data;
}
acuaris::ResponseData settingZeroLevelQueryCallback()
{
  Serial.println("Zero Setting: ");
  acuaris::ResponseData data = awlServer.emptyResponseData();
  uint16_t value = awl.reservoirZeroSetter();
  data.push_back(lowByte(value));
  data.push_back(highByte(value));
  return data;
}
acuaris::ResponseData settingFullLevelQueryCallback()
{
  acuaris::ResponseData data = awlServer.emptyResponseData();
  uint16_t value = awl.reservoirFullSetter();
  data.push_back(lowByte(value));
  data.push_back(highByte(value));
  return data;
}
acuaris::ResponseData releTimeOutQueryCallback()
{
  acuaris::ResponseData data = awlServer.emptyResponseData();
  uint16_t value = awl.runStop();
  data.push_back(lowByte(value));
  data.push_back(highByte(value));
  return data;
}
acuaris::ResponseData histeresisQueryCallback()
{
  acuaris::ResponseData data = awlServer.emptyResponseData();
  uint16_t value = awl.histeresisSetter();
  data.push_back(lowByte(value));
  data.push_back(highByte(value));
  return data;
}
acuaris::ResponseData maintenanceQueryCallback()
{
  acuaris::ResponseData data = awlServer.emptyResponseData();
  data.push_back(awl.maintenance());
  return data;
}

void printStatus(bool *on) {
  if(*on) {
    Serial.println(awl.getPrint());
  }
}