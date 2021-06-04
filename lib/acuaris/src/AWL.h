#ifndef AWL_h
#define AWL_h

#include <Arduino.h>
#include <IntervalSwitch.h>
//#include <NewPing.h> //TODO:Eliminar libreria;

#include <EEPROM.h>

#define MAINTENANCE_PIN 2
#define BUZZER_PIN 4
#define WORKING_LED_PIN 5
#define ERROR_LED_PIN 6

#define HS_TRIGGER_PIN 7
#define HS_ECHO_PIN 8

const uint32_t MEMORY_VERSION = 1;

const bool BUZZER_DEFAULT = true;
const bool BUZZER_SOUND_DEFAULT = false;
const int TIMEOUT_DEFAULT = 12;
const int HISTERESIS_DEFAULT = 0;
const int ADJUST_ZERO_DEFAULT = 0;
const int ADJUST_FULL_DEFAULT = 600;

namespace acuaris
{
    namespace awl
    {
        void actionWorkLed(bool *on);
        void actionErrorLed(bool *on);
        void actionBuzzer(bool *on);

        utils::IntervalSwitch workingLedInterval;
        utils::IntervalSwitch errorLedInterval;
        utils::IntervalSwitch buzzerInterval;

        struct MemoryStorage
        {
            uint32_t memoryVersion = 0;
            uint16_t levelZeroSetter = 0;
            uint16_t levelFullSetter = 0;
            uint16_t timeOutRele = 6;
            uint16_t histeresis = 0;
        };

        MemoryStorage store;
        

        class AWL
        {
        protected:
            static AWL *_instance;
            int _pinLevel1;
            int _pinLevel2;
            int _pinReservoirLevel1;
            int _pinPump;
            bool _maintenance;
            bool _run;
            bool _stLevel1;
            bool _stLevel2;
            bool _stReservoirLevel1;
            MemoryStorage *_store;
            unsigned long _countWaterNeeded;
            unsigned long _startPump;
            int _ledGreenOn = 3000;
            int _ledRedOn = 0;
            int _buzzerOn = 0;
            int _intervalOff = 1000;

            utils::IntervalSwitch *_workingLedInterval;
            utils::IntervalSwitch *_ErrorLedInterval;
            utils::IntervalSwitch *_buzzerInterval;

            void loadMemory() {
                EEPROM.get(0, *this->_store);
                // Check if memory struture version is diferent, then load default parameters
                if(_store->memoryVersion != MEMORY_VERSION) {
                    _store->memoryVersion = MEMORY_VERSION;
                    _store->timeOutRele = 12;
                    _store->levelFullSetter = 150;
                    _store->levelZeroSetter = 10;
                    _store->histeresis = 0;
                    storeMemory();
                }
            }

            void storeMemory() {
                EEPROM.put(0, *this->_store);
            }

            void runPump(bool pump)
            {
                _run = pump;
                digitalWrite(_pinPump, !pump);
            }
            void readSensors()
            {
                _stLevel1 = digitalRead(_pinLevel1);
                _stLevel2 = digitalRead(_pinLevel2);
                _stReservoirLevel1 = digitalRead(_pinReservoirLevel1);
            }
            bool checkRefillCondition()
            {
                bool check = ((!_stLevel1 && !_stLevel2) && _stReservoirLevel1);
                if (check)
                {
                    if (_countWaterNeeded == 0)
                    {
                        _countWaterNeeded = millis();
                    }
                }
                else
                {
                    _countWaterNeeded = 0;
                }
                return check;
            }
            void checkErrors()
            {
                checkWorkingLed();
                checkErrorLed();
                checkBuzzer();
            }
            void checkWorkingLed()
            {
                if (maintenance())
                {
                    _ledGreenOn = 100;
                }
                else if (isRunning())
                {
                    _ledGreenOn = 700;
                }
                else
                {
                    _ledGreenOn = 1500;
                }
            }
            void checkErrorLed()
            {
                if (_stLevel2)
                {
                    _ledRedOn = 300;
                }
                else if (!_stReservoirLevel1)
                {
                    _ledRedOn = 800;
                }
                else if (isOverRun())
                {
                    _ledRedOn = 1000;
                }
                else
                {
                    _ledRedOn = 0;
                }
            }
            void checkBuzzer()
            {
                if (_stLevel2)
                {
                    _buzzerOn = 300;
                }
                else if (!_stReservoirLevel1)
                {
                    _buzzerOn = 800;
                }
                else if (isOverRun())
                {
                    _buzzerOn = 1000;
                }
                else
                {
                    _buzzerOn = 0;
                }
            }

