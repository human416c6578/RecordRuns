import os
import paramiko
import re
from ftplib import FTP, error_perm

webserver = {'host' : '127.0.0.1', 'username': 'root', 'password': '', 'port': 21}
bhop = {'host' : '127.0.0.1', 'username': 'root', 'password': '', 'port': 21}  

ssh_client = paramiko.SSHClient()
ssh_client.set_missing_host_key_policy(paramiko.AutoAddPolicy())

webserver_path = 'www/uploads/recording'
replays_file_path = 'cstrike/addons/amxmodx/data/replays.txt'

def format_time(time_str):
    minutes, seconds_ms = time_str.split(':')
    seconds, milliseconds = seconds_ms.rstrip('s').split('.')
    return int(minutes) * 60000 + int(seconds) * 1000 + int(milliseconds)

def extract_time(line, pattern=r'HEADER "(.*?)" "(.*?)" "\[(.*?)\]"'):
    match = re.search(pattern, line)
    return match.group(2) if match else None

def convert_to_dict(input_str):
    map_str, category_str = input_str.strip().split('" "')
    map_name = map_str.strip('"')
    category = category_str.strip('"\n')
    return {'map': map_name, 'category': category}

# convert already downloaded replays to an array
def parse_replays(download_path):
    replays = []
    maps = os.listdir(download_path)
    for map in maps:
        records = os.listdir(f"{download_path}/{map}")
        for record in records:
            line = open(f"{download_path}/{map}/{record}", encoding='utf8').readlines()[0]
            time = extract_time(line)
            replays.append({'path': f"{download_path}/{map}/{record}", 'map': map, 'category': record.replace(".rec", ""), 'time': format_time(time)})
    
    return replays

# download and converts replays from server to an array
def download_replays(download_path, host, port, username, password):
    try:
        replays = []
        if not os.path.exists(f"{download_path}"):
            os.makedirs(f"{download_path}")
        ssh_client.connect(host, port, username, password)

        sftp = ssh_client.open_sftp()

        file_exists = True
        try:
            sftp.stat(replays_file_path)
        except FileNotFoundError:
            file_exists = False
        if not file_exists:
            return

        sftp.get(replays_file_path, 'temp/temp.txt')
        replays_lines = open('temp/temp.txt').readlines()
        replays_list = [convert_to_dict(item) for item in replays_lines]

        for replay in replays_list:
            if not os.path.exists(f"{download_path}/{replay['map']}"):
                os.makedirs(f"{download_path}/{replay['map']}")
            print(f"Downloading... {replay['map']}/{replay['category']}.rec")
            sftp.get(f"cstrike/addons/amxmodx/data/recording/{replay['map']}/{replay['category']}.rec", f"{download_path}/{replay['map']}/{replay['category']}.rec")
            line = open(f"{download_path}/{replay['map']}/{replay['category']}.rec", encoding='utf8').readlines()[0]
            time = extract_time(line)
            if time:
                replays.append({'path': f"{download_path}/{replay['map']}/{replay['category']}.rec", 'map': replay['map'], 'category': replay['category'], 'time': format_time(time)})
        return replays
    finally:
        sftp.remove(replays_file_path)
        if 'sftp' in locals():
            sftp.close()
        ssh_client.close()

# upload downloaded replays to webserver
def upload_replays(replays, host, port, username, password):
    try:
        if not os.path.exists("temp"):
            os.makedirs("temp")
        ftp = FTP(host)
        ftp.login(username, password)

        for replay in replays:
            replay_exists = True
            time = 99999999
            try:
                localfile = open('temp/temp.rec', 'wb')
                ftp.retrbinary(f'RETR {webserver_path}/{replay["map"]}/{replay["category"]}.rec', localfile.write, 1024)
                localfile.close()
            
                line = open("temp/temp.rec", encoding='utf8').readlines()[0]

                time = extract_time(line)
                time = format_time(time)
            except error_perm:
                replay_exists = False
                localfile.close()

            if time <= replay["time"] and replay_exists:
                print(f"Skip... {replay['map']} {replay['category']} {replay['time']}")
                continue
            
            try:
                ftp.mkd(f'{webserver_path}/{replay["map"]}')
            except:
                print("Directory Exists")
            print(f"Uploading... {replay['map']}/{replay['category']} - {replay['time']}")
            ftp.storbinary(f'STOR {webserver_path}/{replay["map"]}/{replay["category"]}.rec', open(replay["path"], 'rb'))
    finally:
        if 'ftp' in locals():
            ftp.quit()  # Close the FTP connection

replays_bhop = download_replays("folder", bhop['host'], bhop['port'], bhop['username'], bhop['password'])
if replays_bhop:
    upload_replays(replays_bhop, webserver['host'], webserver['port'], webserver['username'], webserver['password'])