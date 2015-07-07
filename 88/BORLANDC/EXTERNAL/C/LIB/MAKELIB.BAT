bcc -mt! -c -I..\ memcmpff.c
bcc -mt! -c -I..\ memcpyff.c
bcc -mt! -c -I..\ memcpyfn.c
bcc -mt! -c -I..\ retint.c  
bcc -mt! -c -I..\ retnstrf.c
bcc -mt! -c -I..\ retnstrn.c
bcc -mt! -c -I..\ retnxdtf.c
bcc -mt! -c -I..\ retnxdtn.c
bcc -mt! -c -I..\ retreal.c 
bcc -mt! -c -I..\ retstrf.c 
bcc -mt! -c -I..\ retstrn.c 
bcc -mt! -c -I..\ strcpyff.c
bcc -mt! -c -I..\ strcpyfn.c
bcc -mt! -c -I..\ strcpynf.c
bcc -mt! -c -I..\ strlenf.c 
bcc -mt! -c -I..\ strncmpf.c
bcc -mt! -c -I..\ strncpff.c
bcc -mt! -c -I..\ strncpfn.c
bcc -mt! -c -I..\ strncpnf.c
tasm tiny
copy tiny.obj ..
del tiny.obj
del extrnlib.lib
tlib extrnlib.lib @extresp, con
copy extrnlib.lib ..
del extrnlib.lib
del *.obj
