 public class Keeloq{
 	// ФОРМУЛЫ ИЗ ВИКИПЕДИИ
 	public static UInt32 KeeLoq_NLF = 0x3A5C742E;
 
 public static UInt64 bit(UInt64 x, int n){ 
 	return (((x) >> (n)) & 1); 
 }
 
 public static UInt64 g5(UInt64 x, int a, int b, int c, int d, int e){
 	return (bit(x,a)+bit(x,b)*2+bit(x,c)*4+bit(x,d)*8+bit(x,e)*16);
 }
 
 public static UInt32 Keeloq_Decrypt(UInt64 mail, UInt64 key){
 	UInt64 x = mail;
 	int r;
 	for (r = 0; r < 528; r++){
 		x = (x << 1) ^ bit(x, 31) ^ bit(x, 15) ^ bit(key, (15 - r) & 63) ^ bit(KeeLoq_NLF, (int)g5(x, 0, 8, 19, 25, 30));
 	}
 	return (UInt32)x; 
 }
 
 public static UInt32 KeeLoq_Encrypt(UInt32 data, UInt64 key){
 	UInt64 x = data;
 	int r;
 	for (r = 0; r < 528; r++){
 		x = (x >> 1) ^ ((bit(x, 0) ^ bit(x, 16) ^(UInt32) bit(key, r & 63) ^ bit(KeeLoq_NLF, (int)g5(x, 1, 9, 20, 26, 31))) << 31);
 	}
 	return (UInt32)x;
 }
 
 // формат кекса в строчку
 public static string formatStringFromHex(UInt64 hex){ 
 	return String.Format("0x{0:X}\n", hex); }
 } 