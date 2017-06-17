import utils, sys
from pydbg import *
from pydbg.defines import *

# Python 2.7

'''
BOOL WINAPI WriteFile(
    _In_        HANDLE hFile,
    _In_        LPCVOID lpBuffer,
    _In_        DWORD nNumberOfBytesToWrite,
    _Out_opt_   LPDWORD lpNumberOfBytesWritten,
    _Inout_opt_ LPOVERLPAPPED lpOverlapped
    );
'''

sDbg = pydbg()
sIsProcess = False
sProcessName = "notepad.exe"

gOriginPattern = "love"
gReplacePattern = "hate"


def replaceString( aDbg, aArgs ):
    sBuffer = aDbg.read_process_memory( aArgs[1], aArgs[2] )

    if gOriginPattern in sBuffer:
        print "[APIHooking] Before : %s" % sBuffer
        sBuffer = sBuffer.replace( gOriginPattern, gReplacePattern )
        sReplace = aDbg.write_process_memory( aArgs[1], sBuffer )

        print "[APIHooking] After : %s" \
        % aDbg.read_process_memory( aArgs[1], aArgs[2] )

    return DBG_CONTINUE


# ���μ��� id ����Ʈ ���
for( pid, name ) in sDbg.enumerate_processes():   
    if name.lower() == sProcessName:
        sIsProcess = True
        sHooks = utils.hook_container()

        # ���μ��� �ڵ� ���ϱ�.
        '''
        ���⼭ DebugActiveProcess(pid): �������� �ʴ� ��û�Դϴ�. ������ ���� ��찡 �ִ�.
        �̴� ��ŷ Ÿ�ٰ� ��Ŀ�� ���� ��Ʈ�� �޶� �ߴ� �����̴�.
        ���⼭ notepad �� ��� C:\Windows\SysWOW64\notepad.exe �� �����Ű�� �ذ�ȴ�.
        SysWOW64 �� 32��Ʈ ���μ����� ���� ��
        '''
        sDbg.attach( pid )
        print" Saves a process handle in self.h_process of pid[%d]" % pid

        # �ߴ����� ��ġ�� �Լ��� �ּ� ���ϱ�
        sHookAddress = sDbg.func_resolve_debuggee( "kernel32.dll", "WriteFile" )

        if sHookAddress:
            # �ߴ��� ����
            sHooks.add( sDbg, sHookAddress, 5, replaceString, None )
            print "sets a breakpoint at the designated address : 0x%08x" \
            % sHookAddress

            break
        else:
            print "[Error] : couldn`t resolv hook address"
            sys.exit( -1 )

if sIsProcess:
    print "waiting for occurring debugger event"
    sDbg.run()
else:
    print "[Error] : There in no process [%s]" % sProcessName
    sys.exit( -1 )
    

        
