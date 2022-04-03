int32_t Cli_CmdDump(uint8_t *args)
{
	int32_t			argc = 1;
	uint8_t			*p = args;
	uint8_t			arg[UART_CMD_USUALMAXLEN];
	static uint32_t	begaddr = 0, count = 16, ix;
	int32_t			fl = 0;

	while (*(p = Cli_FindNextArgv(p)) != '\0') {
		Cli_GetArgv(arg, p);

		switch(argc) {
		case 1:
			begaddr = strtol((const char*)arg, NULL, 16);
			break;

		case 2:
			count = strtol((const char*)arg, NULL, 16);

			if (count > 0x40) {
				count = 0x40;
			}

			break;

		default:
			;
		}

	argc++;
	}

	Cli_Printf("HEX dump of the memory (0x%02X bytes from 0x%08X):\r\n", count, begaddr);
	fl = 1;

	for (ix = begaddr; ix < (begaddr + count); ix += 4) {
		if ((ix != begaddr) && (ix % 16 == 0)) {
			fl = 1;
			Cli_Printf("\r\n");
		}

		if (fl) {
			Cli_Printf(" %08X: ", ix);
			fl = 0;
		}

		Cli_Printf("%08X ", *((uint32_t *)ix));
	}

	return UART_SUCCESS;
}