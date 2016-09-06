import time
import sys
import os
import subprocess
import subpar

absPath = "E:\\Praktyki\\Work\\Source"
#absPath = "C:\\x265_Transcoder\\Source"
threads_amount = 1

ones_to_throw = 0 #range(0,10,1)
throw_prop = range(0, 25, 25)

#   Short name				Full name									Depth	Frames	FR	Class  	Res
inputData = {
	"Crew"				:['Crew_1280x704',								8,	60,	25,	"A",	[1280,704]]
	#"Traffic"		 	:['Traffic_2560x1600_30_8bit',						8,	150,	30,	"A",	[2560,1600]],
	#"PeopleOnStreet" 	:['PeopleOnStreet_2560x1600_30_8bit',			 	8,	150,	30,	"A",	[2560,1600]],
	#"Nebuta"			:['NebutaFestival_2560x1600_60_10bit',		 		10,	300,	60,	"A",	[2560,1600]],
	#"SteamLocomotive"	:['SteamLocomotiveTrain_2560x1600_60_10bit', 		10,	300,	60,	"A",	[2560,1600]],
	
	#"Kimono1"			:['Kimono1_1920x1080_24_8bit',						8,	240,	24,	"B",	[1920,1080]],
	#"ParkScene"		:['ParkScene_1920x1080_24_8bit',						8,	240,	24,	"B",	[1920,1080]],
	#"Cactus"			:['Cactus_1920x1080_50_8bit',						8,	500,	50,	"B",	[1920,1080]],
	#"BQTerrace"			:['BQTerrace_1920x1080_60_8bit',					8,	600,	60,	"B",	[1920,1080]],
	#"BasketballDrive"	:['BasketballDrive_1920x1080_50_8bit',				8,	500,	50,	"B",	[1920,1080]],
	
	#"RaceHorses"		:['RaceHorses_832x480_30_8bit',						8,	300,	30,	"C",	[832,480]],
	#"BQMall"			:['BQMall_832x480_60_8bit',							8,	600,	60,	"C",	[832,480]],
	#"PartyScene"		:['PartyScene_832x480_50_8bit',						8,	25,	50,	"C",	[832,480]],          
	#"BasketballDrill"	:['BasketballDrill_832x480_50_8bit',				8,	500,	50,	"C",	[832,480]],
	
	#"RaceHorsesLow"	:['RaceHorses_416x240_30_8bit',							8,	300,	30,	"D",	[416,240]],
	#"BQSquare"			:['BQSquare_416x240_60_8bit',						8,	600,	60,	"D",	[416,240]],
	#"BlowingBubbles"	:['BlowingBubbles_416x240_50_8bit',					8,	500,	50,	"D",	[416,240]],
	#"BasketballPass" 	:['BasketballPass_416x240_50_8bit',					8,	500,	50,	"D",	[416,240]],
	
	#"FourPeople"		:['FourPeople_1280x720_60_8bit',					8,	600,	60,	"E",	[1280,720]],
	#"Johnny"			:['Johnny_1280x720_60_8bit',						8,	600,	60,	"E",	[1280,720]],
	#"KristenAndSara"	:['KristenAndSara_1280x720_60_8bit',				8,	600,	60,	"E",	[1280,720]],
	
	#"BasketballDrillText":['BasketballDrillText_832x480_50_8bit',			8,	500,	50,	"F",	[832,480]],
	#"ChinaSpeed"		:['ChinaSpeed_1024x768_30_8bit',					8,	500,	30,	"F",	[1024,768]],
	#"SlideEditing"		:['SlideEditing_1280x720_30_8bit',					8,	300,	30,	"F",	[1280,720]],
	#"SlideShow" 		:['SlideShow_1280x720_20_8bit',						8,	500,	20,	"F",	[1280,720]]
}




class Process:
    proc = None
    stdout = None

class ProcManager:
    def __init__(self, threads):
        self.mth = threads  # max threads
        self.ath = 0        # active threads
        self.procs = []

    def freeCompleted(self):
        for p in self.procs:
            ret = p.proc.poll()
            if ret != None:
                self.ath -= 1
                if p.stdout:
                    p.stdout.close()
                self.procs.remove(p)
                
    def runProc(self, program, stdout=None, wait=False, polltime = 1):
        while not self._runProc(program, stdout , wait):
            time.sleep(polltime)
                    
    def _runProc(self, program, stdout=None, wait=False):
        self.freeCompleted()
        if len(self.procs) == self.mth: return False
        self.procs.append(Process())
        if stdout:
            self.procs[-1].stdout = open(stdout,'w')
        self.procs[-1].proc = subprocess.Popen(program, stdout=self.procs[-1].stdout)
        if wait == True:
            while self.procs[-1].proc.poll() is None:
                time.sleep(0.5)
            if self.procs[-1].stdout:
                self.procs[-1].stdout.close()
            self.procs = self.procs[:-1]
        else:
            self.ath += 1
        return True	

		
		
parameters = dict()
pm = ProcManager(threads_amount)


#first pass

for seq, seqlen in inputData.items():
	for k in range(len(throw_prop)):
		parameters = inputData[seq]
		input = " --input " + absPath+ "\\" + parameters[4] + "\\" + parameters[0] + ".yuv"
		output = " --output " + absPath+ "\\" + parameters[4] + "\\" + parameters[0] + "\\tp" + str(throw_prop[k]) +"\\"+ "encoded_" + parameters[0] + ".h265"
		transcoderPath = absPath+ "\\" + parameters[4] + "\\" + parameters[0] + "\\tp" + str(throw_prop[k]) + "\\data\\bData"
		transcoder = " --transcoder-file " +  transcoderPath
			
		if not os.path.exists(transcoderPath):
			os.makedirs(transcoderPath)
				
		cmd = input + output + transcoder + " --transcoder-mode 0" + " --ones-throw " + str(ones_to_throw) + " --throw-prop " + str(throw_prop[k])
		cmd += " --input-res " + str(parameters[5][0])+"x"+str(parameters[5][1]) + " --frames " + str(parameters[2]) + " --fps " + str(parameters[3])
			
		test = str.split(cmd)
		conf = [" " + t for t in test]
			
		pm.runProc(["Tools/x265.exe", conf], stdout = "sto_"+seq+".txt", wait=False)
pm.freeCompleted()

#second pass
#for seq,seqlen in inputData.items():
#	for k in range(len(throw_prop)):
#		parameters = inputData[seq]
#		input = " --input " + absPath+ "\\" + parameters[4] + "\\" + parameters[0] + ".yuv"
#		output = " --output " + absPath+ "\\" + parameters[4] + "\\" + parameters[0] + "\\tp" + str(throw_prop[k]) +"\\"+ "transcoded_" + parameters[0] + ".h265"
#		transcoderPath = absPath+ "\\" + parameters[4] + "\\" + parameters[0] + "\\tp" + str(throw_prop[k]) + "\\data\\bData"
#		transcoder = " --transcoder-file " +  transcoderPath
		
#		if not os.path.exists(transcoderPath):
#			os.makedirs(transcoderPath)
			
#		cmd = input + output + transcoder + " --transcoder-mode 1" + " --ones-throw " + str(ones_to_throw) + " --throw-prop " + str(throw_prop[k])
#		cmd += " --input-res " + str(parameters[5][0])+"x"+str(parameters[5][1]) + " --frames " + str(parameters[2]) + " --fps " + str(parameters[3])
		
#		test = str.split(cmd)
#		conf = [" " + t for t in test]
		
#		pm.runProc(["Tools/x265.exe", conf], stdout = "sto_"+seq+".txt", wait=False)		
#pm.freeCompleted()