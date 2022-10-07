import time
from gpiozero import CPUTemperature
import time
import sys
import datetime

withoutThermocouple = (sys.argv[1] == '1') if len(sys.argv) > 1 else False
quiet = (sys.argv[2] == '1') if len(sys.argv) > 2 else False
timestr = time.strftime("%Y_%m_%d-%H_%M_%S")
counter = 0

if not withoutThermocouple:
    import board
    import digitalio
    import adafruit_max31855

    spi = board.SPI()
    cs = digitalio.DigitalInOut(board.D5)
    max31855 = adafruit_max31855.MAX31855(spi, cs)

try:
    with open("dataOutput/temperature_" + timestr + ".log.csv", "w") as out:
        def output(formatStr, val, boolLast):
            out.write(str(val) + (',' if not boolLast else ''))
            if not quiet:
                print(formatStr.format(val))

        out.write("Clock time,Time since started recording data (seconds),Thermocouple temperature (degrees C),Pi temperature (degrees C)\n")
        print("Press Ctrl-C to stop")
        start = None
        while True:
            current = time.time_ns() # time_ns() is since the epoch.
            thermo = None
            piTemp = None
            if not withoutThermocouple:
                thermo = max31855.temperature
            with open("/sys/class/thermal/thermal_zone0/temp", "r") as temp:
                piTemp = float(temp.read()) / 1000.0

            timestr = datetime.datetime.fromtimestamp(current / 1.0e9).strftime("%Y-%m-%d %H:%M:%S ") + time.strftime("%Z") # or use datetime.datetime.now().isoformat()
            if start is None:
                start = current
            time2 = float(current - start) / 1.0e9 # Then we subtract the start time in nanoseconds and convert to seconds by dividing by 1e9.
            out.write(timestr + ',' + str(time2) + ',')
            
            if not withoutThermocouple:
                output('Thermocouple temperature: {} degrees C', thermo, False)
            else:
                out.write(',')
            output('Pi temperature: {} degrees C', piTemp, True)
            out.write('\n')
            
            time.sleep(0.5) # 2 Hz

            counter+=1
            if counter > 4:
                out.flush() # Save every once in a while
                counter = 0
except KeyboardInterrupt:
    print("Saved")
