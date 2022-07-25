import subprocess
path = '/data/jungfrau/users/JF_JEM_calibration/focusing_correction'

#for i in [0]:
for i in range(0,1):
    subprocess.run(['python', 'convert_data.py', f'{i:03d}', '-p', f'{path}'])
