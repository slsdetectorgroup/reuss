import subprocess
path = '/zwilag/users/mahmoudi/psi/20210811_ZSM5_fresh'

for i in [2]:
    subprocess.run(['python', 'convert_data.py', f'{i:03d}', '-p', f'{path}'])
