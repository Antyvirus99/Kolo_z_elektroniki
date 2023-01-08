void mfrc522_init()
{
	uint8_t byte;
	mfrc522_reset();
	
	mfrc522_write(TModeReg, 0x8D);			//w³¹cz timer autpomatyczny 0x80 oraz 0X0d to czêœæ preskalera
    mfrc522_write(TPrescalerReg, 0x3E);		//pozosta³a czêœæ presklera z rozkazu powy¿ej - st¹ preskaler jest ustawiony na 0xd3e czyli 3390
    mfrc522_write(TReloadReg_1, 30);   		//wartoœæ reload timera bajt High
    mfrc522_write(TReloadReg_2, 0);			//wartoœæ reload timera bajt Low czyli Reload ustawione na 30*256 + 0 = 
	mfrc522_write(TxASKReg, 0x40);			//w³¹cza Force100ASK
	mfrc522_write(ModeReg, 0x3D);			// - zobacz w dokumentacji
	
	byte = mfrc522_read(TxControlReg);		
	if(!(byte&0x03))
	{
		mfrc522_write(TxControlReg,byte|0x03);	// w³¹cz wyjœcia TX1 i TX2
	}
}