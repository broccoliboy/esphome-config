// Credit: https://github.com/nuttytree/ESPHome-Devices

#pragma once
#include "esphome.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"
#include "esphome/components/output/binary_output.h"
#include "esphome/components/light/light_state.h"
#include "esphome/components/light/light_output.h"
#include "esphome/components/uart/uart.h"

namespace esphome
{
  namespace gosund_sw2
  {

    static const char *const TAG = "gosund_sw2";

    class GosundSW2 : public Component, public light::LightOutput, public uart::UARTDevice
    {
    public:
      void set_indicator(output::BinaryOutput *_indicator) { indicator = _indicator; }
      void setup() override;
      void loop() override;
      light::LightTraits get_traits() override;
      void setup_state(light::LightState *state) override { this->state = state; }
      void write_state(light::LightState *state) override;
      void dump_config() override;

      void set_output_min(float val);
      void set_output_max(float val);
      void set_output_gamma(float val);

      void update_mcu();

    protected:
      void publish();
      light::LightState *state;
      output::BinaryOutput *indicator = NULL;

      bool enabled_current = false;
      bool enabled_target = false;
      float brightness_current = 0;
      float brightness_target = 0;
      const char RX_START = 0x24;
      const char RX_STOP = 0x23;
      static const char RX_LENGTH = 5;
      uint8_t RX_DATA[RX_LENGTH];
      const char SLIDER_MIN = 0;
      const char SLIDER_MAX = 150;
      const char LED_MIN = 1;
      const char LED_MAX = 7;
      const char DIMMER_MIN = 137;
      const char DIMMER_MAX = 228;
      const float OUTPUT_RANGE_MIN = 0;
      const float OUTPUT_RANGE_MAX = 1;
      const float OUTPUT_GAMMA_MIN = 0.5;
      const float OUTPUT_GAMMA_MAX = 3.0;
      float output_min = 0;
      float output_max = 1;
      float output_gamma = 1;
      uint32_t publish_time_previous = 0;
      uint32_t publish_interval = 250; // ms
      bool need_to_publish = false;
    };

    void GosundSW2::dump_config()
    {
      ESP_LOGCONFIG(TAG, "Gosund SW2 Light");
    }

    void GosundSW2::setup()
    {
      state->set_default_transition_length(0);
      state->set_flash_transition_length(0);
      state->set_gamma_correct(1);
      state->set_restore_mode(light::LightRestoreMode::LIGHT_ALWAYS_OFF);
      update_mcu();
    }

    light::LightTraits GosundSW2::get_traits()
    {
      auto traits = light::LightTraits();
      traits.set_supported_color_modes({light::ColorMode::BRIGHTNESS});
      return traits;
    }

    void GosundSW2::set_output_min(float val)
    {
      output_min = constrain(val, OUTPUT_RANGE_MIN, OUTPUT_RANGE_MAX);
      update_mcu();
    }

    void GosundSW2::set_output_max(float val)
    {
      output_max = constrain(val, OUTPUT_RANGE_MIN, OUTPUT_RANGE_MAX);
      update_mcu();
    }

    void GosundSW2::set_output_gamma(float val)
    {
      output_gamma = constrain(val, OUTPUT_GAMMA_MIN, OUTPUT_GAMMA_MAX);
      update_mcu();
    }

    void GosundSW2::update_mcu()
    {
      char led = LED_MIN + round((LED_MAX - LED_MIN) * brightness_target);
      led = constrain(led, LED_MIN, LED_MAX);
      float output_corrected = powf(brightness_target, output_gamma);
      output_corrected = output_min + (output_max - output_min) * output_corrected; 
      char dimmer = DIMMER_MIN + round((DIMMER_MAX - DIMMER_MIN) * output_corrected);
      dimmer = constrain(dimmer, DIMMER_MIN, DIMMER_MAX);
      char b0 = (char(enabled_target) << 6) + led;
      char b1 = dimmer;

      ESP_LOGV(TAG, "Writing to MCU: enabled=%d, led=%d, dimmer=%d, command=0x%02X,0x%02X", enabled_target, led, dimmer, b0, b1);

      write_byte(b0);
      delay(5); // sw2 needs a slight delay between these two bytes
      write_byte(b1);
      delay(5);

      enabled_current = enabled_target;
      brightness_current = brightness_target;
    }

    void GosundSW2::publish()
    {
      ESP_LOGV(TAG, "Publishing new state: enabled=%d, brightness=%.4f", enabled_target, brightness_target);
      auto call = state->make_call();
      call.set_state(enabled_target);
      call.set_brightness(brightness_target);
      call.perform();
      need_to_publish = false;
    }

    void GosundSW2::write_state(light::LightState *state)
    {
      // update enabled_target from state call
      state->current_values.as_binary(&enabled_target);

      // only update brightness_target if enabled
      if (enabled_target)
      {
        state->current_values.as_brightness(&brightness_target);
      }

      if (indicator)
        indicator->set_state(enabled_target);

      ESP_LOGV(TAG, "Update state request: enabled=%d, brightness=%.4f", enabled_target, brightness_target);
    }

    void GosundSW2::loop()
    {
      const uint8_t max_commands_per_loop = 5;
      uint8_t commands_received = 0;

      // Purge anything from serial buffer that's not RX_START byte
      // This solves issue where MCU transmits garbage data on power up
      while (available() && peek() != RX_START)
        read_byte(RX_DATA);

      // Read commands in groups of 5 bytes, but only up to max_commands_per_loop times
      // so we turn over control to the rest of esphome semi-regularly
      while (available() >= 5 && commands_received < max_commands_per_loop)
      {
        read_array(RX_DATA, RX_LENGTH);
        if (RX_START == RX_DATA[0] && RX_STOP == RX_DATA[RX_LENGTH - 1])
        {
          uint8_t slider = RX_DATA[1];
          brightness_target = float(slider - SLIDER_MIN) / float(SLIDER_MAX - SLIDER_MIN);
          need_to_publish = true;
          commands_received++;
          // send any new brightness values immediately
          update_mcu();
        }
      }

      // If we have updated values (from esphome or homeassistant), send them to MCU
      if (enabled_current != enabled_target || brightness_current != brightness_target)
      {
        update_mcu();
      }

      // After we're done processing UART input, see if we need to publish updated values to esphome
      // Limit publishes to once per publish_interval
      uint32_t current_time = millis();
      if (need_to_publish && (current_time - publish_time_previous) > publish_interval)
      {
        publish();
        publish_time_previous = current_time;
      }
    }

  }
}
