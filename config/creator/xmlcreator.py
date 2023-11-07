import xml.etree.ElementTree as ET
from parameters import test_name, test_duration, test_cycletime, test_datagrams

def create_client_xml(client_name, client_ip, client_port, client_interface, server_ip, server_port, server_interface):
    # Create the root element
    root = ET.Element("client_description")
    
    # Create 'method' element
    ET.SubElement(root, "method").text = "CUSTOM"

    # Create 'path' element
    ET.SubElement(root, "path").text = f"/testsuite/{client_name}/{test_name}"

    # Create 'duration' element
    duration = ET.SubElement(root, "duration")
    ET.SubElement(duration, "short_duration").text = test_duration  
    ET.SubElement(duration, "long_duration").text = "0"

    # Update 'target_connection' with provided client IP and port
    target_connection = ET.SubElement(root, "target_connection")
    ET.SubElement(target_connection, "client_ip").text = client_ip
    ET.SubElement(target_connection, "server_ip").text = server_ip  # server IP is now dynamic
    ET.SubElement(target_connection, "bandwidth_limit").text = ""
    ET.SubElement(target_connection, "port").text = client_port
    ET.SubElement(target_connection, "gap").text = test_cycletime

    # Create 'datagram' element
    datagram = ET.SubElement(target_connection, "datagram")
    sizes = ET.SubElement(datagram, "sizes")
    for size in test_datagrams:
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


def create_server_xml(server_name, server_ip, server_port):
    # Create the root element
    root = ET.Element("server_description")

    # Create 'path' element
    ET.SubElement(root, "path").text = f"/testsuite"

    # Update 'service_connection' with provided server IP and port
    service_connection = ET.SubElement(root, "service_connection")
    ET.SubElement(service_connection, "server_ip").text = server_ip
    ET.SubElement(service_connection, "port").text = server_port

    # Create the XML tree and write it to a file
    tree = ET.ElementTree(root)
    if hasattr(ET, 'indent'):
        ET.indent(tree, space="\t", level=0)  # Indent for pretty print, for Python 3.9 and later
    tree.write(f"server/{server_name}.xml", encoding="utf-8", xml_declaration=True)