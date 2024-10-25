--- config.mk.orig	2022-10-04 17:38:18 UTC
+++ config.mk
@@ -23,17 +23,17 @@ INCS = -I${X11INC} -I${FREETYPEINC}
 
 # includes and libs
 INCS = -I${X11INC} -I${FREETYPEINC}
-LIBS = -L${X11LIB} -lX11 ${XINERAMALIBS} ${FREETYPELIBS}
+LIBS = -L${X11LIB} -lX11 ${XINERAMALIBS} ${FREETYPELIBS} -lXrender
 
 # flags
-CPPFLAGS = -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_POSIX_C_SOURCE=200809L -DVERSION=\"${VERSION}\" ${XINERAMAFLAGS}
-#CFLAGS   = -g -std=c99 -pedantic -Wall -O0 ${INCS} ${CPPFLAGS}
-CFLAGS   = -std=c99 -pedantic -Wall -Wno-deprecated-declarations -Os ${INCS} ${CPPFLAGS}
-LDFLAGS  = ${LIBS}
+CPPFLAGS += -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_POSIX_C_SOURCE=200809L -DVERSION=\"${VERSION}\" ${XINERAMAFLAGS}
+#CFLAGS   += -g -std=c99 -pedantic -Wall -O0 ${INCS} ${CPPFLAGS}
+CFLAGS   += -std=c99 -pedantic -Wall -Wno-deprecated-declarations -Os ${INCS} ${CPPFLAGS}
+LDFLAGS  += ${LIBS}
 
 # Solaris
-#CFLAGS = -fast ${INCS} -DVERSION=\"${VERSION}\"
-#LDFLAGS = ${LIBS}
+#CFLAGS += -fast ${INCS} -DVERSION=\"${VERSION}\"
+#LDFLAGS += ${LIBS}
 
 # compiler and linker
 CC = cc
