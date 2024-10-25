--- dwm.c.orig	2022-10-04 17:38:18 UTC
+++ dwm.c
@@ -57,9 +57,13 @@
 #define TAGMASK                 ((1 << LENGTH(tags)) - 1)
 #define TEXTW(X)                (drw_fontset_getwidth(drw, (X)) + lrpad)
 
+#define OPAQUE                  0xffU
+
+#define OPAQUE                  0xffU
+
 /* enums */
 enum { CurNormal, CurResize, CurMove, CurLast }; /* cursor */
-enum { SchemeNorm, SchemeSel }; /* color schemes */
+enum { SchemeNorm, SchemeSel, SchemeTitle }; /* color schemes */
 enum { NetSupported, NetWMName, NetWMState, NetWMCheck,
        NetWMFullscreen, NetActiveWindow, NetWMWindowType,
        NetWMWindowTypeDialog, NetClientList, NetLast }; /* EWMH atoms */
@@ -163,7 +167,6 @@ static void drawbars(void);
 static Monitor *dirtomon(int dir);
 static void drawbar(Monitor *m);
 static void drawbars(void);
-static void enternotify(XEvent *e);
 static void expose(XEvent *e);
 static void focus(Client *c);
 static void focusin(XEvent *e);
@@ -182,7 +185,6 @@ static void monocle(Monitor *m);
 static void mappingnotify(XEvent *e);
 static void maprequest(XEvent *e);
 static void monocle(Monitor *m);
-static void motionnotify(XEvent *e);
 static void movemouse(const Arg *arg);
 static Client *nexttiled(Client *c);
 static void pop(Client *c);
@@ -233,6 +235,7 @@ static int xerrorstart(Display *dpy, XErrorEvent *ee);
 static int xerror(Display *dpy, XErrorEvent *ee);
 static int xerrordummy(Display *dpy, XErrorEvent *ee);
 static int xerrorstart(Display *dpy, XErrorEvent *ee);
+static void xinitvisual();
 static void zoom(const Arg *arg);
 
 /* variables */
@@ -250,13 +253,11 @@ static void (*handler[LASTEvent]) (XEvent *) = {
 	[ConfigureRequest] = configurerequest,
 	[ConfigureNotify] = configurenotify,
 	[DestroyNotify] = destroynotify,
-	[EnterNotify] = enternotify,
 	[Expose] = expose,
 	[FocusIn] = focusin,
 	[KeyPress] = keypress,
 	[MappingNotify] = mappingnotify,
 	[MapRequest] = maprequest,
-	[MotionNotify] = motionnotify,
 	[PropertyNotify] = propertynotify,
 	[UnmapNotify] = unmapnotify
 };
@@ -269,6 +270,11 @@ static Window root, wmcheckwin;
 static Monitor *mons, *selmon;
 static Window root, wmcheckwin;
 
+static int useargb = 0;
+static Visual *visual;
+static int depth;
+static Colormap cmap;
+
 /* configuration, allows nested code to access above variables */
 #include "config.h"
 
@@ -427,7 +433,8 @@ buttonpress(XEvent *e)
 
 	click = ClkRootWin;
 	/* focus monitor if necessary */
