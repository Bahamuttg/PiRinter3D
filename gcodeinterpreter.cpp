#include "gcodeinterpreter.h"
#include "thermalprobe.h"
#include "steppermotor.h"
#include "motorcontroller.h"

GCodeInterpreter::GCodeInterpreter(const QString &FilePath, const QString &Logpath,
                                   StepperMotor *XMotor, StepperMotor *YMotor, StepperMotor *ZMotor, StepperMotor *ExtMotor,
                                   ThermalProbe *BedProbe, ThermalProbe *ExtruderProbe)
{

}

GCodeInterpreter::~GCodeInterpreter()
{

}
//======================================Private Methods============================================
    QList<Coordinate>  GCodeInterpreter::GetCoordValues(QString &)
    {
        /*
     xchar_loc=lines.index('X');
    i=xchar_loc+1;
    while (47<ord(lines[i])<58)|(lines[i]=='.')|(lines[i]=='-'):
        i+=1;
    x_pos=float(lines[xchar_loc+1:i]);

    ychar_loc=lines.index('Y');
    i=ychar_loc+1;
    while (47<ord(lines[i])<58)|(lines[i]=='.')|(lines[i]=='-'):
        i+=1;
    y_pos=float(lines[ychar_loc+1:i]);

    extchar_loc=lines.index('E');
    i=extchar_loc+1;
    while (47<ord(lines[i])<58)|(lines[i]=='.')|(lines[i]=='-'):
        i+=1;
    ext_pos=float(lines[extchar_loc+1:i]);
    */
    }
    void GCodeInterpreter::MoveToolHead(const float &XPosition, const float &YPosition, const float &ZPosition, const float &ExtPosition)
    {
        int stepx = 0, stepy = 0, stepz = 0, stepext = 0;
        if(XPosition != 0)
            stepx = qFloor((XPosition/_XRes) + 0.5) - _XMotor->Position;
        if(YPosition != 0)
            stepy = qFloor((YPosition/_YRes) + 0.5) - _YMotor->Position;
        if(ZPosition != 0)
            stepz = qFloor((ZPosition/_ZRes) + 0.5) - _ZMotor->Position;
        if(ExtPosition != 0)
            stepext = qFloor((ExtPosition/_ExtRes) + 0.5) - _ExtMotor->Position;

        float total_steps = qSqrt((qPow(stepx, 2) + qPow(stepy, 2)));
        float total_3dsteps = 0;

        if(total_steps > 0 && total_3dsteps > 0 && stepext > 0)
             MotorController::StepMotors(_XMotor, stepx, _YMotor, setpy, _ZMotor, stepz, _ExtMotor, stepext, _SpeedFactor / qMin(_XRes, _YRes));
        else if(total_steps > 0 && stepext > 0)
            MotorController::StepMotors(_XMotor, stepx, _YMotor, setpy, _ExtMotor, stepext, _SpeedFactor / qMin(_XRes, _YRes));
        else if(total_steps > 0)
            MotorController::StepMotors(_XMotor, stepx, _YMotor, setpy, _SpeedFactor / qMin(_XRes, _YRes));
        else
        {
            if(XPosition > 0)
                MotorController::StepMotor(_XMotor, stepx, _SpeedFactor / _XRes);
            else
                MotorController::StepMotor(_YMotor, stepy, _SpeedFactor / _YRes);
        }
    }

//======================================End Private Methods========================================
void GCodeInterpreter::ParseLine()
{

}

