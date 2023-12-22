from keeloq import Keeloq

mail = 0x55318DCCE381BD55
key = 0x87318DCCE381BD59
data = 0xE381BD59

pager = Keeloq(key)
print("Data: " + hex(data))
mail = pager.build_mail(pager.get_button_code(mail), pager.get_serial(mail), pager.encript(data))
dec = pager.decrypt(mail)
print("mail:", hex(mail))
print("Decrypted: " + hex(dec))

