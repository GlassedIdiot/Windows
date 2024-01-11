import subprocess

# Save current global username and email
old_username = subprocess.check_output("git config --global user.name", shell=True).decode().strip()
old_email = subprocess.check_output("git config --global user.email", shell=True).decode().strip()

# Set new username and email
subprocess.call('git config --global user.name "GlassedIdiot"', shell=True)
subprocess.call('git config --global user.email "rehmanareeb69@gmail.com"', shell=True)

# Push commits
subprocess.call('git push origin main', shell=True)

# Restore old username and email
subprocess.call(f'git config --global user.name "{old_username}"', shell=True)
subprocess.call(f'git config --global user.email "{old_email}"', shell=True)
