import ipaddress

class Node:
    def __init__(self):
        self.children = {0: None, 1: None}
        self.route_id = None
        self.is_terminal = False

class TrieLPM:
    def __init__(self):
        self.root = Node()

    def _get_bit(self, ip_int, pos, max_bits):
        return (ip_int >> (max_bits - 1 - pos)) & 1

    def insert(self, cidr_str, route_id):
        network = ipaddress.ip_network(cidr_str)
        ip_int = int(network.network_address)
        prefix_len = network.prefixlen
        max_bits = network.max_prefixlen
        
        curr = self.root
        
        for i in range(prefix_len):
            bit = self._get_bit(ip_int, i, max_bits)
            if not curr.children[bit]:
                curr.children[bit] = Node()
            curr = curr.children[bit]
            
        curr.is_terminal = True
        curr.route_id = route_id

    def lookup(self, ip_str):
        ip_obj = ipaddress.ip_address(ip_str)
        ip_int = int(ip_obj)
        max_bits = ip_obj.max_prefixlen
        
        curr = self.root
        last_route_id = None
        
        for i in range(max_bits):
            if curr.is_terminal:
                last_route_id = curr.route_id
                
            bit = self._get_bit(ip_int, i, max_bits)
            
            if not curr.children[bit]:
                break
                
            curr = curr.children[bit]
            
        # Puxa o Terminal da última correspondência
        if curr and curr.is_terminal:
            last_route_id = curr.route_id
            
        return last_route_id

# Bateria de Validação da Imagem
trie = TrieLPM()

trie.insert("192.168.0.0/16", 10)
trie.insert("192.168.1.0/24", 20)
trie.insert("192.168.1.128/25", 30)
trie.insert("10.0.0.0/8", 40)
trie.insert("0.0.0.0/0", 50)
trie.insert("2001:db8::/32", 100)
trie.insert("2001:db8:a::/48", 200)

print(f"IP 192.168.0.50 -> Rota ID: {trie.lookup('192.168.0.50')}")     # 10
print(f"IP 192.168.1.20 -> Rota ID: {trie.lookup('192.168.1.20')}")     # 20
print(f"IP 192.168.1.150 -> Rota ID: {trie.lookup('192.168.1.150')}")   # 30
print(f"IP 10.255.0.1 -> Rota ID: {trie.lookup('10.255.0.1')}")         # 40
print(f"IP 8.8.8.8 -> Rota ID: {trie.lookup('8.8.8.8')}")               # 50