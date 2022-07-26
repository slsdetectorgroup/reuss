import subprocess
path = '/data/jungfrau/users/JF_JEM_calibration/conversion_test/'

#for i in [0]:
for i in range(0,2):
    subprocess.run(['python', 'convert_data.py', f'{i:03d}', '-p', f'{path}'])