-	if ((m = wintomon(ev->window)) && m != selmon) {
+	if ((m = wintomon(ev->window)) && m != selmon
+	    && (focusonwheel || (ev->button != Button4 && ev->button != Button5))) {
 		unfocus(selmon->sel, 1);
 		selmon = m;
 		focus(NULL);
@@ -447,8 +454,8 @@ buttonpress(XEvent *e)
 		else
 			click = ClkWinTitle;
 	} else if ((c = wintoclient(ev->window))) {
-		focus(c);
-		restack(selmon);
+		if (focusonwheel || (ev->button != Button4 && ev->button != Button5))
+			focus(c);
 		XAllowEvents(dpy, ReplayPointer, CurrentTime);
 		click = ClkClientWin;
 	}
@@ -737,7 +744,7 @@ drawbar(Monitor *m)
 
 	if ((w = m->ww - tw - x) > bh) {
 		if (m->sel) {
-			drw_setscheme(drw, scheme[m == selmon ? SchemeSel : SchemeNorm]);
+			drw_setscheme(drw, scheme[m == selmon ? SchemeTitle : SchemeNorm]);
 			drw_text(drw, x, 0, w, bh, lrpad / 2, m->sel->name, 0);
 			if (m->sel->isfloating)
 				drw_rect(drw, x + boxs, boxs, boxw, boxw, m->sel->isfixed, 0);
@@ -759,25 +766,6 @@ void
 }
 
 void
-enternotify(XEvent *e)
-{
-	Client *c;
-	Monitor *m;
-	XCrossingEvent *ev = &e->xcrossing;
-
-	if ((ev->mode != NotifyNormal || ev->detail == NotifyInferior) && ev->window != root)
-		return;
-	c = wintoclient(ev->window);
-	m = c ? c->mon : wintomon(ev->window);
-	if (m != selmon) {
-		unfocus(selmon->sel, 1);
-		selmon = m;
-	} else if (!c || c == selmon->sel)
-		return;
-	focus(c);
-}
-
-void
 expose(XEvent *e)
 {
 	Monitor *m;
@@ -1117,23 +1105,6 @@ void
 }
 
 void
-motionnotify(XEvent *e)
-{
-	static Monitor *mon = NULL;
-	Monitor *m;
-	XMotionEvent *ev = &e->xmotion;
-
-	if (ev->window != root)
-		return;
-	if ((m = recttomon(ev->x_root, ev->y_root, 1, 1)) != mon && mon) {
-		unfocus(selmon->sel, 1);
-		selmon = m;
-		focus(NULL);
-	}
-	mon = m;
-}
-
-void
 movemouse(const Arg *arg)
 {
 	int x, y, ocx, ocy, nx, ny;
@@ -1542,7 +1513,8 @@ setup(void)
 	sw = DisplayWidth(dpy, screen);
 	sh = DisplayHeight(dpy, screen);
 	root = RootWindow(dpy, screen);
-	drw = drw_create(dpy, screen, root, sw, sh);
+  xinitvisual();
+	drw = drw_create(dpy, screen, root, sw, sh, visual, depth, cmap);
 	if (!drw_fontset_create(drw, fonts, LENGTH(fonts)))
 		die("no fonts could be loaded.");
 	lrpad = drw->fonts->h;
@@ -1569,8 +1541,9 @@ setup(void)
 	cursor[CurMove] = drw_cur_create(drw, XC_fleur);
 	/* init appearance */
 	scheme = ecalloc(LENGTH(colors), sizeof(Clr *));
+  unsigned int alphas[] = {borderalpha, baralpha, OPAQUE};
 	for (i = 0; i < LENGTH(colors); i++)
-		scheme[i] = drw_scm_create(drw, colors[i], 3);
+		scheme[i] = drw_scm_create(drw, colors[i], alphas, 3);
 	/* init bars */
 	updatebars();
 	updatestatus();
@@ -1669,28 +1642,31 @@ tile(Monitor *m)
 void
 tile(Monitor *m)
 {
-	unsigned int i, n, h, mw, my, ty;
+	unsigned int i, n, h, mw, my, ty, ns;
 	Client *c;
 
 	for (n = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), n++);
 	if (n == 0)
 		return;
 
-	if (n > m->nmaster)
+	if (n > m->nmaster) {
 		mw = m->nmaster ? m->ww * m->mfact : 0;
-	else
+		ns = m->nmaster > 0 ? 2 : 1;
+	} else {
 		mw = m->ww;
-	for (i = my = ty = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), i++)
+		ns = 1;
+	}
+	for(i = 0, my = ty = gappx, c = nexttiled(m->clients); c; c = nexttiled(c->next), i++)
 		if (i < m->nmaster) {
-			h = (m->wh - my) / (MIN(n, m->nmaster) - i);
-			resize(c, m->wx, m->wy + my, mw - (2*c->bw), h - (2*c->bw), 0);
+			h = (m->wh - my) / (MIN(n, m->nmaster) - i) - gappx;
+			resize(c, m->wx + gappx, m->wy + my, mw - (2*c->bw) - gappx*(5-ns)/2, h - (2*c->bw), False);
 			if (my + HEIGHT(c) < m->wh)
-				my += HEIGHT(c);
+				my += HEIGHT(c) + gappx;
 		} else {
-			h = (m->wh - ty) / (n - i);
-			resize(c, m->wx + mw, m->wy + ty, m->ww - mw - (2*c->bw), h - (2*c->bw), 0);
+			h = (m->wh - ty) / (n - i) - gappx;
+			resize(c, m->wx + mw + gappx/ns, m->wy + ty, m->ww - mw - (2*c->bw) - gappx*(5-ns)/2, h - (2*c->bw), False);
 			if (ty + HEIGHT(c) < m->wh)
-				ty += HEIGHT(c);
+				ty += HEIGHT(c) + gappx;
 		}
 }
 
@@ -1803,16 +1779,18 @@ updatebars(void)
 	Monitor *m;
 	XSetWindowAttributes wa = {
 		.override_redirect = True,
-		.background_pixmap = ParentRelative,
+		.background_pixel = 0,
+		.border_pixel = 0,
+		.colormap = cmap,
 		.event_mask = ButtonPressMask|ExposureMask
 	};
 	XClassHint ch = {"dwm", "dwm"};
 	for (m = mons; m; m = m->next) {
 		if (m->barwin)
 			continue;
-		m->barwin = XCreateWindow(dpy, root, m->wx, m->by, m->ww, bh, 0, DefaultDepth(dpy, screen),
-				CopyFromParent, DefaultVisual(dpy, screen),
-				CWOverrideRedirect|CWBackPixmap|CWEventMask, &wa);
+		m->barwin = XCreateWindow(dpy, root, m->wx, m->by, m->ww, bh, 0, depth,
+		                          InputOutput, visual,
+		                          CWOverrideRedirect|CWBackPixel|CWBorderPixel|CWColormap|CWEventMask, &wa);
 		XDefineCursor(dpy, m->barwin, cursor[CurNormal]->cursor);
 		XMapRaised(dpy, m->barwin);
 		XSetClassHint(dpy, m->barwin, &ch);
@@ -2108,6 +2086,43 @@ xerrorstart(Display *dpy, XErrorEvent *ee)
 {
 	die("dwm: another window manager is already running");
 	return -1;
+}
+
+void
+xinitvisual()
+{
+	XVisualInfo *infos;
+	XRenderPictFormat *fmt;
+	int nitems;
+	int i;
+
+	XVisualInfo tpl = {
+		.screen = screen,
+		.depth = 32,
+		.class = TrueColor
+	};
+	long masks = VisualScreenMask | VisualDepthMask | VisualClassMask;
+
+	infos = XGetVisualInfo(dpy, masks, &tpl, &nitems);
+	visual = NULL;
+	for(i = 0; i < nitems; i ++) {
+		fmt = XRenderFindVisualFormat(dpy, infos[i].visual);
+		if (fmt->type == PictTypeDirect && fmt->direct.alphaMask) {
+			visual = infos[i].visual;
+			depth = infos[i].depth;
+			cmap = XCreateColormap(dpy, root, visual, AllocNone);
+			useargb = 1;
+			break;
+		}
+	}
+
+	XFree(infos);
+
+	if (! visual) {
+		visual = DefaultVisual(dpy, screen);
+		depth = DefaultDepth(dpy, screen);
+		cmap = DefaultColormap(dpy, screen);
+	}
 }
 
 void
