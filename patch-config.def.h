--- config.def.h.orig	2022-10-04 17:38:18 UTC
+++ config.def.h
@@ -2,20 +2,61 @@ static const unsigned int borderpx  = 1;        /* bor
 
 /* appearance */
 static const unsigned int borderpx  = 1;        /* border pixel of windows */
+static const unsigned int gappx     = 18;       /* gap pixel between windows */
 static const unsigned int snap      = 32;       /* snap pixel */
 static const int showbar            = 1;        /* 0 means no bar */
 static const int topbar             = 1;        /* 0 means bottom bar */
-static const char *fonts[]          = { "monospace:size=10" };
-static const char dmenufont[]       = "monospace:size=10";
+static const int focusonwheel       = 0;
+static const char *fonts[]          = { "Hack:style=Regular:size=12" };
+static const char dmenufont[]       = "Hack:style=Regular:size=11";
+static unsigned int baralpha        = 0xd0;
+static unsigned int borderalpha     = OPAQUE;
 static const char col_gray1[]       = "#222222";
 static const char col_gray2[]       = "#444444";
 static const char col_gray3[]       = "#bbbbbb";
 static const char col_gray4[]       = "#eeeeee";
 static const char col_cyan[]        = "#005577";
+static const char col_background[]  = "#1e1e2e";
+static const char col_foreground[]  = "#cdd6f4";
+static const char col_cursorColor[] = "#f5e0dc";
+
+// black
+static const char col_color0[]      = "#45475a";
+static const char col_color8[]      = "#585b70";
+
+// red
+static const char col_color1[]      = "#f38ba8";
+static const char col_color9[]      = "#f38ba8";
+
+// green
+static const char col_color2[]      = "#a6e3a1";
+static const char col_color10[]     = "#a6e3a1";
+
+// yellow
+static const char col_color3[]      = "#f9e2af";
+static const char col_color11[]     = "#f9e2af";
+
+// blue
+static const char col_color4[]      = "#89b4fa";
+static const char col_color12[]     = "#89b4fa";
+
+// magenta
+static const char col_color5[]      = "#f5c2e7";
+static const char col_color13[]     = "#f5c2e7";
+
+// cyan
+static const char col_color6[]      = "#94e2d5";
+static const char col_color14[]     = "#94e2d5";
+
+// white
+static const char col_color7[]      = "#bac2de";
+static const char col_color15[]     = "#a6adc8";
+
 static const char *colors[][3]      = {
 	/*               fg         bg         border   */
-	[SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
-	[SchemeSel]  = { col_gray4, col_cyan,  col_cyan  },
+	[SchemeNorm] = { col_foreground, col_color0, col_color8 },
+	[SchemeSel]  = { col_color0, col_color6, col_color6  },
+	[SchemeTitle] = { col_foreground, col_color0, col_color0 },
 };
 
 /* tagging */
@@ -45,7 +86,7 @@ static const Layout layouts[] = {
 };
 
 /* key definitions */
-#define MODKEY Mod1Mask
+#define MODKEY Mod4Mask
 #define TAGKEYS(KEY,TAG) \
 	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
 	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
@@ -56,7 +97,8 @@ static const Layout layouts[] = {
 #define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }
 
 /* commands */
-static const char *dmenucmd[] = { "dmenu_run", "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, NULL };
+//static const char *dmenucmd[] = { "dmenu_run", "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, NULL };
+static const char *dmenucmd[] = { "dmenu_run", NULL };
 static const char *termcmd[]  = { "st", NULL };
 
 static const Key keys[] = {
@@ -80,10 +122,10 @@ static const Key keys[] = {
 	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
 	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
 	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
-	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
-	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
-	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
-	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
+	{ MODKEY,                       XK_comma,  focusmon,       {.i = +1 } },
+	{ MODKEY,                       XK_period, focusmon,       {.i = -1 } },
+	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = +1 } },
+	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = -1 } },
 	TAGKEYS(                        XK_1,                      0)
 	TAGKEYS(                        XK_2,                      1)
 	TAGKEYS(                        XK_3,                      2)
