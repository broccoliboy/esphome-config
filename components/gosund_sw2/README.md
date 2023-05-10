```yaml

# example configuration:

output:
  - platform: gpio
    id: gosund_led
    pin: GPIO13

uart:
  - id: gosund_uart
    rx_pin: GPIO3
    tx_pin: GPIO1
    baud_rate: 115200

light:
  - platform: gosund_sw2
    id: gosund_sw2_0
    output: gosund_led

```
