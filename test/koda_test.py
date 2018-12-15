# -*- coding: utf-8 -*-

import subprocess
from os import listdir
from os.path import isfile, join
import os
from pylab import *
import numpy as np
import matplotlib.pyplot as plt
import filecmp
import math
import re
import matplotlib
import sys  

kodek = 'codec.exe'

normalRawDataDir      = "data_raw_normal_mean_sweep_0_to_255"
normalEncodedDataDir  = "data_encoded_normal_mean_sweep_0_to_255"
normalDecodedDataDir  = "data_decoded_normal_mean_sweep_0_to_255"

laplaceRawDataDir     = "data_raw_laplace_mean_sweep_0_to_255"
laplaceEncodedDataDir = "data_encoded_laplace_mean_sweep_0_to_255"
laplaceDecodedDataDir = "data_decoded_laplace_mean_sweep_0_to_255"

uniformRawDataDir 	  = "data_raw_uniform"
uniformEncodedDataDir = "data_encoded_uniform"
uniformDecodedDataDir = "data_decoded_uniform"

def alphanum_key(s):
    """ Turn a string into a list of string and number chunks.
        "z23a" -> ["z", 23, "a"]
    """
    return [ tryint(c) for c in re.split('([0-9]+)', s) ]

def tryint(s):
    try:
        return int(s)
    except:
        return s

def readFiles(directory):
	file_paths = list()
	for root, dirs, files in os.walk(directory):
	    for fileName in files:
	    	file_paths.append(os.path.join(directory, fileName))
	
	file_paths.sort(key=alphanum_key)
	return file_paths

def generateUniformDistribution(output_path):
	subprocess.call([kodek, "generator", "distribution", "uniform", output_path])

def generateNormalDistribution(output_path, mean=128):
	subprocess.call([kodek, "generator", "distribution", "normal", output_path, str(mean)])

def generateLaplaceDistribution(output_path, mean=128):
	subprocess.call([kodek, "generator", "distribution", "laplace", output_path,str(mean)])

def generateData(distribution_file, output_data_path, number_of_samples):
	subprocess.call([kodek, "generator", "data", distribution_file, output_data_path, number_of_samples])

def encodeData(raw_data_file, encoded_output_file):
	subprocess.call([kodek, "coder", raw_data_file, encoded_output_file, "data"])

def decodeData(encoded_data_file, decoded_output_file):
	subprocess.call([kodek, "decoder", encoded_data_file, decoded_output_file, "data"])

def encodeImage(raw_image_file, encoded_output_file):
	subprocess.call([kodek, "coder", raw_image_file, encoded_output_file, "image"])

def decodeImage(encoded_image_file, decoded_output_file):
	subprocess.call([kodek, "decoder", encoded_image_file, decoded_output_file, "image"])

####################################################################################
#						  		    TESTS										   #
####################################################################################

####################################################################################
#						  DATA GENERATION (10000000 samples)					   #
#																				   #
#							Normal and Laplace distribution 					   #
#							 with mean sweep from 0 to 255						   #
#									  with step 1								   #
####################################################################################
def generateLaplaceData(filename, number_of_samples=1000):
	generateData("laplace_distribution.txt", filename, str(number_of_samples))

def generateNormalData(filename, number_of_samples=1000):
	generateData("normal_distribution.txt", filename, str(number_of_samples))

def generateUniformDataSingle(filename, number_of_samples=1000):
	generateData("uniform_distribution.txt", filename, str(number_of_samples))

def generateLaplaceSweepData(rawDataDir, number_of_samples=1000):
	for i in range (0, 256):
		print "Generating", rawDataDir + "/" + str(i) + ".dat"
		generateLaplaceDistribution("laplace_distribution.txt", mean = str(i))
		generateData("laplace_distribution.txt", rawDataDir + "/" + str(i) + ".dat", str(number_of_samples))

def generateNormalSweepData(rawDataDir, number_of_samples=1000):
	for i in range (0, 256):
		print "Generating", rawDataDir + "/" + str(i) + ".dat"
		generateNormalDistribution("normal_distribution.txt", mean = str(i))
		generateData("normal_distribution.txt", rawDataDir + "/" + str(i) + ".dat", str(number_of_samples))

def generateUniformData(rawDataDir, number_of_data_sets, number_of_samples=1000):
	generateUniformDistribution("uniform_distribution.txt")
	for i in range (0, number_of_data_sets):
		print rawDataDir + "/" + str(i) + ".dat"
		generateData("uniform_distribution.txt", rawDataDir + "/" + str(i) + ".dat", str(number_of_samples))

