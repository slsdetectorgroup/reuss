import subprocess
path = '/data/jungfrau/psi/Kleitz/CILM012'

# explicit numbers, separated by commas
# for i in [0, 31]:
# range (a, b): 1st number a: first existing directory
#               2nd number b: one beyond last existing directory
#        ( a <= i < b )
for i in range(0,4):
    subprocess.run(['python', 'convert_data.py', f'{i:03d}', '-p', f'{path}'])
