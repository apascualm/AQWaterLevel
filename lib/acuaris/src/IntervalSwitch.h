#ifndef ACUARIS_INTERVAL_SWITCH_H
#define ACUARIS_INTERVAL_SWITCH_H

#include <Arduino.h>

namespace acuaris
{
    namespace utils
    {
        class IntervalSwitch
        {
        private:
            void (*_callback)(bool *on);
            int *_intervalOn;
            int *_intervalOff;
            int _msOn = 0;
            int _msOff = 0;
            String _id = "";
            unsigned long _lastMillis = millis();
            unsigned long _nextMillis = millis();
            bool _on;
            bool isTimeOverflow()
            {
                if (abs(_nextMillis - _lastMillis) > 300000)
                    return true;
                return false;
            }
            void recordTimes()
            {
                _nextMillis = millis();
            }
            int getMillisInterval()
            {
                recordTimes();
                if (isTimeOverflow())
                {
                    return (4294967296 + _nextMillis) - _lastMillis;
                }
                return _nextMillis - _lastMillis;
            }

        public:
            IntervalSwitch()
            {
                int dato = 0;
                _intervalOn = &dato;
                _intervalOff = &dato;
                _on = false;
                _id = "simple";
            }
            IntervalSwitch(String id, int *intervalOn, int *intervalOff, void (&callback)(bool *on), bool startOn = false)
            {
                _callback = &callback;
                _intervalOn = intervalOn;
                _intervalOff = intervalOff;
                _on = startOn;
                _id = id;
            }
            IntervalSwitch(int *intervalOn, int *intervalOff, void (&callback)(bool *on), bool startOn = false)
            {
                _callback = &callback;
                _intervalOn = intervalOn;
                _intervalOff = intervalOff;
                _on = startOn;
            }
            void setIntervalOn(int *intervalOn)
            {
                _intervalOn = intervalOn;
            }
            void setIntervalOff(int *intervalOff)
            {
                _intervalOff = intervalOff;
            }
            void setCallBack(void (&callback)(bool *on))
            {
                _callback = &callback;
            }
            void loop()
            {
                int intervalMillis = getMillisInterval();
                //Serial.print("print in loop for id: ");
                //Serial.println(_id);
                if (_on)
                {
                    _msOn = _msOn + intervalMillis;
                    if (_msOn > *_intervalOn)
                    {
                        _on = false;
                        _msOn = 0;
                    }
                }
                else
                {
                    _msOff = _msOff + intervalMillis;
                    if (_msOff > *_intervalOff)
                    {
                        _on = true;
                        _msOff = 0;
                    }
                }
                if (*_intervalOn == 0)
                    _on = false;
                _lastMillis = _nextMillis;
                if (_callback)
                    _callback(&_on);
            }
            ~IntervalSwitch()
            {
            }
        };
    } // namespace utils
} // namespace acuaris

#endif