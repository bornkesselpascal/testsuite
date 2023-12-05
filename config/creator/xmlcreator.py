import xml.etree.ElementTree as ET
from parameters import test_name, test_duration, test_cycletime, test_datagrams, test_type, test_dynamic, test_dyn_mode, test_dyn_min, test_dyn_max, test_dyn_steps, test_latency

def create_client_xml(client_name, client_ip, client_port, client_interface, server_ip, server_port, server_interface):
    # Create the root element
    root = ET.Element("client_description")

    # Create 'path' element
    ET.SubElement(root, "path").text = f"/testsuite/{client_name[7:]}/{test_name}"

    # Create 'duration' element
    ET.SubElement(root, "duration").text = test_duration

    if test_dynamic:
        dynamic_behavior = ET.SubElement(root, "dynamic_behavior")
        ET.SubElement(dynamic_behavior, "mode").text = test_dyn_mode
        ET.SubElement(dynamic_behavior, "min").text = test_dyn_min
        ET.SubElement(dynamic_behavior, "max").text = test_dyn_max
        ET.SubElement(dynamic_behavior, "steps").text = test_dyn_steps

    # Update 'target_connection' with provided client IP and port
    target_connection = ET.SubElement(root, "target_connection")
    ET.SubElement(target_connection, "type").text = test_type
    ET.SubElement(target_connection, "client_ip").text = client_ip
    ET.SubElement(target_connection, "server_ip").text = server_ip
    ET.SubElement(target_connection, "port").text = client_port
    ET.SubElement(target_connection, "cycletime").text = test_cycletime

    # Create 'datagram' element
    datagram = ET.SubElement(target_connection, "datagram_sizes")
    for size in test_datagrams:
        ET.SubElement(datagram, "size").text = size

    # Create 'interface' element
    interface = ET.SubElement(root, "interface")
    ET.SubElement(interface, "client").text = client_interface
    ET.SubElement(interface, "server").text = server_interface

    # Update 'service_connection' with provided server IP and port
    service_connection = ET.SubElement(root, "service_connection")
    ET.SubElement(service_connection, "server_ip").text = server_ip
    ET.SubElement(service_connection, "port").text = server_port

    # Create 'stress' element
    stress = ET.SubElement(root, "stress")
    ET.SubElement(stress, "type").text = "NONE"
    ET.SubElement(stress, "num").text = "0"
    ET.SubElement(stress, "location").text = "LOC_BOTH"

    # Create 'latency_measurement' element
    ET.SubElement(root, "latency_measurement").text = test_latency

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