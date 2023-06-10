import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart, light, output
from esphome.const import CONF_ID, CONF_OUTPUT_ID

DEPENDENCIES = ['uart']
INDICATOR_KEY = "indicator"

gosund_sw2_ns = cg.esphome_ns.namespace('gosund_sw2')
GosundSW2 = gosund_sw2_ns.class_('GosundSW2', light.LightOutput, uart.UARTDevice, cg.Component)

CONFIG_SCHEMA = light.BRIGHTNESS_ONLY_LIGHT_SCHEMA.extend({
    cv.GenerateID(CONF_OUTPUT_ID): cv.declare_id(GosundSW2),
    cv.Optional(INDICATOR_KEY): cv.use_id(output.BinaryOutput)
}).extend(uart.UART_DEVICE_SCHEMA)


def to_code(config):
    print("config:", config)
    var = cg.new_Pvariable(config[CONF_OUTPUT_ID])
    yield cg.register_component(var, config)
    yield uart.register_uart_device(var, config)
    yield light.register_light(var, config)

    if INDICATOR_KEY in config:
        indicator = yield cg.get_variable(config[INDICATOR_KEY])
        cg.add(var.set_indicator(indicator))
