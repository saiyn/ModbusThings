# What is ModbusThings?

ModbusThings works as an agent that collect industry devices data through modbus protocol and transfer the data to server real-time or in a bulk way when network is not available.

# How to test

## setup  modbus simulator server(slave)

* download and install [diagslave](https://www.modbusdriver.com/diagslave.html)

* select the version matching your system and then cmd `sudo diagslave -m tcp` to start a tcp mode modebus slave

* server address is 127.0.0.1:502 