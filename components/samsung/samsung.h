#pragma once

#include "esphome/core/log.h"
#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "esphome/components/climate_ir/climate_ir.h"

#include "ir_Samsung.h"


namespace esphome
{
    namespace samsung
    {       
        class SamsungClimate : public climate_ir::ClimateIR
        {
        public:
            SamsungClimate()
                : ClimateIR(16, 30, 1.0f, true, true,
                            {climate::CLIMATE_FAN_AUTO, climate::CLIMATE_FAN_LOW, climate::CLIMATE_FAN_MEDIUM, climate::CLIMATE_FAN_HIGH, climate::CLIMATE_FAN_QUIET},
                            {climate::CLIMATE_SWING_OFF, climate::CLIMATE_SWING_VERTICAL}) {}
            void set_transmit_pin(InternalGPIOPin *transmit_pin) { transmit_pin_ = transmit_pin; }
            void setup() override;
            climate::ClimateTraits traits() override;

        protected:
            InternalGPIOPin *transmit_pin_;
            void transmit_state() override;
            void apply_state();            
            IRSamsungAc ac_ = IRSamsungAc(255); // pin is not used

        };

    } // namespace samsung
} // namespace esphome
