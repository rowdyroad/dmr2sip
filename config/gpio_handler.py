import os;
import time;
from time import sleep;
from optparse import OptionParser

parser = OptionParser()
parser.add_option("-p", "--pin", dest="pin", help="GPIO button pin", metavar="pin")
parser.add_option("-d", "--delay", dest="delay", help="GPIO button delay", metavar="delay", default=5)
parser.add_option("-c", "--command", dest="command", help="Shell command", metavar="command")

(options, args) = parser.parse_args()

gpio_file="/sys/class/gpio/gpio" + str(options.pin) + "/value";
os.system("echo " + str(options.pin) + " > /sys/class/gpio/export");
os.system("echo in > /sys/class/gpio/gpio" + str(options.pin) + "/direction");

started=False;
while True:
    with open(gpio_file,"rb") as f:
        while True:
            c = f.read(1);
            if not c:
                break;
            if c == '1':
                if not started:
                    started = time.time();
                else:
                    if time.time() - started > int(options.delay):
                        print "Run external command";
                        os.system(options.command);
                        started = False
            sleep(0.1);