        public:
            AWL(int pinLevel1, int pinLevel2, int pinReservoirLevel1, int pinPump)
            {
                _pinLevel1 = pinLevel1;
                _pinLevel2 = pinLevel2;
                _pinReservoirLevel1 = pinReservoirLevel1;
                _pinPump = pinPump;
                _maintenance = false;
                _store = &store;

                runStop(12); // TODO: transfer to new variable
                _countWaterNeeded = 0;

                workingLedInterval.setIntervalOn(&_ledGreenOn);
                workingLedInterval.setIntervalOff(&_ledGreenOn);
                workingLedInterval.setCallBack(actionWorkLed);
                _workingLedInterval = &workingLedInterval;

                errorLedInterval.setIntervalOn(&_ledRedOn);
                errorLedInterval.setIntervalOff(&_ledRedOn);
                errorLedInterval.setCallBack(actionErrorLed);
                _ErrorLedInterval = &errorLedInterval;

                buzzerInterval.setIntervalOn(&_buzzerOn);
                buzzerInterval.setIntervalOff(&_buzzerOn);
                buzzerInterval.setCallBack(actionBuzzer);
                _buzzerInterval = &buzzerInterval;

                _instance = this;
            }
            void begin()
            {
                loadMemory();
                pinMode(_pinLevel1, INPUT_PULLUP);
                pinMode(_pinLevel2, INPUT_PULLUP);
                pinMode(_pinReservoirLevel1, INPUT_PULLUP);
                pinMode(_pinPump, OUTPUT);

                pinMode(WORKING_LED_PIN, OUTPUT);
                pinMode(ERROR_LED_PIN, OUTPUT);
                pinMode(BUZZER_PIN, OUTPUT);
                pinMode(MAINTENANCE_PIN, INPUT_PULLUP);

                pinMode(HS_TRIGGER_PIN, OUTPUT);
                pinMode(HS_ECHO_PIN, INPUT);

                attachInterrupt(digitalPinToInterrupt(MAINTENANCE_PIN), interrupMaintenance, FALLING);
            }

            bool maintenance()
            {
                return _maintenance;
            }

            void maintenance(bool maintenance)
            {
                _maintenance = maintenance;
            }

            static void interrupMaintenance()
            {
                
                if (_instance != 0)
                {
                    _instance->maintenance(!_instance->maintenance());
                }
            }

            int runStop()
            {
                return _store->timeOutRele;
            }
            void runStop(int seg)
            {
                
                if (seg < 3 || seg > 240)
                    return;
                _store->timeOutRele = seg;
                storeMemory();
            }

            int reservoirZeroSetter(bool set= false){
                if(levelRaw() == -1) return -1;
                if(set){
                    _store->levelZeroSetter = levelRaw();
                    storeMemory();
                }
                return _store->levelZeroSetter;
            }
            int reservoirFullSetter(bool set= false){
                if(levelRaw() == -1) return -1;
                if(set){
                    _store->levelFullSetter = levelRaw();
                    storeMemory();
                }
                return _store->levelFullSetter;
            }

            int histeresisSetter(){
                return _store->histeresis;
            };
            void histeresisSetter(int ms){
                _store->histeresis = ms;
                storeMemory();
            };

            int levelRaw()
            {
                long duration, distance;

                digitalWrite(HS_TRIGGER_PIN, LOW); //para generar un pulso limpio ponemos a LOW 4us
                delayMicroseconds(4);
                digitalWrite(HS_TRIGGER_PIN, HIGH); //generamos Trigger (disparo) de 10us
                delayMicroseconds(10);
                digitalWrite(HS_TRIGGER_PIN, LOW);

                duration = pulseIn(HS_ECHO_PIN, HIGH); //medimos el tiempo entre pulsos, en microsegundos

                distance = duration * 100 / 292 / 2; //convertimos a distancia, en cm
                if (distance <= 0)
                    return -1;
                if (distance > 0xFFFF)
                    return 0xFFFF;
                return distance;
            }

