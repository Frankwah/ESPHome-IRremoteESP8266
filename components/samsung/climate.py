import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import climate_ir
from esphome.const import CONF_PIN
from esphome import pins

AUTO_LOAD = ["climate_ir"]

samsung_ns = cg.esphome_ns.namespace("samsung")
samsungClimate = samsung_ns.class_("SamsungClimate", climate_ir.ClimateIR)

CONFIG_SCHEMA = climate_ir.climate_ir_schema(samsungClimate).extend(
    {        
        cv.Required(CONF_PIN): pins.gpio_output_pin_schema        
    }
)


async def to_code(config):   
    var = await climate_ir.new_climate_ir(config)
    #cg.add(var.set_model(config[CONF_MODEL]))
    pin = await cg.gpio_pin_expression(config[CONF_PIN])
    cg.add(var.set_transmit_pin(pin))
