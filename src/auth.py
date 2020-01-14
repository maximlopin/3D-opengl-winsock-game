import socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.settimeout(3.0)

sock.connect(('127.0.0.1', 25565))
sock.send(b'Hello, world!')
