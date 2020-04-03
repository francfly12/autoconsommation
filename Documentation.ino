/*
 * Documentation
 * I have used an Arduino 101 because the format is similar to an arduino UNO and the EmonTX shield adapts perfectly
 * And because I HAD one, it is now discontinued
 * UNO cannot be used because it has not enough memory, needs about 80 K
 * Should work on other brands of Arduinos as well, if the memory is there and Pins not used by other resources.
 * Also Freq is 32 Mhz,
 * Display is a simple OLED. performs power diversion. no external data sent, unless for testing.
 * 
 * How to wire the circuit : 
 * 
 * A0 Pin Analog 0   VOLTSPIN, EmonTXshield, connected to voltage sensing
 * A1     CT1    current sensing on circuit 1, used for main power from the grid
 * A2     CT2    current sensing on circuit 2, used to sense PV generation
 * A3     CT3    current sensing on circuit 3, used to monitor a secondary circuit, for display only
 * A4     reserved for I2C,  pin to the shield has to be cut because it disables I2C 
 * A5     reserved for I2C,  same as above  ,
 * SDA    This is the same as pin A4, connected to SDA of OLED display
 * SCL    This is the same as pin A5, connected to SCL of OLED display
 * D0 to D4  Not implemented, Radio module need to be not installed on the shield
 * D5     Reserved, not implemented yet but will be used later to input a temperature sensing (hot water temp)
 * D6     PWM, also called triac command, but used to control water heater with excess electricity(diversion). PWM from 0 to 100%
 * D7  to D9   not used
 * D10    Red pin of the tricolor LED     
 * D11    Green pin of LED
 * D12    Blue pin of LED
 * Reset  wired to the ground through the reset pushbutton on the front face, resets everything
 * 
 * LED flashes once per second. colour depends on consumption ; red for power from the grid, blue when solar is generated
 *        red intensity depens on amount of consumption
 *        in addition green permanent when Diversion operates, dimmed depending on PWM
 *      OLED connected via SDA/SCL  uses 5V or 3.3V from arduino but power quality has to be checked, use a correct power supply with capacitor because       
 *      these displays are sensitive or generate power transients on the +5V
 *      Here, the driver is an SH1106, pure chinese
 *      
 * Calibration  -  See EmontX for calibration
 *    I had to calibrate the voltage sensing. Read the Voltage with a voltmeter and adapt the value (265 in my case, depends on the transformer you use
 *    the I calibrated the values for CT1, CT2, CT3. Each sensor slightly different, 
 *    I had to adapt the values by reading actual consumption from a wattmeter
 *    but you can also use measurement of a known consumer (water boiler) to adapt these
 *    
 *    For better operation, You need to modify in the program some constants to fit your installation :
 *    MaxDump = 2200 ; // used to compute % of power diverted, 
 *                     // i.e. a water heater of 2200 W dumps 100% of the load
 *    MaxConsumption = 9000 ; // max power rated for the total installation, used to show red led
 *    int MaxSolar = 300 ; // maximum power of your solar arrays in watts, used to show blue led
 *    Min_Solar = 10; // used to setup minimum for Blue LED
 *    
 *    
 */
