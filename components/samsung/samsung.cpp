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
            ac.send(kSamsungAcDefaultRepeat);
        }

        void SamsungClimate::apply_state()
        {
            if (this->mode == climate::CLIMATE_MODE_OFF)
            {
                this->ac.off();
            }
            else
            {
                this->ac.setTemp(this->target_temperature);

                switch (this->mode)
                {
                case climate::CLIMATE_MODE_HEAT_COOL:
                    this->ac.setMode(kSamsungAcAuto);
                    break;
                case climate::CLIMATE_MODE_HEAT:
                    this->ac.setMode(kSamsungAcHeat);
                    break;
                case climate::CLIMATE_MODE_COOL:
                    this->ac.setMode(kSamsungAcCool);
                    break;
                case climate::CLIMATE_MODE_DRY:
                    this->ac.setMode(kSamsungAcDry);
                    break;
                case climate::CLIMATE_MODE_FAN_ONLY:
                    this->ac.setMode(kSamsungAcFan);
                    break;
                }

                if (this->fan_mode.has_value())
                {
                    switch (this->fan_mode.value())
                    {
                    case climate::CLIMATE_FAN_AUTO:
                        this->ac.setQuiet(false);
                        this->ac.setFan(kSamsungAcFanAuto);
                        break;
                    case climate::CLIMATE_FAN_QUIET:
                        this->ac.setQuiet(true);
                        break;
                    case climate::CLIMATE_FAN_LOW:
                        this->ac.setQuiet(false);
                        this->ac.setFan(kSamsungAcFanLow);
                        break;
                    case climate::CLIMATE_FAN_MEDIUM:
                        this->ac.setQuiet(false);
                        this->ac.setFan(kSamsungAcFanMed);
                        break;
                    case climate::CLIMATE_FAN_HIGH:
                        this->ac.setQuiet(false);
                        this->ac.setFan(kSamsungAcFanHigh);
                        break;
                    }
                }

                switch (this->swing_mode)
                {
                case climate::CLIMATE_SWING_OFF:
                    this->ac.setSwing(false);
                    this->ac.setSwingH(false);
                    break;
                case climate::CLIMATE_SWING_VERTICAL:
                    this->ac.setSwing(true);
                    this->ac.setSwingH(false);
                    break;
                case climate::CLIMATE_SWING_HORIZONTAL:
                    this->ac.setSwing(false);
                    this->ac.setSwingH(true);
                    break;
                case climate::CLIMATE_SWING_BOTH:
                    this->ac.setSwing(true);
                    this->ac.setSwingH(true);
                    break;
                }

                this->ac.on();
            }

            ESP_LOGI(TAG, "%s", this->ac.toString().c_str());
        }

    } // namespace Samsung_general
} // namespace esphome