####################################################################################
#						 		 DATA ENCODING				   					   #
####################################################################################

def encodeMultipleData(rawDataDir, encodedDataDir, number_of_data_sets):
	for i in range (0, number_of_data_sets):
		print "Encoding", rawDataDir + "/" + str(i) + ".dat", "to", encodedDataDir + "/" + str(i) + ".koda"
		encodeData(rawDataDir + "/" + str(i) + ".dat", encodedDataDir + "/" + str(i) + ".koda")

####################################################################################
#						 		 DATA DECODING			   					       #
####################################################################################

def decodeMultipleData(encodedDataDir, decodedDataDir, number_of_data_sets):
	for i in range (0, number_of_data_sets):
		print "Decoding", encodedDataDir + "/" + str(i) + ".koda", "to", decodedDataDir + "/" + str(i) + ".dat"
		decodeData(encodedDataDir + "/" + str(i) + ".koda", decodedDataDir + "/" + str(i) + ".dat")

####################################################################################
#						 		 IMAGES ENCODING				   				   #
####################################################################################

def encodeImages(rawImagesDir, encodedImagesDir):
	raw_images_info = list()
	#Get relative paths to all files
	for root, dirs, files in os.walk(rawImagesDir):
	    for fileName in files:
	    	filename_without_extension = os.path.splitext(fileName)[0]
	    	raw_file_info = {
	    		'relative_path' : os.path.join(rawImagesDir, fileName),
	    		'filename' : fileName,
	    		'filename_noext' : filename_without_extension
	    	}

	        raw_images_info.append(raw_file_info)

	for i in range(0, len(raw_images_info)):
		print int(100 * (float(i) / float(len(raw_images_info)))), "%"
		encodeImage(raw_images_info[i]['relative_path'], os.path.join(encodedImagesDir, raw_images_info[i]['filename_noext']) + ".koda")
	print "100 %"
####################################################################################
#						 		 IMAGES DECODING				   				   #
####################################################################################
def decodeImages(encodedImagesDir, decodedImagesDir):
	encoded_images_info = list()
	#Get relative paths to all files
	for root, dirs, files in os.walk(encodedImagesDir):
	    for fileName in files:
	    	filename_without_extension = os.path.splitext(fileName)[0]
	    	raw_file_info = {
	    		'relative_path' : os.path.join(encodedImagesDir, fileName),
	    		'filename' : fileName,
	    		'filename_noext' : filename_without_extension
	    	}

	        encoded_images_info.append(raw_file_info)

	for i in range(0, len(encoded_images_info)):
		print int(100 * (float(i) / float(len(encoded_images_info)))), "%"
		decodeImage(encoded_images_info[i]['relative_path'], os.path.join(decodedImagesDir, encoded_images_info[i]['filename_noext']) + ".png")
	print "100 %"

####################################################################################
#						 		 DATA EQUALITY TESTING			   				   #
####################################################################################

def compareUniformData():
	return filecmp.cmp('data_raw_uniform/1.dat', 'data_decoded_uniform/1.dat')

def compareSweepData(raw_data_directory, decoded_data_directory):
	raw_data_info = list()
	decoded_data_info = list()

	#Get relative paths to all files
	for root, dirs, files in os.walk(raw_data_directory):
	    for fileName in files:
	    	raw_file_info = {
	    		'relative_path' : os.path.join(raw_data_directory, fileName),
	    		'filename' : fileName
	    	}

	        raw_data_info.append(raw_file_info)

	for root, dirs, files in os.walk(decoded_data_directory):
	    for fileName in files:
	    	decoded_file_info = {
	    		'relative_path' : os.path.join(decoded_data_directory, fileName),
	    		'filename' : fileName
	    	}

	       	decoded_data_info.append(decoded_file_info)
	

	number_of_raw_files = len(raw_data_info)
	number_of_decoded_files = len(decoded_data_info)

	if number_of_raw_files != number_of_decoded_files:
		print "Number of files is not equal!"
	else:
		equal = True

		for i in range(0, number_of_raw_files):
			if not filecmp.cmp(raw_data_info[i]['relative_path'], decoded_data_info[i]['relative_path']):
				print raw_data_info[i]['relative_path'] + " " + "not equal to: " + decoded_data_info[i]['relative_path']
				equal = False
				break;

		return equal

	return False

