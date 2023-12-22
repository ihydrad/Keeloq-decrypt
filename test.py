from keeloq import Keeloq

key = 0x87318dcce381bd59
mail = 0x55318DCCE381BD55

pager = Keeloq(key)
print(hex(pager.decrypt(mail)))