/*

################################################################################################
################################################################################################
#################                                ###############################################
#################    G code reading Functions    ###############################################
#################                                ###############################################
################################################################################################
################################################################################################

def writeToLog(outputText):
    with open('tempout.txt','a') as f:
        f.write(outputText)
    print outputText

#these functions are for debugging purposes only
def sampleHeaters(extThermPin,heatbeadThermPin):
    sampleHeaterDutyCycle(extThermPin, "Extruder")
    sampleHeaterDutyCycle(heatbeadThermPin, "Heated Bed")

def sampleHeaterDutyCycle(pin, name):
    writeToLog("Testing "+ name +" Temperature\n");
    highTime = get555PulseHighTime(pin);
    writeToLog(name+ " Thermistor 555 Timer High Pulse Time "+ str(highTime)+"\n")
    estTemp = getTempFromTable(pin)
    writeToLog(name+ " Estimated Tempurature "+ str(estTemp)+"\n")

def get555PulseHighTime(pin):
    counter = 0;
    GPIO.wait_for_edge(pin, GPIO.RISING);
    while GPIO.input(pin) == GPIO.HIGH:
        counter += 1;
        time.sleep(0.001); # may try to change this to 0.0001 for more resolution
    return float(counter);

#This function takes in the current temp and name of heater and returns the current average
 #of the last three tempurature readings.  This avoids issues with reading spikes
def getAverageTempFromQue(temp, name):
    retTemp = 0;
    if(name == "Extruder"):
        if(len(extTempQue) > 2):
            extTempQue.pop();
        extTempQue.appendleft(temp)
        retTemp = sum(extTempQue)/len(extTempQue);
    else:
        if(len(heatBedTempQue) > 2):
            heatBedTempQue.pop();
        heatBedTempQue.appendleft(temp)
        retTemp = sum(heatBedTempQue)/len(heatBedTempQue);
    return float(retTemp);

#this function gets the rise time from a pin(thermistor pin) from the 555 timer out and cross reference with
#tempurature table to return the estimated current temperature of the cooresponding heater.
def getTempFromTable(pin):
    pulseHighTime = get555PulseHighTime(pin);
    estTemp = 0;
    #read from tempurate text file and return estimated temp from pulse time
    linectr = 0;
    for lines in open('Thermistor555TimerTempChart.txt','r'):
        if linectr > 5:
            lineSplit = lines.split();
            if float(lineSplit[2]) <= pulseHighTime:
                estTemp = lineSplit[1];
                break
        linectr += 1;
        #if estTemp == 0: #causing temp sensing error
            #estTemp = 250; #more than max temp
    return float(estTemp);

#polling tempurature and setting to +/- 20degC of supplied tempfrom GCode
def checkTemps():
    curExtTemp = getAverageTempFromQue(getTempFromTable(ExtThermistor), "Extruder");#getTempFromTable(ExtThermistor);
    curHeatBedTemp = getAverageTempFromQue(getTempFromTable(HeatBedThermistor), "HeatBed");#getTempFromTable(HeatBedThermistor);
    if (curExtTemp - 5) >= extTemp:
        GPIO.output(ExtHeater, False);
    elif(curExtTemp + 5) <= extTemp:
        GPIO.output(ExtHeater, True);
    if (curHeatBedTemp - 5) >= heatBedTemp:
        GPIO.output(HeatBed, False);
    elif(curHeatBedTemp + 5) <= heatBedTemp:
        GPIO.output(HeatBed, True);

def PenOff(ZMotor):
    # move ZAxis ~5 steps up
    ZMotor.move(1,5)

def PenOn(ZMotor):
    # move ZAxis ~5 steps down
    ZMotor.move(-1,5)



def homeAxis(motor,endStopPin):
    #need to home the axis
    #step axis until endstop is triggered
    while GPIO.input(endStopPin) == GPIO.HIGH:
        motor.move(-1,1);
    #then change endstop GPIOs to output to re-enable the motors and fire of 1-5 steps
    #in the opposite direction.  Note* this is first time I've changed the function of GPIO(input to output)
    #in the middle of a program.  I'm not sure if this is even possible but I'm assuming it is.
    GPIO.setup(endStopPin,GPIO.OUT);
    GPIO.output(endStopPin, True);
    motor.move(1,5);
    #Then step endstop GPIO back to input.
    GPIO.output(endStopPin, False);
    GPIO.setup(endStopPin,GPIO.IN,pull_up_down=GPIO.PUD_DOWN); # pull_up_down=GPIO.PUD_UP  or pull_up_down=GPIO.PUD_DOWN


###########################################################################################
###########################################################################################
#################                           ###############################################
#################    Main program           ###############################################
#################                           ###############################################
###########################################################################################
###########################################################################################
#TODO  G28, M107, M106
#GCode defintion reference: http://reprap.org/wiki/G-code
#Bug - motion is slow on XY moves when steps are ~50 or more on each, speed issue?
#TODO threading for temperature management. For a quick fix now we call checktemps every 25th Ext move
try:#read and execute G code
    lineCtr = 1;
    heaterCheck = 1;
    for lines in open(filename,'r'):
        print 'processing line# '+str(lineCtr)+ ': '+lines;
        lineCtr += 1;
        if lines==[]:
            1; #blank lines
        elif lines[0:3]=='G90':
            print 'start';
        elif lines[0:3]=='G92':
            print 'Reset Extruder to 0';
            MExt.position = 0;

        elif lines[0:3]=='G20':# working in inch;
            dx/=25.4;
            dy/=25.4;
            print 'Working in inch';

        elif lines[0:3]=='G21':# working in mm;
            print 'Working in mm';

        elif lines[0:3]=='G28': # homing all axis
            print 'Homing all axis...';
            #move till endstops trigger
            print 'Homing X axis...';
            homeAxis(MX,EndStopX)
            print 'Homing Y axis...';
            homeAxis(MY,EndStopY)
            print 'Homing Z axis...';
            homeAxis(MZ,EndStopZ)

        elif lines[0:3]=='M05':
            PenOff(MZ)
            #GPIO.output(Laser_switch,False);
            print 'Pen turned off';

        elif lines[0:3]=='M03':
            PenON(MZ)
            #GPIO.output(Laser_switch,True);
            print 'Pen turned on';

        elif lines[0:3]=='M02':
            GPIO.output(Laser_switch,False);
            print 'finished. shuting down';
            break;
        elif lines[0:4]=='M104': #Set Extruder Temperature
            #note that we should just be setting the tempurature here, but because this always fires before M109 call
            #I'm just turning the extruder on as well because then it can start heating up
            extTemp = float(SinglePosition(lines,'S'));
            print 'Extruder Heater On and setting temperature to '+ str(extTemp) +'C';
            GPIO.output(ExtHeater,True);
            sampleHeaters(ExtThermistor,HeatBedThermistor);
        elif lines[0:4]=='M106': #Fan on
            #for now we will just print the following text
            print 'Fan On';
        elif lines[0:4]=='M107': #Fan off
            #for now we will just print the following text
            print 'Fan Off';
        elif lines[0:4]=='M109':  #Set Extruder Temperature and Wait
            #need to set temperature here and wait for correct temp as well
            #for now we will just turn on extruderheater
            #I would like to this all with the raspberry pi but...
            #I may use a simple Arduino(Uno) sketch to handle tempurature regulation
            #Doing with the RaspPi only would require polling the tempurature(maybe at each Z axis move?)
            print 'Extruder Heater On';
            GPIO.output(ExtHeater,True);
            extTemp = float(SinglePosition(lines,'S'));
            print 'Extruder Heater On and setting temperature to '+ str(extTemp) +'C';
            print 'Waiting to reach target temp...';
            sampleHeaters(ExtThermistor,HeatBedThermistor);
            temp = getTempFromTable(ExtThermistor)
            while temp < extTemp:
                time.sleep(0.2);
                temp = getAverageTempFromQue(getTempFromTable(ExtThermistor), "Extruder");
                print str(temp);

        elif lines[0:4]=='M140': #Set Heat Bed Temperature
            #need to set temperature here as well
            #for now we will just turn on extruderheater
            heatBedTemp = float(SinglePosition(lines,'S'));
            print 'Setting Heat Bed temperature to '+ str(heatBedTemp) +'C';

        elif lines[0:4]=='M190':  #Set HeatBed Temperature and Wait
            #need to set temperature here and wait for correct temp as well
            #for now we will just turn on HeatBedheater
            #I would like to this all with the raspberry pi but...
            #I may use a simple Arduino(Uno) sketch to handle tempurature regulation
            #Doing with the RaspPi only would require polling the tempurature(maybe at each Z axis move?)
            heatBedTemp = float(SinglePosition(lines,'S'));
            print 'HeatBed Heater On';
            print 'Setting HeatBed temperature to '+ str(heatBedTemp) +'C and waiting';
            GPIO.output(HeatBed,True);
            sampleHeaters(ExtThermistor,HeatBedThermistor);
            temp = getTempFromTable(HeatBedThermistor)
            while temp < heatBedTemp:
                time.sleep(0.2);
                temp = getAverageTempFromQue(getTempFromTable(HeatBedThermistor), "HeatBed");
                print str(temp);

        elif (lines[0:3]=='G1F')|(lines[0:4]=='G1 F'):
            1;#do nothing
        elif (lines[0:3]=='G0 ')|(lines[0:3]=='G1 ')|(lines[0:3]=='G01'):#|(lines[0:3]=='G02')|(lines[0:3]=='G03'):
            #linear engraving movement
            if (lines[0:3]=='G0 '):
                engraving=False;
            else:
                engraving=True;

            #Update F Value(speed) if available
            if(lines.find('F') >= 0):
                speed = (SinglePosition(lines,'F')/60)/min(dx,dy);  #getting F value as mm/min so we need to convert to mm/sec then calc and update speed

            if(lines.find('E') < 0 and lines.find('Z') < 0):
                [x_pos,y_pos]=XYposition(lines);
                moveto(MX,x_pos,dx,MY,y_pos,dy,speed,engraving);
            elif(lines.find('X') < 0 and lines.find('Z') < 0): #Extruder only
                ext_pos = SinglePosition(lines,'E');
                stepsExt = int(round(ext_pos/dext)) - MExt.position;
                #TODO fix this extMotor Delay
                Motor_control_new.Single_Motor_Step(MExt,stepsExt,30);
                #still need to move Extruder using stepExt(signed int)
            elif(lines.find('X') < 0 and lines.find('E') < 0): #Z Axis only
                print 'Moving Z axis only';
                z_pos = SinglePosition(lines,'Z');
                stepsZ = int(round(z_pos/dz)) - MZ.position;
                Motor_control_new.Single_Motor_Step(MZ,stepsZ,60);
                #check Extruder and Heat Bed temp after Z axiz move
                checkTemps();
            else:
                [x_pos,y_pos,ext_pos]=XYExtposition(lines);
                movetothree(MX,x_pos,dx,MY,y_pos,dy,MExt,ext_pos,dext,speed,engraving);
                heaterCheck += 1;
                #create new moveto function to include Extruder postition

        elif (lines[0:3]=='G02')|(lines[0:3]=='G03'): #circular interpolation
            old_x_pos=x_pos;
            old_y_pos=y_pos;

            ext_pos = 0;

            #still need to add code here to handle extrusion info from the line if it is available
            if(lines.find('E') >= 0):
                #get E value as well as the rest
                [x_pos,y_pos]=XYposition(lines);
                [i_pos,j_pos,ext_pos]=IJEposition(lines);
            else:
                [x_pos,y_pos]=XYposition(lines);
                [i_pos,j_pos]=IJposition(lines);

            xcenter=old_x_pos+i_pos;   #center of the circle for interpolation
            ycenter=old_y_pos+j_pos;


            Dx=x_pos-xcenter;
            Dy=y_pos-ycenter;      #vector [Dx,Dy] points from the circle center to the new position

            r=sqrt(i_pos**2+j_pos**2);   # radius of the circle

            e1=[-i_pos,-j_pos]; #pointing from center to current position
            if (lines[0:3]=='G02'): #clockwise
                e2=[e1[1],-e1[0]];      #perpendicular to e1. e2 and e1 forms x-y system (clockwise)
            else:                   #counterclockwise
                e2=[-e1[1],e1[0]];      #perpendicular to e1. e1 and e2 forms x-y system (counterclockwise)

            #[Dx,Dy]=e1*cos(theta)+e2*sin(theta), theta is the open angle

            costheta=(Dx*e1[0]+Dy*e1[1])/r**2;
            sintheta=(Dx*e2[0]+Dy*e2[1])/r**2;        #theta is the angule spanned by the circular interpolation curve

            if costheta>1:  # there will always be some numerical errors! Make sure abs(costheta)<=1
                costheta=1;
            elif costheta<-1:
                costheta=-1;

            theta=arccos(costheta);
            if sintheta<0:
                theta=2.0*pi-theta;

            no_step=int(round(r*theta/dx/5.0));   # number of point for the circular interpolation
            extruderMovePerStep = 0;
            if ext_pos != 0:
                extruderMovePerStep = (ext_pos - MExt.position)/no_step;

            for i in range(1,no_step+1):
                tmp_theta=i*theta/no_step;
                tmp_x_pos=xcenter+e1[0]*cos(tmp_theta)+e2[0]*sin(tmp_theta);
                tmp_y_pos=ycenter+e1[1]*cos(tmp_theta)+e2[1]*sin(tmp_theta);
                if extruderMovePerStep == 0:
                    moveto(MX,tmp_x_pos,dx,MY, tmp_y_pos,dy,speed,True);
                else:
                    movetothree(MX,tmp_x_pos,dx,MY, tmp_y_pos,dy,MExt,MExt.position+extruderMovePerStep,dext,speed,True);
        if heaterCheck >= 10: #checking every fifth extruder motor move
            checkTemps();
            heaterCheck = 0;
            print 'Checking Temps';

except KeyboardInterrupt:
    pass

#shut off heaters
GPIO.output(outputs, False);
#PenOff(MZ);   # turn off laser
moveto(MX,0,dx,MY,0,dy,50,False);  # move back to Origin

MX.unhold();
MY.unhold();
MZ.unhold();

GPIO.cleanup();
*/
