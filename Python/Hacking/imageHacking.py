
sFName = "img.bmp"
sNewFName = "imgNew.bmp"

# ���� ���� �б� ���� ���
sFile = open( sFName, "r+b" )
sBuf = sFile.read()
# ��ũ��Ʈ ���� �� ������ �߻���ų �� �ִ� */ ���ڴ� �������� ġȯ
sBuf.replace( b'\x2A\x2F', b'\x00\x00' )
sFile.close()

sFile = open( sNewFName, "w+b" )
sFile.write( sBuf )
# �б� Ŀ�� �������� �ڷ� 2����Ʈ �̵�. ó�� 2 ����Ʈ�� ��Ʈ�� ������ �ĺ��ϴµ� ���Ǵ� ���� �ѹ�
sFile.seek( 2, 0 )
'''
��Ʈ�� ������ �ĺ��ϱ� ���� ���Ǵ� �����ѹ� �ڿ� �ּ����� ������ �ǹ����� /* �� ����
�������� ���� �ѹ��� �ν��ϸ� ������ �����Ϳ� �Ϻ� �ջ��� �߻��ϴ��� ��Ʈ�� ������
���������� ���� �� �ִ�.
'''
sFile.write( b'\x2F\x2A' )
sFile.close()

sFile = open( sNewFName, "a+b" )
sFile.write( b'\xFF\x2A\x2F\x3D\x31\x3B' )
sFile.write( open ( 'hello.js', 'rb' ).read() )
sFile.close()
