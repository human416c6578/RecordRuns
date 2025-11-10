import os
import paramiko
import struct
from ftplib import FTP, error_perm

webserver = {'host' : '127.0.0.1', 'username': 'root', 'password': '', 'port': 21}
bhop = {'host' : '127.0.0.1', 'username': 'root', 'password': '', 'port': 21}  

ssh_client = paramiko.SSHClient()
ssh_client.set_missing_host_key_policy(paramiko.AutoAddPolicy())

webserver_path = 'www/uploads/recording'
replays_file_path = 'cstrike/addons/amxmodx/data/replays.txt'

def extract_header(file_path):
	try:
		with open(file_path, 'rb') as f:
			# Read the first 133 bytes
			packed_header = f.read(133)
			if len(packed_header) < 133:
				raise ValueError("Header data is incomplete or corrupted.")

			# Extract timestamp (8 bytes)
			timestamp = struct.unpack('>Q', packed_header[:8])[0]

			# Extract version (2 bytes)
			version = struct.unpack('>H', packed_header[8:10])[0]

			# Extract map name (32 bytes, null-terminated string)
			map_name = packed_header[10:42].decode('utf-8').split('\0', 1)[0]

			# Extract time (3 bytes)
			time = (packed_header[42] << 16) | (packed_header[43] << 8) | packed_header[44]

			# Extract player name (32 bytes, null-terminated string)
			player_name = packed_header[45:77].decode('utf-8').split('\0', 1)[0]

			# Extract SteamID (24 bytes, null-terminated string)
			steam_id = packed_header[77:101].decode('utf-8').split('\0', 1)[0]

			# Extract additional info (32 bytes, null-terminated string)
			additional_info = packed_header[101:133].decode('utf-8').split('\0', 1)[0]

			return {
				'timestamp': timestamp,
				'version': version,
				'map': map_name,
				'time': time,
				'player_name': player_name,
				'steam_id': steam_id,
				'info': additional_info
			}

	except Exception as e:
		print(f"Error reading header: {e}")
		return None

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
			header = extract_header(f"{download_path}/{map}/{record}")
			replays.append({'path': f"{download_path}/{map}/{record}", 'map': map, 'category': header['info'], 'time': header['time']})
	
	return replays

# download and converts replays from server to an array
def download_replays(download_path, host, port, username, password):
	try:
		# Create a transport object
		transport = paramiko.Transport((host, port))
		
		# Connect using password authentication
		transport.connect(username=username, password=password)
		
		# Open an SFTP session
		sftp = paramiko.SFTPClient.from_transport(transport)
		
		file_exists = True
		try:
			sftp.stat(replays_file_path)
		except FileNotFoundError:
			file_exists = False
		if not file_exists:
			return
		
		sftp.get(replays_file_path, 'temp/temp.txt')
		sftp.remove(replays_file_path)
		replays_lines = open('temp/temp.txt').readlines()
		replays_list = [convert_to_dict(item) for item in replays_lines]

		replays = []
		
		for replay in replays_list:
			if not os.path.exists(f"{download_path}/{replay['map']}"):
				os.makedirs(f"{download_path}/{replay['map']}")
			print(f"Downloading... {replay['map']}/{replay['category']}.rec")
			sftp.get(f"cstrike/addons/amxmodx/data/recording/{replay['map']}/{replay['category']}.rec", f"{download_path}/{replay['map']}/{replay['category']}.rec")
			header = extract_header(f"{download_path}/{replay['map']}/{replay['category']}.rec")
			print(header)
			if header['time']:
				replays.append({'path': f"{download_path}/{replay['map']}/{replay['category']}.rec", 'map': replay['map'], 'category': replay['category'], 'time': header['time']})

		sftp.close()
		transport.close()

		return replays
		
	except Exception as e:
		print(f"Error connecting to SFTP server: {e}")

# upload downloaded replays to webserver
def upload_replays(replays, host, port, username, password):
	try:
		if not os.path.exists("temp"):
			os.makedirs("temp")
		ftp = FTP(host)
		ftp.login(username, password)

		for replay in replays:
			replay_exists = True

			time = 9999999
			try:
				localfile = open('temp/temp.rec', 'wb')
				ftp.retrbinary(f'RETR {webserver_path}/{replay["map"]}/{replay["category"]}.rec', localfile.write, 1024)
				localfile.close()
			
				header = extract_header("temp/temp.rec")
				time = header['time']
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

replays_bhop = download_replays("bhop", bhop['host'], bhop['port'], bhop['username'], bhop['password'])
if replays_bhop:
	upload_replays(replays_bhop, webserver['host'], webserver['port'], webserver['username'], webserver['password'])