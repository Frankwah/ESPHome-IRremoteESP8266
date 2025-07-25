import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import climate_ir
from esphome.components import ir_remote_base
from esphome.const import CONF_PIN
from esphome import pins

AUTO_LOAD = ["climate_ir", "ir_remote_base"]

samsung_ns = cg.esphome_ns.namespace("samsung")
samsungClimate = samsung_ns.class_("SamsungClimate", climate_ir.ClimateIR)

CONFIG_SCHEMA = climate_ir.climate_ir_schema(samsungClimate).extend(
    {        
        
    }
)


async def to_code(config):   
    ir_remote_base.load_ir_remote()
    var = await climate_ir.new_climate_ir(config)       
