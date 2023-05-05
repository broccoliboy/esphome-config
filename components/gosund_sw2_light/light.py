import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart, light, output
from esphome.const import CONF_ID
from esphome.const import CONF_OUTPUT_ID, CONF_OUTPUT

DEPENDENCIES = ['uart']

ns = cg.esphome_ns.namespace('gosund_sw2_light')
component = ns.class_('GosundSW2Light', uart.UARTDevice, light.LightOutput)

CONFIG_SCHEMA = light.BRIGHTNESS_ONLY_LIGHT_SCHEMA.extend({
    cv.GenerateID(CONF_OUTPUT_ID): cv.declare_id(component),
    cv.Optional(CONF_OUTPUT): cv.use_id(output.BinaryOutput)
}).extend(cv.COMPONENT_SCHEMA).extend(uart.UART_DEVICE_SCHEMA)

def to_code(config):
    var = cg.new_Pvariable(config[CONF_OUTPUT_ID])
    yield light.register_light(var, config)
    yield uart.register_uart_device(var, config)

    if CONF_OUTPUT in config:
        indicator = yield cg.get_variable(config[CONF_OUTPUT])
        cg.add(var.set_indicator(indicator))
