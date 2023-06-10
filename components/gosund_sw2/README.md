```yaml

# Example configuration. See `devices/gosund-sw2/gosund-sw2.yaml` for a complete device example.

output:
  - platform: gpio
    id: gosund_indicator
    pin:
      number: GPIO13
      inverted: true

uart:
  - id: gosund_uart
    rx_pin: GPIO3
    tx_pin: GPIO1
    baud_rate: 115200

light:
  - platform: gosund_sw2
    id: gosund_sw2_light
    name: "Dimmer"  # need name so light will show up in Home Assistant
    indicator: gosund_indicator

```
