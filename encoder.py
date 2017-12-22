#  Created by Prateek Joshi on 10/3/17
#  Copyright © 2017 Prateek Joshi. All rights reserved.
#  CS 8821

import binascii
import codecs
from bs4 import BeautifulSoup
from collections import OrderedDict
import re
import pdb

def find_pairs(tag):
	keys = list(map(str, tag.attrs.keys()))
	keys.sort()
	keys_length = len(keys)
	pairs_length = keys_length / 2
	pairs = {}
	paired_keys = []
	for i in range(0, pairs_length):
		pairs[i] = { "primary": keys[i], "secondary": keys[keys_length - 1 - i]}
		paired_keys.extend([keys[i], keys[keys_length - 1 - i]])
	unpaired = [key for key in keys if key not in paired_keys]
	return pairs, pairs_length, unpaired

def new_attributes(old_attributes, pairs, pairs_length, unpaired, bits_to_encode):
	new_attributes = OrderedDict()
	for i in range(0, pairs_length):
		primary_key_unicode   = pairs[i]['primary'].decode('unicode-escape')
		secondary_key_unicode = pairs[i]['secondary'].decode('unicode-escape')
		if i >= len(bits_to_encode) or bits_to_encode[i] == '1':
			new_attributes[primary_key_unicode]   = old_attributes[primary_key_unicode]
			new_attributes[secondary_key_unicode] = old_attributes[secondary_key_unicode]
		elif bits_to_encode[i] == '0':
			new_attributes[secondary_key_unicode] = old_attributes[secondary_key_unicode]
			new_attributes[primary_key_unicode]   = old_attributes[primary_key_unicode]
	for key in unpaired:
		new_attributes[key.decode('unicode-escape')] = old_attributes[key.decode('unicode-escape')]
	return new_attributes

def build_tag_string(tag, new_attributes):
	temp_string = "<{0}".format(tag.name)
	for key, value in new_attributes.items():
		key = str(key)
		if isinstance(value, list):
			value = ' '.join(list(map(str, value)))
		else:
			value = value.encode('utf-8')
		temp_string = temp_string + ' ' + key + '="' + value + '"'
	temp_string1 = temp_string + '>'
	temp_string2 = temp_string + '/>'
	return temp_string1, temp_string2	

def max_message_size(soup):
	pairs_count = 0
	for tag in soup.findAll():
		pairs, pairs_length, unpaired = find_pairs(tag)	
		pairs_count = pairs_count + pairs_length
	return pairs_count / 8

print
print "********************* *********************** *********************"
print "********************* HTML STEGANOGRAPHY TOOL *********************"
print "********************* *********************** *********************"
print

input_file_name = raw_input("Name of input html file: ")
output_file_name = raw_input("Name of output html file: ")

with codecs.open(input_file_name, 'r', 'utf-8') as html_file:
	soup = BeautifulSoup(html_file, "lxml")

	print "Processing file ..."
	print "maximum size of message that can be hidden: {0} characters".format(max_message_size(soup))

	message = raw_input("Input message to hide. Note: End the message with character '~': ")
	temp_binary_message = bin(int(binascii.hexlify(message),16))[2:]
	binary_message = '0' * (8 - (len(temp_binary_message) % 8)) + temp_binary_message

	message_pointer_start = 0
	max_pointer_position  = len(binary_message) - 1

	master_hash = {}
	master_hash_counter = 0


	for tag in soup.findAll():
		if message_pointer_start > max_pointer_position:
			break
		pairs, pairs_length, unpaired = find_pairs(tag)

		if pairs_length < 1:
			continue

		message_pointer_end = message_pointer_start + pairs_length
		bits_to_encode      = binary_message[message_pointer_start:message_pointer_end]
		
		new_attrs = new_attributes(tag.attrs, pairs, pairs_length, unpaired, bits_to_encode)

		tag_string1, tag_string2 = build_tag_string(tag, new_attrs)
		tag.attrs["a"]="{0}".format(str(master_hash_counter))
		tag.attrs["zzz"]="{0}".format(str(master_hash_counter))

		master_hash[master_hash_counter] = { 
			'tag_name': tag.name, 
			'replace_string1': tag_string1,
			'replace_string2': tag_string2,
			'regex1': '<' + tag.name + ' a="' + str(master_hash_counter) + '".*' + 'zzz="' + str(master_hash_counter) + '"\s*>',
			'regex2': '<' + tag.name + ' a="' + str(master_hash_counter) + '".*' + 'zzz="' + str(master_hash_counter) + '"\s*/>'
		}
		print 
		print "bits_to_encode: {0}".format(bits_to_encode)
		print "{0}".format(master_hash_counter)
		print master_hash[master_hash_counter]['replace_string1']
		print
		print "message_pointer_start: {0}".format(message_pointer_start)
		print "message_pointer_end: {0}".format(message_pointer_end)
		print

		message_pointer_start = message_pointer_end
		master_hash_counter = master_hash_counter + 1

	soup_str = str(soup)
	for i in range(0, master_hash_counter):
		soup_str = re.sub(master_hash[i]['regex1'], master_hash[i]['replace_string1'], soup_str, flags=re.DOTALL)
		soup_str = re.sub(master_hash[i]['regex2'], master_hash[i]['replace_string2'], soup_str, flags=re.DOTALL)

	with open(output_file_name, "w") as output_file:
		output_file.write(soup_str)
