from pymodbus.client import ModbusTcpClient

client = ModbusTcpClient("192.168.138.2", port=502)
client.connect()

client.write_register(0, 1)

result = client.read_holding_registers(0, count=3)
print(result.registers)

client.close()