####################################################################################
#						 		 ENTROPY CALCULATION		   				       #
####################################################################################
def analyzeSingleFile(file_path):
	number_of_zeros = 0
	number_of_ones = 0

	file = open(file_path, 'rb')

	for byte in file.read():
 		for i in xrange(8):
 			bit = (ord(byte) >> i) & 1

			if bit == 0:
				number_of_zeros += 1
			else:
				number_of_ones += 1

   	p0 = float(number_of_zeros) / float(number_of_zeros + number_of_ones)
   	p1 = float(number_of_ones) / float(number_of_zeros + number_of_ones)
   	H = -(p0 * math.log(p0, 2) + p1 * math.log(p1, 2))

   	print file_path, " ", H

	info = {
		'number_of_zeros' : number_of_zeros,
		'number_of_ones' : number_of_ones,
		'total_bits' : number_of_zeros + number_of_ones,
		'p0' : p0,
		'p1' : p1,
		'entropy' : H
	}

	return info

def analyzeMultipleFiles(raw_file_paths, encoded_file_paths):
	multipleFilesEntropyInfo = list()

	totalInfoRaw = {
		'entropy' : list(),
		'zeros' : list(),
		'ones' : list(),
		'number_of_bits' : list()
	}

	totalInfoEncoded = {
		'entropy' : list(),
		'zeros' : list(),
		'ones' : list(),
		'number_of_bits' : list(),
	}

	totalInfo = {
		'entropy' : list(),
		'number_of_bits_encoded' : list(),
		'bits_per_symbol' : list(),
		'compression_ratio' : list(),
	}


	for path in raw_file_paths:
		singleFileInfo = analyzeSingleFile(path)
		totalInfoRaw['entropy'].append(singleFileInfo['entropy'])
		totalInfoRaw['number_of_bits'].append(singleFileInfo['total_bits'])
		totalInfoRaw['zeros'].append(singleFileInfo['number_of_zeros'])
		totalInfoRaw['ones'].append(singleFileInfo['number_of_ones'])

	for path in encoded_file_paths:
		singleFileInfo = analyzeSingleFile(path)
		totalInfoEncoded['entropy'].append(singleFileInfo['entropy'])
		totalInfoEncoded['number_of_bits'].append(singleFileInfo['total_bits'])
		totalInfoEncoded['zeros'].append(singleFileInfo['number_of_zeros'])
		totalInfoEncoded['ones'].append(singleFileInfo['number_of_ones'])


	for i in range(0, len(raw_file_paths)):
		entropy = totalInfoRaw['entropy']
		bitsRaw = totalInfoRaw['number_of_bits'][i]
		bitsEncoded = totalInfoEncoded['number_of_bits'][i]
		onesEncoded = totalInfoEncoded['ones'][i]
		zerosEncoded = totalInfoEncoded['zeros'][i]

		totalInfo['entropy'].append(entropy),
		totalInfo['number_of_bits_encoded'].append(bitsEncoded),
		totalInfo['bits_per_symbol'].append(float(bitsEncoded) / float(bitsRaw)),
		totalInfo['compression_ratio'].append(100 * (1.0 - float(bitsEncoded) / float(bitsRaw)))

	return totalInfo

####################################################################################
#						 		 	  PLOTTING		   				               #
####################################################################################
def plot(title, data1, data2, xlabel, ylabel1, ylabel2, fontsize, color_x, color_y1, color_y2, xlim1, xlim2, ylim11, ylim12, ylim21, ylim22):
	figure, firstAxis = plt.subplots()
	secondAxis = firstAxis.twinx()

	firstAxis.set_title(title)

	firstAxis.set_xlabel(xlabel, color=color_x, fontsize=18)
	firstAxis.set_ylabel(ylabel1, color=color_y1, fontsize=18)
	firstAxis.set_xlim([xlim1,xlim2])
	secondAxis.set_ylabel(ylabel2, color=color_y2, fontsize=18)

	if ylim11 == 0 and ylim12 == 0:
		print data1
		print data2
		firstAxis.set_ylim([min(data1), max(data1)])
	else:
		firstAxis.set_ylim([ylim11, ylim12])

	if ylim21 == 0 and ylim22 == 0:
		secondAxis.set_ylim([min(data2), max(data2)])
	else:
		secondAxis.set_ylim([ylim21, ylim22])
	
	firstAxis.plot(data1, color_y1)
	secondAxis.plot(data2, color_y2)
	

