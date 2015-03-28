#ifndef _WX_LUA_WXH__
#define _WX_LUA_WXH__

// This file is a copy of wx.h with added class for
// wrapping with wxLua

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/dynarray.h"
#include "wx/list.h"
#include "wx/hash.h"
#include "wx/string.h"
#include "wx/intl.h"
#include "wx/log.h"
#include "wx/event.h"
#include "wx/app.h"
#include "wx/utils.h"
#include "wx/stream.h"

// extra non-gui classes for wxLua


#if wxUSE_GUI

#include "wx/window.h"
#include "wx/panel.h"
#include "wx/frame.h"
#include "wx/dc.h"
#include "wx/dcclient.h"
#include "wx/dcmemory.h"
#include "wx/dcprint.h"
#include "wx/dcscreen.h"
#include "wx/button.h"
#include "wx/menu.h"
#include "wx/pen.h"
#include "wx/brush.h"
#include "wx/palette.h"
#include "wx/icon.h"
#include "wx/cursor.h"
#include "wx/dialog.h"
#include "wx/timer.h"
#include "wx/settings.h"
#include "wx/msgdlg.h"
#include "wx/cmndata.h"

#include "wx/control.h"
#include "wx/ctrlsub.h"
#include "wx/bmpbuttn.h"
#include "wx/checkbox.h"
#include "wx/checklst.h"
#include "wx/choice.h"
#include "wx/scrolbar.h"
#include "wx/stattext.h"
#include "wx/statbmp.h"
#include "wx/statbox.h"
#include "wx/listbox.h"
#include "wx/radiobox.h"
#include "wx/radiobut.h"
#include "wx/textctrl.h"
#include "wx/slider.h"
#include "wx/gauge.h"
#include "wx/scrolwin.h"
#include "wx/dirdlg.h"
#include "wx/toolbar.h"
#include "wx/combobox.h"
#include "wx/layout.h"

#include "wx/sizer.h"
#include "wx/memory.h"
#include "wx/mdi.h"
#include "wx/statusbr.h"
#include "wx/scrolbar.h"
#include "wx/choicdlg.h"
#include "wx/textdlg.h"
#include "wx/filedlg.h"
#include "wx/validate.h"        // always include, even if !wxUSE_VALIDATORS

#if wxUSE_VALIDATORS
    #include "wx/valtext.h"
#endif // wxUSE_VALIDATORS

// extra gui classes for wxLua

/*
accel.h
access.h
app.h
artprov.h
bitmap.h
bmpbuttn.h
brush.h
buffer.h
build.h
busyinfo.h
button.h
calctrl.h
caret.h
checkbox.h
checklst.h
chkconf.h
choicdlg.h
choice.h
clipbrd.h
clntdata.h
cmdline.h
cmdproc.h
cmndata.h
colordlg.h
colour.h
combobox.h
confbase.h
config.h
containr.h
control.h
cshelp.h
ctrlsub.h
cursor.h
dataobj.h
datetime.h
datstrm.h
dbgrid.h
db.h
dbkeyg.h
dbtable.h
dcbuffer.h
dcclient.h
dc.h
dcmemory.h
dcprint.h
dcps.h
dcscreen.h
dde.h
debug.h
defs.h
dialog.h
dialup.h
dirctrl.h
dirdlg.h
dir.h
display.h
dnd.h
docmdi.h
docview.h
dragimag.h
dynarray.h
dynlib.h
dynload.h
effects.h
encconv.h
event.h
evtloop.h
fdrepdlg.h
features.h
ffile.h
fileconf.h
filedlg.h
filefn.h
file.h
filename.h
filesys.h
fontdlg.h
fontenc.h
fontenum.h
font.h
fontmap.h
fontutil.h
frame.h
fs_inet.h
fs_mem.h
fs_zip.h
gauge.h
gdicmn.h
gdiobj.h
geometry.h
gifdecod.h
glcanvas.h
grid.h
gsocket.h
hash.h
hashmap.h
helpbase.h
help.h
helphtml.h
helpwin.h
htmllbox.h
iconbndl.h
icon.h
imagbmp.h
image.h
imaggif.h
imagiff.h
imagjpeg.h
imaglist.h
imagpcx.h
imagpng.h
imagpnm.h
imagtiff.h
imagxpm.h
intl.h
iosfwrap.h
ioswrap.h
ipcbase.h
ipc.h
isqlext.h
isql.h
joystick.h
layout.h
laywin.h
listbase.h
listbox.h
listctrl.h
list.h
log.h
longlong.h
math.h
matrix.h
mdi.h
memconf.h
memory.h
memtext.h
menu.h
menuitem.h
metafile.h
mimetype.h
minifram.h
module.h
msgdlg.h
msgout.h*
mstream.h
notebook.h
object.h
ownerdrw.h
palette.h
panel.h
paper.h
pen.h
platform.h
popupwin.h
printdlg.h
print.h
prntbase.h
process.h
progdlg.h
ptr_scpd.h
quantize.h
radiobox.h
radiobut.h
rawbmp.h
regex.h
region.h
sashwin.h
sckaddr.h
sckipc.h
sckstrm.h
scrolbar.h
scrolwin.h
settings.h
setup_redirect.h
sizer.h
slider.h
snglinst.h
socket.h
spinbutt.h
spinctrl.h
splash.h
splitter.h
stack.h
statbmp.h
statbox.h
statline.h
stattext.h
statusbr.h
strconv.h
stream.h
string.h
sysopt.h
tabctrl.h
tab.h
taskbar.h
tbarbase.h
tbarsmpl.h
textbuf.h
textctrl.h
textdlg.h
textfile.h
tglbtn.h
thread.h
timer.h
tipdlg.h
tipwin.h
tokenzr.h
toolbar.h
tooltip.h
toplevel.h
treebase.h
treectrl.h
txtstrm.h
types.h
url.h
utils.h
valgen.h
validate.h
valtext.h
variant.h
vector.h
version.h
vlbox.h
vms_x_fix.h
volume.h
vscroll.h
wave.h
wfstream.h
window.h
wizard.h
wxchar.h
wx_cwc_d.h
wx_cwc.h
wx_cw_cm.h
wx_cw_d.h
wx_cw.h
wx_cwu_d.h
wx.h
wxhtml.h
wxprec.h
wxshlba_cwc_d.h
wxshlba_cwc.h
wxshlba_cw_d.h
wxshlba_cw.h
wxshlb_cwc_d.h
wxshlb_cwc.h
wxshlb_cw_d.h
wxshlb_cw.h
xpmdecod.h
xpmhand.h
zipstrm.h
zstream.h
*/

#endif // wxUSE_GUI

#endif
    // _WX_LUA_WXH__
