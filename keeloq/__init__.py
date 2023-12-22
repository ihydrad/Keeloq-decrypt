def bit(data, bitnum):
    return (((data)>>(bitnum)) & 1)


class KeeloqCrypt:

    KeeLoqNLF = 0x3A5C742E

    def __init__(self, key: int, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.key = key

    @staticmethod   
    def g5(x, a, b, c, d, e):
        return (bit(x, a) + bit(x, b)*2 + bit(x, c)*4 + bit(x, d)*8 + bit(x, e)*16)

    def decrypt(self, x: int):
        for r in range(528):
            x = ((x<<1)&0xFFFFFFFF)^(bit(x, 31) ^ bit(x, 15) ^ bit(self.key, (15-r)&63)^bit(self.KeeLoqNLF, self.g5(x, 0, 8, 19, 25, 30)))
        return x

    def encrypt(self, x: int):
        """Keeloq Encrypt funcion. Put data for encrypt here."""     
        for r in range(528):
            x = ((x>>1)&0xFFFFFFFF)^((bit(x, 0) ^ bit(x, 16) ^ bit(self.key, r&63)^bit(self.KeeLoqNLF, self.g5(x, 1, 9, 20, 26, 31))) << 31)            
        return x  

    def __repr__(self):
        return "key: " + str(hex(self.key))\


class KeeloqFormat:

    @staticmethod
    def build_mail(button, serial, hop):
        return (button<<60) + (serial<<32) + hop

    @staticmethod
    def get_hop(mail):
        return (mail & 0xFFFFFFFF)

    @staticmethod
    def get_button_code(mail):
        return (mail >> 32) >> 28

    @staticmethod
    def get_serial(mail):
        return (mail >> 32) & 0xFFFFFFF 


class Keeloq(KeeloqFormat, KeeloqCrypt):

    def decrypt(self, mail):      
        return KeeloqCrypt.decrypt(self, self.get_hop(mail))        

    def encript(self, data):        
        return KeeloqCrypt.encrypt(self, data)