####################################################################################
#						 		 	  TESTS		   				               #
####################################################################################
def dataCoderTest():	
	samples=100000
	print "Generating data..."
	generateNormalSweepData(rawDataDir=normalRawDataDir, number_of_samples=samples)
	generateLaplaceSweepData(rawDataDir=laplaceRawDataDir, number_of_samples=samples)
	generateUniformData(rawDataDir=uniformRawDataDir, number_of_data_sets=100, number_of_samples=samples)

	print "Encoding data..."
	encodeMultipleData(rawDataDir=normalRawDataDir, encodedDataDir=normalEncodedDataDir, number_of_data_sets=256)
	encodeMultipleData(rawDataDir=laplaceRawDataDir, encodedDataDir=laplaceEncodedDataDir, number_of_data_sets=256)
	encodeMultipleData(rawDataDir=uniformRawDataDir, encodedDataDir=uniformEncodedDataDir, number_of_data_sets=100)

	print "Decoding data..."
	decodeMultipleData(encodedDataDir=normalEncodedDataDir, decodedDataDir=normalDecodedDataDir, number_of_data_sets=256)
	decodeMultipleData(encodedDataDir=laplaceEncodedDataDir, decodedDataDir=laplaceDecodedDataDir, number_of_data_sets=256)
	decodeMultipleData(encodedDataDir=uniformEncodedDataDir, decodedDataDir=uniformDecodedDataDir, number_of_data_sets=100)

	print "Comparing data..."
	print compareUniformData()
	print compareSweepData(normalRawDataDir, normalDecodedDataDir)
	print compareSweepData(laplaceRawDataDir, laplaceDecodedDataDir)
	
	totalInfoNormal  = analyzeMultipleFiles(readFiles(normalRawDataDir),  readFiles(normalEncodedDataDir))
	totalInfoLaplace = analyzeMultipleFiles(readFiles(laplaceRawDataDir), readFiles(laplaceEncodedDataDir))
	totalInfoUniform = analyzeMultipleFiles(readFiles(uniformRawDataDir), readFiles(uniformEncodedDataDir))

	entropyNormal = totalInfoNormal['entropy']
	############### PLOT FOR NORMAL DISTRIBUTION ##############
	plot("Number of output bits vs input entropy for \n Normal distribution, mean sweep from 0 do 255 \n input bits: " + str(samples) + " samples * 8 bits = " + str(samples * 8),
		  totalInfoNormal['entropy'][0], totalInfoNormal['number_of_bits_encoded'], "Mean", "Entropy", "Bits", 16, 'r', 'b', 'g', 
		  0, 255, 0, 0, 0, 0)
	
	plot("bits / symbol vs input entropy for \n Normal distribution, mean sweep from 0 do 255",
		  totalInfoNormal['entropy'][0], totalInfoNormal['bits_per_symbol'], "Mean", "Entropy", "Bits / Symbol", 16, 'r', 'b', 'g', 
		  0, 255, 0, 0, 0, 1.5)

	plot("Compression ratio [%] vs input entropy for \n Normal distribution, mean sweep from 0 do 255",
		  totalInfoNormal['entropy'][0], totalInfoNormal['compression_ratio'],   "Mean", "Entropy", "Compression ratio [%]", 16, 'r', 'b', 'g',
		  0, 255, 0, 0, 0, 0)

	############### PLOT FOR LAPLACE DISTRIBUTION ##############
	plot("Number of output bits vs input entropy for \n Laplace distribution, mean sweep from 0 do 255 \n input bits: " + str(samples) + " samples * 8 bits = " + str(samples * 8),
		  totalInfoLaplace['entropy'][0], totalInfoLaplace['number_of_bits_encoded'], "Mean", "Entropy", "Bits", 16, 'r', 'b', 'g',
		  0, 255, 0, 0, 0, 0)

	plot("bits / symbol vs input entropy for \n Laplace distribution, mean sweep from 0 do 255",
		  totalInfoLaplace['entropy'][0], totalInfoLaplace['bits_per_symbol'], "Mean", "Entropy", "Bits / Symbol", 16, 'r', 'b', 'g',
		  0, 255, 0, 0, 0, 1.5)

	plot("Compression ratio [%] vs input entropy for \n Laplace distribution, mean sweep from 0 do 255",
		  totalInfoLaplace['entropy'][0], totalInfoLaplace['compression_ratio'], "Mean", "Entropy", "Compression ratio", 16, 'r', 'b', 'g',
		  0, 255, 0, 0, 0, 0)

	plot("Number of output bits vs entropy for \n Uniform distribution \n input bits: " + str(samples) + " samples * 8 bits = " + str(samples * 8),
		  totalInfoUniform['entropy'][0], totalInfoUniform['number_of_bits_encoded'], "Dataset", "Entropy", "Bits", 16, 'r', 'b', 'g',
		  0, 100, 0, 1.5, 0, 0)

	plot("bits / symbol vs input entropy for \n Uniform distribution",
		  totalInfoUniform['entropy'][0], totalInfoUniform['bits_per_symbol'], "Dataset", "Entropy", "Bits / Symbol", 16, 'r', 'b', 'g',
		  0, 100, 0, 1.5, 0, 1.5)

	plot("Compression ratio [%] vs input entropy for \n Uniform distribution",
		  totalInfoUniform['entropy'][0], totalInfoUniform['compression_ratio'], "Dataset", "Entropy", "Compression ratio [%]", 16, 'r', 'b', 'g',
		  0, 100, 0, 1.5, -5, 5)

	plt.show()

