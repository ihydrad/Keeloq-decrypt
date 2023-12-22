from keeloq import Keeloq

mail = 0x55318DCCE381BD55
key = 0x87318DCCE381BD59
data = 0xE381BD59

pager = Keeloq(key)
button = pager.get_button_code(mail)
serial = pager.get_serial(mail)
hop = pager.encript(data)
mail = pager.build_mail(button, serial, hop)
dec = pager.decrypt(mail)
print("mail:", hex(mail))
print("Decrypted: " + hex(dec))
