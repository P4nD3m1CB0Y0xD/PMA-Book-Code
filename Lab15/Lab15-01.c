#include <stdio.h>
#include <string.h>

void _ConstantCondition(void)
{
	__asm {
		xor eax, eax
		jz _END
		_emit 0e8h
	_END:
	}
}

int main(int argc, char* argv[])
{
	if (argc == 2)
	{
		_ConstantCondition();
		if (argv[1][0] == 'p')
		{
			_ConstantCondition();
			if (argv[1][2] == 'q')
			{
				_ConstantCondition();
				if (argv[1][1] == 'd')
				{
					_ConstantCondition();
					printf_s("Good Job!\n");
				}
			}
		}
	}
	else
	{
		_ConstantCondition();
		printf_s("Son, I am disappoint.\n");
	}


	return 0;
}
