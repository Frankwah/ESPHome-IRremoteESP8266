#define _IR_ENABLE_DEFAULT_ false
#define SEND_SAMSUNG_AC true
#define DECODE_SAMSUNG_AC false

#include "esphome.h"
#include "ir_Samsung.h"
#include "samsung.h"
#include "esphome/core/log.h"
namespace esphome
{
    namespace samsung
    {
        static const char *const TAG = "samsung.climate";

        void SamsungClimate::setup()
        {
            climate_ir::ClimateIR::setup();            
            this->apply_state(); 
            
        }

        climate::ClimateTraits SamsungClimate::traits()
        {
            auto traits = climate_ir::ClimateIR::traits();

            traits.add_supported_swing_mode(climate::CLIMATE_SWING_HORIZONTAL);
            traits.add_supported_swing_mode(climate::CLIMATE_SWING_BOTH);

            return traits;
        }

        void SamsungClimate::transmit_state()
        {
            this->apply_state();   
            ESP_LOGI(TAG, "APPLIED STATE, SENDING...");         
            ac_.send(1);
            ESP_LOGI(TAG, "STATE SENT");    
        }

        void SamsungClimate::apply_state()
        {
            if (this->mode == climate::CLIMATE_MODE_OFF)
            {
                this->ac_.off();
            }
            else
            {
                this->ac_.setTemp(this->target_temperature);

                switch (this->mode)
                {
                case climate::CLIMATE_MODE_HEAT_COOL:
                    this->ac_.setMode(kSamsungAcAuto);
                    break;
                case climate::CLIMATE_MODE_HEAT:
                    this->ac_.setMode(kSamsungAcHeat);
                    break;
                case climate::CLIMATE_MODE_COOL:
                    this->ac_.setMode(kSamsungAcCool);
                    break;
                case climate::CLIMATE_MODE_DRY:
                    this->ac_.setMode(kSamsungAcDry);
                    break;
                case climate::CLIMATE_MODE_FAN_ONLY:
                    this->ac_.setMode(kSamsungAcFan);
                    break;
                }

                if (this->fan_mode.has_value())
                {
                    switch (this->fan_mode.value())
                    {
                    case climate::CLIMATE_FAN_AUTO:
                        this->ac_.setQuiet(false);
                        this->ac_.setFan(kSamsungAcFanAuto);
                        break;
                    case climate::CLIMATE_FAN_QUIET:
                        this->ac_.setQuiet(true);
                        break;
                    case climate::CLIMATE_FAN_LOW:
                        this->ac_.setQuiet(false);
                        this->ac_.setFan(kSamsungAcFanLow);
                        break;
                    case climate::CLIMATE_FAN_MEDIUM:
                        this->ac_.setQuiet(false);
                        this->ac_.setFan(kSamsungAcFanMed);
                        break;
                    case climate::CLIMATE_FAN_HIGH:
                        this->ac_.setQuiet(false);
                        this->ac_.setFan(kSamsungAcFanHigh);
                        break;
                    }
                }

                switch (this->swing_mode)
                {
                case climate::CLIMATE_SWING_OFF:
                    this->ac_.setSwing(false);
                    this->ac_.setSwingH(false);
                    break;
                case climate::CLIMATE_SWING_VERTICAL:
                    this->ac_.setSwing(true);
                    this->ac_.setSwingH(false);
                    break;
                case climate::CLIMATE_SWING_HORIZONTAL:
                    this->ac_.setSwing(false);
                    this->ac_.setSwingH(true);
                    break;
                case climate::CLIMATE_SWING_BOTH:
                    this->ac_.setSwing(true);
                    this->ac_.setSwingH(true);
                    break;
                }

                this->ac_.on();
            }

            ESP_LOGI(TAG, "%s", this->ac_.toString().c_str());
        }

    } // namespace Samsung_general
} // namespace esphome
