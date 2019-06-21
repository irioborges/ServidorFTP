ftp: apaga
	gcc -o ftp ftp.c -w -pthread
apaga:
	rm -f ftp
