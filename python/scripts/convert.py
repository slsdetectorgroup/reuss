import subprocess
path = '/data/jfarrival/users/psi/test'

for i in [9]:
    subprocess.run(['python', 'convert_data.py', f'{i:03d}', '-p', f'{path}'])
