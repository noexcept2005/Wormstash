#include <Windows.h>
NTSTATUS NTAPI RtlAdjustPrivilege(ULONG, BOOLEAN, BOOLEAN, PBOOLEAN);
NTSTATUS NTAPI NtRaiseHardError(LONG, ULONG, ULONG,
    PULONG_PTR, ULONG, PULONG );
int main()
{
	BOOLEAN bl;
    ULONG Response;
    return 0;
	RtlAdjustPrivilege(19, TRUE, FALSE, &bl);
	NtRaiseHardError(STATUS_ILLEGAL_INSTRUCTION, 0, 0, NULL, 6, &Response); 
	return 0;
}
