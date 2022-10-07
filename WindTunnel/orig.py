import board
import digitalio
import adafruit_max31855
spi = board.SPI()
cs = digitalio.DigitalInOut(board.D5)
max31855 = adafruit_max31855.MAX31855(spi, cs)

print('Temperature: {} degrees C'.format(max31855.temperature))
