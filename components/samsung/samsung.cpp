#define _IR_ENABLE_DEFAULT_ false
#define SEND_SAMSUNG_AC true
#define DECODE_SAMSUNG_AC false

#include "esphome.h"
#include "ir_Samsung.h"
#include "samsung.h"

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

        climate::ClimateTraits PanasonicClimate::traits()
        {
            auto traits = climate_ir::ClimateIR::traits();

            traits.add_supported_swing_mode(climate::CLIMATE_SWING_HORIZONTAL);
            traits.add_supported_swing_mode(climate::CLIMATE_SWING_BOTH);

            return traits;
        }

        void PanasonicClimate::transmit_state()
        {
            this->apply_state();

            ac_.send(1);
        }

        void PanasonicClimate::apply_state()
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
                    this->ac_.setMode(kPanasonicAcAuto);
                    break;
                case climate::CLIMATE_MODE_HEAT:
                    this->ac_.setMode(kPanasonicAcHeat);
                    break;
                case climate::CLIMATE_MODE_COOL:
                    this->ac_.setMode(kPanasonicAcCool);
                    break;
                case climate::CLIMATE_MODE_DRY:
                    this->ac_.setMode(kPanasonicAcDry);
                    break;
                case climate::CLIMATE_MODE_FAN_ONLY:
                    this->ac_.setMode(kPanasonicAcFan);
                    break;
                }

                if (this->fan_mode.has_value())
                {
                    switch (this->fan_mode.value())
                    {
                    case climate::CLIMATE_FAN_AUTO:
                        this->ac_.setFan(kPanasonicAcFanAuto);
                        break;
                    case climate::CLIMATE_FAN_QUIET:
                        this->ac_.setFan(kPanasonicAcFanMin);
                        break;
                    case climate::CLIMATE_FAN_LOW:
                        this->ac_.setFan(kPanasonicAcFanLow);
                        break;
                    case climate::CLIMATE_FAN_MEDIUM:
                        this->ac_.setFan(kPanasonicAcFanMed);
                        break;
                    case climate::CLIMATE_FAN_HIGH:
                        this->ac_.setFan(kPanasonicAcFanHigh);
                        break;
                    }
                }

                switch (this->swing_mode)
                {
                case climate::CLIMATE_SWING_OFF:
                    this->ac_.setSwingVertical(kPanasonicAcSwingVMiddle);
                    this->ac_.setSwingHorizontal(kPanasonicAcSwingHMiddle);
                    break;
                case climate::CLIMATE_SWING_VERTICAL:
                    this->ac_.setSwingVertical(kPanasonicAcSwingVAuto);
                    this->ac_.setSwingHorizontal(kPanasonicAcSwingHMiddle);
                    break;
                case climate::CLIMATE_SWING_HORIZONTAL:
                    this->ac_.setSwingVertical(kPanasonicAcSwingVMiddle);
                    this->ac_.setSwingHorizontal(kPanasonicAcSwingHAuto);
                    break;
                case climate::CLIMATE_SWING_BOTH:
                    this->ac_.setSwingVertical(kPanasonicAcSwingVAuto);
                    this->ac_.setSwingHorizontal(kPanasonicAcSwingHAuto);
                    break;
                }

                this->ac_.on();
            }

            ESP_LOGI(TAG, "%s", this->ac_.toString().c_str());
        }

    } // namespace panasonic_general
} // namespace esphome
