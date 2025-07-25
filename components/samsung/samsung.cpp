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
        const uint16_t kSamsungAcHdrMark = 2920;
        const uint16_t kSamsungAcHdrSpace = 8960;
        const uint16_t kSamsungAcBitMark = 490;
        const uint16_t kSamsungAcOneSpace = 1560;
        const uint16_t kSamsungAcZeroSpace = 546;
        const uint16_t kSamsungAcSectionGap = 2886;
        const uint16_t kSamsungTick = 560;
        const uint16_t kSamsungHdrMarkTicks = 8;
        const uint16_t kSamsungMinMessageLengthTicks = 193;
        const uint16_t kSamsungHdrSpaceTicks = 8;
        const uint16_t kSamsungBitMarkTicks = 1;
        const uint16_t kSamsungOneSpaceTicks = 3;
        const uint16_t kSamsungMinGapTicks =
            kSamsungMinMessageLengthTicks -
            (kSamsungHdrMarkTicks + kSamsungHdrSpaceTicks +
             kSamsungBits * (kSamsungBitMarkTicks + kSamsungOneSpaceTicks) +
             kSamsungBitMarkTicks);
        const uint32_t kSamsungMinGap = 0;
        const uint16_t kSamsungAcSectionLength = 7;

        static const char *const TAG = "samsung.climate";

        void SamsungClimate::setup()
        {
            climate_ir::ClimateIR::setup();
            this->ac_.stateReset(true, false);
            _lastsentpowerstate = false;
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
            this->check_and_send();
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

        void SamsungClimate::check_and_send()
        {
            uint8_t *message = this->ac_.getRaw();

            // When changing power state an extende message is required
            // Timer settings aren't managed so it's ignored here
            if (this->ac_.getPower() != _lastsentpowerstate)
            {
                ESP_LOGI(TAG, "Sending EXTENDED message");

                // Copied from ir_Samsung.cpp
                _lastsentpowerstate = this->ac_.getPower();

                static const uint8_t extended_middle_section[kSamsungAcSectionLength] = {
                    0x01, 0xD2, 0x0F, 0x00, 0x00, 0x00, 0x00};

                std::memcpy(message + 2 * kSamsungAcSectionLength,
                            message + kSamsungAcSectionLength,
                            kSamsungAcSectionLength);

                std::memcpy(message + kSamsungAcSectionLength, extended_middle_section,
                            kSamsungAcSectionLength);

                this->send(message, kSamsungAcExtendedStateLength);

                std::memcpy(message + kSamsungAcSectionLength,
                            message + 2 * kSamsungAcSectionLength,
                            kSamsungAcSectionLength);
            }
            else
            {
                this->send(message, kSamsungAcStateLength);
            }
        }

        void SamsungClimate::send(const uint8_t *data, const uint16_t nbytes)
        {
            if (nbytes < kSamsungAcStateLength && nbytes % kSamsungAcSectionLength)
                return; // Not an appropriate number of bytes to send a proper message.
            ESP_LOGD(TAG, "Sending %d bytes", nbytes);
            // sendGeneric(
            //     this->transmitter_,
            //     kSamsungAcHdrMark, kSamsungAcHdrSpace,
            //     kSamsungAcBitMark, kSamsungAcOneSpace,
            //     kSamsungAcBitMark, kSamsungAcZeroSpace,
            //     kSamsungAcBitMark, kSamsungMinGap,
            //     data, nbytes,
            //     38000);
            for (uint16_t offset = 0; offset < nbytes;
                 offset += kSamsungAcSectionLength)
            {
                // sendGeneric(kSamsungAcSectionMark, kSamsungAcSectionSpace,
                //             kSamsungAcBitMark, kSamsungAcOneSpace, kSamsungAcBitMark,
                //             kSamsungAcZeroSpace, kSamsungAcBitMark, kSamsungAcSectionGap,
                //             data + offset, kSamsungAcSectionLength, // 7 bytes == 56 bits
                //             38000, false, 0, 50);                   // Send in LSBF order
                sendGeneric(
                    this->transmitter_,
                    kSamsungAcHdrMark, kSamsungAcHdrSpace,
                    kSamsungAcBitMark, kSamsungAcOneSpace,
                    kSamsungAcBitMark, kSamsungAcZeroSpace,
                    kSamsungAcBitMark, kSamsungAcOneSpace,
                    data+offset, 7,
                    38000);
            }
        }

    } // namespace Samsung_general
} // namespace esphome
