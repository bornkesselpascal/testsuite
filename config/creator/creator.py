import xml.etree.ElementTree as ET
import re

test_name = "newcard_inspur"


def create_client_xml(client_name, client_ip, client_port, client_interface, server_ip, server_port, server_interface):
    # Create the root element
    root = ET.Element("client_description")
    
    # Create 'method' element
    ET.SubElement(root, "method").text = "CUSTOM"

    # Create 'path' element
    ET.SubElement(root, "path").text = f"/testsuite/{client_name}/{test_name}"

    # Create 'duration' element
    duration = ET.SubElement(root, "duration")
    ET.SubElement(duration, "short_duration").text = "7200"
    ET.SubElement(duration, "long_duration").text = "0"

    # Update 'target_connection' with provided client IP and port
    target_connection = ET.SubElement(root, "target_connection")
    ET.SubElement(target_connection, "client_ip").text = client_ip
    ET.SubElement(target_connection, "server_ip").text = server_ip  # server IP is now dynamic
    ET.SubElement(target_connection, "bandwidth_limit").text = ""
    ET.SubElement(target_connection, "port").text = client_port
    ET.SubElement(target_connection, "gap").text = "2500"

    # Create 'datagram' element
    datagram = ET.SubElement(target_connection, "datagram")
    sizes = ET.SubElement(datagram, "sizes")
    for size in ["80", "8900", "65000"]:
        ET.SubElement(sizes, "value").text = size
    ET.SubElement(datagram, "random").text = "false"

    # Create 'interface' element
    interface = ET.SubElement(root, "interface")
    ET.SubElement(interface, "client").text = client_interface
    ET.SubElement(interface, "server").text = server_interface

    # Update 'service_connection' with provided server IP and port
    service_connection = ET.SubElement(root, "service_connection")
    ET.SubElement(service_connection, "server_ip").text = server_ip  # server IP is now dynamic
    ET.SubElement(service_connection, "port").text = server_port
   
    # Create 'client_only' element
    ET.SubElement(root, "client_only").text = "false"

    # Create 'stress' element
    stress = ET.SubElement(root, "stress")
    ET.SubElement(stress, "type").text = "NONE"
    ET.SubElement(stress, "location").text = "LOC_BOTH"

    # Create the XML tree and write it to a file
    tree = ET.ElementTree(root)
    if hasattr(ET, 'indent'):
        ET.indent(tree, space="\t", level=0)  # Indent for pretty print, for Python 3.9 and later
    tree.write(f"client/{client_name}.xml", encoding="utf-8", xml_declaration=True)

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
                    servers[identifier] = (ip, port, interface)

    # Now create an XML file for each client
    for identifier, (client_name, client_ip, client_port, client_interface) in clients.items():
        if identifier in servers:
            server_ip, server_port, server_interface = servers[identifier]
            create_client_xml(client_name, client_ip, client_port, client_interface, server_ip, server_port, server_interface)

# Specify the path to your input file here
input_file = 'pairs.txt'
process_input_file(input_file)
