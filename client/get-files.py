from pathlib import Path
import json

'''
	Ran at the beginning of program
	Reads directory contents in ./sounds
	If updates are made, stores in sounds.json
'''
obj = dict()

sounds_dir = Path('./sounds')
sound_sets = [x for x in sounds_dir.iterdir() if x.is_dir()]
for set in sound_sets:
	set_s = str(set)
	set_sounds = [x for x in set.iterdir()]
	obj[set_s] = []
	for sound in set_sounds:
		sound_s = str(sound)
		obj[set_s].append(sound_s)
with open('sounds.json', 'w') as json_file:
  json.dump(obj, json_file)
#   json.dumps(student, indent=4, separators=(',', ': '), sort_keys=True)