import subprocess
path = '/fast_raid0_md0/test'

for i in [2]:
    subprocess.run(['python', 'convert_data.py', f'{i:03d}', '-p', f'{path}'])