            int levelPercent()
            {
                int level = levelRaw();
                if (level <= 0)
                    return -1;

                int zero = 150;
                int full = 10;

                float pendiente = (float)(0 - 100) / (float)(zero - full);

                int result = (pendiente * ((float)level - (float)full)) + (float)100;
                if (result > 100)
                    return 100;
                if (result < 0)
                    return 0;
                return result;
            }

            void checkLevel()
            {
                readSensors();
                if (!_maintenance && checkRefillCondition() && (_countWaterNeeded + 2000) < millis()) // && abs(millis() - _countWaterNeeded) > 2000 ))
                {
                    if (_run && isOverRun()) //&& abs(millis() - _startPump) > _store->timeOutRele * 1000 )
                    {
                        runPump(false);
                    }
                    else if (_startPump == 0)
                    {
                        runPump(true);
                        _startPump = millis();
                    }
                }
                else
                {
                    reset();
                    runPump(false);
                }
                checkErrors();
                _workingLedInterval->loop();
                _ErrorLedInterval->loop();
                _buzzerInterval->loop();
            }
            String getPrint()
            {
                String result = "{\"lv1\":\"" + String(getStLevel1()) + "\",\"lv2\":\"" + String(_stLevel2) + "\",\"lvR1\":\"" + String(_stReservoirLevel1) + "\"";
                result += ",\"run\":\"" + String(_run) + "\",\"TimeRun\":\"" + String(_startPump) + "\",\"TimeOut\":\"" + String(_store->timeOutRele*1000) + "\",\"mnt\":\"" + String(_maintenance) + "\",\"ms\":\"" + String(millis()) + "\", \"level_dis\":\"" + levelRaw() + "\", \"level_%\":\"" + levelPercent() + "\"}";
                return result;
            }
            bool getStLevel1()
            {
                return _stLevel1;
            }
            bool getStLevel2()
            {
                return _stLevel2;
            }
            bool getStReservoirLevel1()
            {
                return _stReservoirLevel1;
            }
            bool isRunning()
            {
                return _run;
            }
            bool isOverRun()
            {
                if (_startPump == 0)
                {
                    return false;
                }
                return (_startPump + (_store->timeOutRele * 1000)) < millis();
            }
            void reset()
            {
                _startPump = 0;
            }
            uint16_t status()
            {
                uint16_t result = 0;
                int level = levelPercent();

                if (_stLevel1)
                {
                    bitSet(result, 0);
                }

                if (_stLevel2)
                {
                    bitSet(result, 1);
                }

                if (_stReservoirLevel1)
                {
                    bitSet(result, 2);
                }

                if (isRunning())
                {
                    bitSet(result, 3);
                }

                if (isOverRun())
                {
                    bitSet(result, 4);
                }

                if (_maintenance)
                {
                    bitSet(result, 5);
                }
                if (level != -1)
                {
                    bitSet(result, 6);
                }
                if (level != -1 && bitRead((uint8_t)level, 0))
                {
                    bitSet(result, 8);
                }
                if (level != -1 && bitRead((uint8_t)level, 1))
                {
                    bitSet(result, 9);
                }
                if (level != -1 && bitRead((uint8_t)level, 2))
                {
                    bitSet(result, 10);
                }
                if (level != -1 && bitRead((uint8_t)level, 3))
                {
                    bitSet(result, 11);
                }
                if (level != -1 && bitRead((uint8_t)level, 4))
                {
                    bitSet(result, 12);
                }
                if (level != -1 && bitRead((uint8_t)level, 5))
                {
                    bitSet(result, 13);
                }
                if (level != -1 && bitRead((uint8_t)level, 6))
                {
                    bitSet(result, 14);
                }
                return result;
            }
        };

        AWL *AWL::_instance = 0;

        void actionBuzzer(bool *on)
        {
            if (*on)
            {
                tone(BUZZER_PIN, 2000);
            }
            else
            {
                noTone(BUZZER_PIN);
            }
        }
        void actionWorkLed(bool *on)
        {
            if (*on)
            {
                digitalWrite(WORKING_LED_PIN, HIGH);
            }
            else
            {
                digitalWrite(WORKING_LED_PIN, LOW);
            }
        }

        void actionErrorLed(bool *on)
        {
            if (*on)
            {
                digitalWrite(ERROR_LED_PIN, HIGH);
            }
            else
            {
                digitalWrite(ERROR_LED_PIN, LOW);
            }
        }

    } // namespace AWL

} // namespace acuaris

#endif