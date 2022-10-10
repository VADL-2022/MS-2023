import time
from csv import writer
from datetime import datetime
from datetime import timedelta
#import pigpio

# Append a list as a row to the CSV
def append_list_as_row(write_obj, list_of_elem):
    # Create a writer object from csv module
    csv_writer = writer(write_obj)
    # Add contents of list as last row in the csv file
    csv_writer.writerow(list_of_elem)

# Initialize GPIO pins ############################################################
# pi = pigpio.pi() # Initialize GPIO on the pi
# pi.set_pull_up_down(26, pigpio.PUD_DOWN) # Set pull down resistor on pin 26 for reading voltage
# pi.set_mode(26, pigpio.INPUT) # Set pin 26 to output

# Initialize CSV file for recording IMU data ########################################
timestr = time.strftime("%Y%m%d-%H%M%S")
my_log = "../dataOutput/LOG_" + timestr + ".StrainVoltage.csv"
file_name=my_log
with open(my_log, 'w', newline='') as file:
    new_file = writer(file)
start = datetime.now()

# Starts running the mission loop that continually checks data
try:
    # Open file in append mode
    with open(file_name, 'a+', newline='') as write_obj:
        while True:
            now = datetime.now()
            delt = now - start
            csvList = [delt.microseconds/1000.00,"H"]#pi.read(26)]
            append_list_as_row(write_obj, csvList)#pi.read(26)) # Read input voltage on pin 26 and write to csv
            time.sleep(0.008) # sleep for 10ms (lowest sample period)
except KeyboardInterrupt:
    print("Stopping logging due to keyboard interrupt")
# finally:
#     if counter > 0:
#         with open(file_name, 'a+', newline='') as write_obj:
#             append_list_as_row(write_obj, ["Calibration data",avgX/counter,avgY/counter,avgZ/counter])

