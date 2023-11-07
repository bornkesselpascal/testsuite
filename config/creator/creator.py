import os
import re
from xmlcreator import create_client_xml, create_server_xml

def process_input_file(input_file):
    clients = {}
    servers = {}

    # First read all clients and servers and store them by identifier
    with open(input_file, 'r') as file:
        for line in file:
            parts = line.strip().split(' @ ')
            if len(parts) == 2:
                name, rest = parts
                identifier = re.search(r'(_\w+)', name).group(1)
                ip, rest = rest.split(':')
                port, interface = rest.split(' ')
                    
                if 'client' in name:
                    clients[identifier] = (name, ip, port, interface)
                elif 'server' in name:
                    servers[identifier] = (name, ip, port, interface)

    # Create client and server directories if not already present
    if not os.path.exists('client'):
        os.makedirs('client')
    if not os.path.exists('server'):
        os.makedirs('server')

    # Now create an XML file for each client
    for identifier, (client_name, client_ip, client_port, client_interface) in clients.items():
        if identifier in servers:
            server_name, server_ip, server_port, server_interface = servers[identifier]
            create_client_xml(client_name, client_ip, client_port, client_interface, server_ip, server_port, server_interface)
            create_server_xml(server_name, server_ip, server_port)

# Specify the path to your input file here
input_file = 'support/pairs.txt'
process_input_file(input_file)
