#!/bin/python

import os
import ROOT

path = "../Data/Data_Readout_Current"
file_list = os.listdir(path)
file_list = [file for file in file_list if file.endswith(".txt")]

if os.path.isfile("Data_List.dat"):
    os.remove("Data_List.dat")
    
f_data = open("Data_List.dat", "wt")

for file in file_list:
    print(file)
    f_data.write(file+"\n")

f_data.close()
#ROOT.gSystem.Load("/home/iyoon/GEM_Aging/Build/libGEM_Aging.so")


#ROOT.Syncer()
