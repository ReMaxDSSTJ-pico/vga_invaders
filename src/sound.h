// I don know what happens here
#include "PicoTimer.h"

#define PIN_SOUND_OUTPUT 19

// generate sound by toggling the pin state in the frequency of the tone


/**
 * @brief We use the TimerAlarm to generate tones.
 *
 * See class ArduinoPicoTone on PicoTone.h library 
 * 
 * Use 
 *  static void tone(uint8_t pinNumber, unsigned int frequency, int duration)
 * to generate a tone on the desired pin, desired frequency, desired miliseconds 
 *
 */

#include "PicoTimer.h"

enum SndSource {Pulse,Noise};

/**
 * @brief We use the TimerAlarm to generate tones.
 */
class PicoTone {
    public:
        PicoTone(){
        }
        ~PicoTone(){
            noTone();
        }
        static PicoTone* getInstance() { return &picotoneInstance; }
        
    protected:
        TimerAlarmRepeating alarm;
        TimerAlarm alarm_end;

        int pin=PIN_SOUND_OUTPUT;
        bool state=false;
        SndSource   signal=Pulse;
    private:
        static PicoTone picotoneInstance;
        
    public:
        uint64_t snddelay=0;
        int16_t tremolo_delta=0;
        uint16_t tremolo_max_delay=0;
        uint16_t tremolo_min_delay=0;
           
// generate sound by toggling the pin state in the frequency of the tone
        static bool generate_sound_callback(repeating_timer_t *rt){
            
            PicoTone *pt = (PicoTone*)rt->user_data;
            
            switch (pt->signal) {
            
            case Pulse:     pt->state = !pt->state; // toggle state
                            break;
            case Noise:     pt->state = (RandU8Max(6) > 3);
                            break;
            }

            gpio_put(pt->pin, pt->state);        
            
            if ((pt->tremolo_delta)!=0) {
                
                (pt->snddelay) += pt->tremolo_delta;
                
                if (pt->snddelay > pt->tremolo_max_delay) pt->tremolo_delta=-(pt->tremolo_delta);
                if (pt->snddelay < pt->tremolo_min_delay) pt->tremolo_delta=-(pt->tremolo_delta);


                // cancel timer and set the new one
                cancel_repeating_timer(rt);
                pt->alarm.start(pt->generate_sound_callback, pt->snddelay, US, pt);
                //return false;
            }  
 
 
            
            return true;
        }
        
// stop sound after the duration of the tone
        static int64_t stop_sound_callback(alarm_id_t id, void *user_data){
            PicoTone *pt = (PicoTone*)user_data;
            pt->noTone();
            return 0; 
        }


        void tone(unsigned int frequency, uint64_t duration, SndSource sndtype=Pulse, uint8_t tremolo_speed=0, uint8_t tremolo_percent=0) {
            uint16_t    steps;
            
            snddelay = 500000 / frequency;
            signal = sndtype;
            
            if (tremolo_speed>0 && tremolo_percent>0) {     // Set the tremolo limits and delta
                tremolo_max_delay = (snddelay * (100 + tremolo_percent))/100;
                tremolo_min_delay = (snddelay * (100 - tremolo_percent))/100;
                steps = (500000 / tremolo_speed) / snddelay;                
                tremolo_delta = (snddelay * tremolo_percent / 100) / steps;
            }
            // check for running alarms
            if (alarm_end.alarm_id_run!=-1) {
                alarm_end.stop();
                alarm.stop();
            }
            //alarm.stop();
            alarm.start(generate_sound_callback, snddelay, US, this);
            
            // Set alarm to stop sound
            if (duration>0) 
                alarm_end.start(stop_sound_callback, duration, MS, this, true);
            
        }
        
        void noTone() {
            alarm.stop();
            alarm_end.stop();
            //snddelay=0;
            tremolo_delta=0;
            tremolo_max_delay=0;
            tremolo_min_delay=0;
        }

};