def imagesTest():
	a = datetime.datetime.now()
	encodeImages("images_raw", "images_encoded")
	b = datetime.datetime.now()
	print (b-a).microseconds
	a = datetime.datetime.now()
	decodeImages("images_encoded", "images_decoded")
	b = datetime.datetime.now()
	print (b-a).microseconds

	print "Analyzing output data..."
	
	totalInfoImages = analyzeMultipleFiles(readFiles("images_raw"), readFiles("images_encoded"))


	plot("Images",
		  totalInfoImages['entropy'][0], totalInfoImages['number_of_bits_encoded'], "Image", "Entropy", "Bits", 16, 'r', 'b', 'g',
		  0, 100, 0, 1.0, 0, 0)

	plot("Images",
		  totalInfoImages['entropy'][0], totalInfoImages['bits_per_symbol'], "Image", "Entropy", "Bits / Symbol", 16, 'r', 'b', 'g',
		  0, 100, 0, 1.0, -0.5, 1.5)

	plot("Images",
		  totalInfoImages['entropy'][0], totalInfoImages['compression_ratio'], "Image", "Entropy", "Compression ratio [%]", 16, 'r', 'b', 'g',
		  0, 100, 0, 1.0, 0, 100)

	plt.show()

def generationDataTimeTest():
	generateLaplaceDistribution("laplace_distribution.txt", mean = str(128))
	generateNormalDistribution("normal_distribution.txt", mean = str(128))
	generateUniformDistribution("uniform_distribution.txt")

	execution_times = list()
	averaging = 100 # Averages 100x times time calculation
	for i in range(0, averaging):
		execution_times.append(list())

	samples = np.linspace(0, 100000, num=100)

	for j in range(0, averaging):
		for i in range(0, len(samples)):
			print i
			a = datetime.datetime.now()
			generateLaplaceData("laplace_data.dat", number_of_samples=samples[i])
			b = datetime.datetime.now()
			diff = ((b-a).microseconds) / 1000.0 # miliseconds
			execution_times[j].append(diff)

	exec_time = list()

	for j in range (0, len(samples)):
		time_sum = 0.0
		for i in range(0, averaging):
			time_sum += execution_times[i][j]

		exec_time.append(float(time_sum) / float(averaging))


	plt.plot(samples, exec_time, label='Laplace')
	###########
	execution_times = list()
	for i in range(0, averaging):
		execution_times.append(list())

	samples = np.linspace(0, 100000, num=100)

	for j in range(0, averaging):
		for i in range(0, len(samples)):
			print i
			a = datetime.datetime.now()
			generateNormalData("normal_data.dat", number_of_samples=samples[i])
			b = datetime.datetime.now()
			diff = ((b-a).microseconds) / 1000.0 # miliseconds
			execution_times[j].append(diff)

	exec_time = list()

	for j in range (0, len(samples)):
		time_sum = 0.0
		for i in range(0, averaging):
			time_sum += execution_times[i][j]

		exec_time.append(float(time_sum) / float(averaging))

	plt.plot(samples, exec_time, label='Normal')
	##########
	execution_times = list()
	for i in range(0, averaging):
		execution_times.append(list())

	samples = np.linspace(0, 100000, num=100)

	for j in range(0, averaging):
		for i in range(0, len(samples)):
			print i
			a = datetime.datetime.now()
			generateUniformDataSingle("uniform_data.dat", number_of_samples=samples[i])
			b = datetime.datetime.now()
			diff = ((b-a).microseconds) / 1000.0 # miliseconds
			execution_times[j].append(diff)

	exec_time = list()

	for j in range (0, len(samples)):
		time_sum = 0.0
		for i in range(0, averaging):
			time_sum += execution_times[i][j]

		exec_time.append(float(time_sum) / float(averaging))

	plt.plot(samples, exec_time, label='Uniform')

	legend = plt.legend(loc='upper center', shadow=True, fontsize='x-large')

	legend.get_frame().set_facecolor('#FFFFFF')
	plt.show()

#defEncodingTimeTest():

#generationDataTimeTest();

#dataCoderTest()

imagesTest()