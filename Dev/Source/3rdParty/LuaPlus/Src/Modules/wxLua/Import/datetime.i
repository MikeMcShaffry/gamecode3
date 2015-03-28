/////////////////////////////////////////////////////////////////////////////
// Purpose:     wxDateTime and other time related classes and functions
// Author:      J Winwood
// Created:     14/11/2001
// Copyright:   (c) 2001-2002 Lomtick Software. All rights reserved.
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

%include "wx/utils.h"
%include "wx/timer.h"

%builtin wxString wxNow()
%builtin long wxGetLocalTime()
%builtin long wxGetUTCTime()
//%builtin wxLongLong wxGetLocalTimeMillis()
%builtin void wxStartTimer()
%builtin long wxGetElapsedTime(bool resetTimer = TRUE)
%builtin void wxSleep(int secs)
%builtin void wxUsleep(unsigned long milliseconds)

///////////////////////////////////////////////////////////////////////////////
// wxDateTime

%if wxLUA_USE_wxDateTime

%include "wx/datetime.h"

%enum wxDateTime::TZ
    Local
    GMT_12
    GMT_11
    GMT_10
    GMT_9
    GMT_8
    GMT_7
    GMT_6
    GMT_5
    GMT_4
    GMT_3
    GMT_2
    GMT_1
    GMT0
    GMT1
    GMT2
    GMT3
    GMT4
    GMT5
    GMT6
    GMT7
    GMT8
    GMT9
    GMT10
    GMT11
    GMT12
    WET
    WEST
    CET
    CEST
    EET
    EEST
    MSK
    MSD
    AST
    ADT
    EST
    EDT
    CST
    CDT
    MST
    MDT
    PST
    PDT
    HST
    AKST
    AKDT
    A_WST
    A_CST
    A_EST
    A_ESST
    UTC
%end

%enum wxDateTime::Calendar
    Gregorian
    Julian
%end

%enum wxDateTime::Country
    Country_Unknown
    Country_Default
    Country_WesternEurope_Start
    Country_EEC
    France
    Germany
    UK
    Country_WesternEurope_End
    Russia
    USA
%end

%enum wxDateTime::Month
    Jan
    Feb
    Mar
    Apr
    May
    Jun
    Jul
    Aug
    Sep
    Oct
    Nov
    Dec
    Inv_Month
%end

%enum wxDateTime::WeekDay
    Sun
    Mon
    Tue
    Wed
    Thu
    Fri
    Sat
    Inv_WeekDay
%end

%typedef wxDateTime_t unsigned short

%class %delete %noclassinfo %encapsulate wxDateTime
	%define %object wxDefaultDateTime

	wxDateTime()
    %constructor wxDateTimeFromTime(time_t dateTime)
    %constructor wxDateTimeFromJDN(double dateTime)
    %constructor wxDateTimeFromHMS(int hour, int minute, int second, int millisec)
    %constructor wxDateTimeFromDMY(int day, wxDateTime::Month month = wxDateTime::Inv_Month, int year = wxDateTime::Inv_Year, int hour = 0, int minute = 0, int second = 0, int millisec = 0)
    wxDateTime& SetToCurrent()
    %rename SetToTime wxDateTime& Set(time_t time)
    %rename SetToJDN wxDateTime& Set(double dateTime)
    %rename SetToHMS wxDateTime& Set(int hour, int minute, int second, int millisec)
    %rename SetToDMY wxDateTime& Set(int day, wxDateTime::Month month = wxDateTime::Inv_Month, int year = wxDateTime::Inv_Year, int hour = 0, int minute = 0, int second = 0, int millisec = 0)
    wxDateTime& ResetTime()
    wxDateTime& SetDay(int day)
    wxDateTime& SetMonth(wxDateTime::Month month)
    wxDateTime& SetYear(int year)
    wxDateTime& SetHour(int hour)
    wxDateTime& SetMinute(int minute)
    wxDateTime& SetSecond(int second)
    wxDateTime& SetMillisecond(int millisecond)
    bool IsWorkDay(wxDateTime::Country country = wxDateTime::Country_Default) const
    bool IsEqualTo(const wxDateTime& datetime) const
    bool IsEarlierThan(const wxDateTime& datetime) const
    bool IsLaterThan(const wxDateTime& datetime) const
    bool IsStrictlyBetween(const wxDateTime& t1, const wxDateTime& t2) const
    bool IsBetween(const wxDateTime& t1, const wxDateTime& t2) const
    bool IsSameDate(const wxDateTime& dt) const
    bool IsSameTime(const wxDateTime& dt) const
    bool IsEqualUpTo(const wxDateTime& dt, const wxTimeSpan& ts) const
    bool IsValid()
    long GetTicks()
    wxDateTime& SetToWeekDayInSameWeek(wxDateTime::WeekDay weekday)
    wxDateTime  GetWeekDayInSameWeek(wxDateTime::WeekDay weekday) const
    wxDateTime& SetToNextWeekDay(wxDateTime::WeekDay weekday)
    wxDateTime GetNextWeekDay(wxDateTime::WeekDay weekday) const
    wxDateTime& SetToPrevWeekDay(wxDateTime::WeekDay weekday)
    wxDateTime GetPrevWeekDay(wxDateTime::WeekDay weekday) const
    bool SetToWeekDay(wxDateTime::WeekDay weekday, int n = 1, wxDateTime::Month month = wxDateTime::Inv_Month, int year = wxDateTime::Inv_Year)
    wxDateTime GetWeekDay(wxDateTime::WeekDay weekday, int n = 1, wxDateTime::Month month = wxDateTime::Inv_Month, int year = wxDateTime::Inv_Year) const
    bool SetToLastWeekDay(wxDateTime::WeekDay weekday, wxDateTime::Month month = wxDateTime::Inv_Month, int year = wxDateTime::Inv_Year)
    wxDateTime GetLastWeekDay(wxDateTime::WeekDay weekday, wxDateTime::Month month = wxDateTime::Inv_Month, int year = wxDateTime::Inv_Year)
    bool SetToTheWeek(wxDateTime_t numWeek, wxDateTime::WeekDay weekday = wxDateTime::Mon)
    wxDateTime GetWeek(wxDateTime_t numWeek, wxDateTime::WeekDay weekday = wxDateTime::Mon) const
    wxDateTime& SetToLastMonthDay(wxDateTime::Month month = wxDateTime::Inv_Month, int year = wxDateTime::Inv_Year)
    wxDateTime GetLastMonthDay(wxDateTime::Month month = wxDateTime::Inv_Month, int year = wxDateTime::Inv_Year) const
    wxDateTime& SetToYearDay(wxDateTime_t yday)
    wxDateTime GetYearDay(wxDateTime_t yday) const
    double GetJulianDayNumber() const
    double GetJDN() const
    double GetModifiedJulianDayNumber() const
    double GetMJD() const
    double GetRataDie() const
    wxDateTime ToGMT(bool noDST = FALSE) const
    wxDateTime& MakeGMT(bool noDST = FALSE)
    int IsDST(wxDateTime::Country country = wxDateTime::Country_Default) const
    wxDateTime& Add(const wxTimeSpan& diff)
    wxDateTime& Subtract(const wxTimeSpan& diff)
    %rename AddDateSpan wxDateTime& Add(const wxDateSpan& diff)
    %rename SubtractDateSpan wxDateTime& Subtract(const wxDateSpan& diff)
    wxString ParseRfc822Date(wxString date)
    wxString ParseFormat(wxString date, wxString format = "%c", const wxDateTime& dateDef = wxDefaultDateTime)
    wxString ParseDateTime(wxString datetime)
    wxString ParseDate(wxString date)
    wxString ParseTime(wxString time)
    wxString FormatDate() const
    wxString FormatTime() const
    wxString FormatISODate() const
    wxString FormatISOTime() const
    wxString Format(wxString format = "%c", wxDateTime::TZ tz = wxDateTime::Local) const
%endclass

%endif wxLUA_USE_wxDateTime

///////////////////////////////////////////////////////////////////////////////
// wxTimeSpan

%if wxLUA_USE_wxTimeSpan

%include "wx/datetime.h"

%class %delete %noclassinfo %encapsulate wxTimeSpan
    wxTimeSpan()
    %builtin %rename wxTimeSpan_Seconds wxTimeSpan wxTimeSpan::Seconds(long sec)
    %builtin %rename wxTimeSpan_Second  wxTimeSpan wxTimeSpan::Second()
    %builtin %rename wxTimeSpan_Minutes wxTimeSpan wxTimeSpan::Minutes(long min)
    %builtin %rename wxTimeSpan_Minute  wxTimeSpan wxTimeSpan::Minute()
    %builtin %rename wxTimeSpan_Hours   wxTimeSpan wxTimeSpan::Hours(long hours)
    %builtin %rename wxTimeSpan_Hour    wxTimeSpan wxTimeSpan::Hour()
    %builtin %rename wxTimeSpan_Days    wxTimeSpan wxTimeSpan::Days(long days)
    %builtin %rename wxTimeSpan_Day     wxTimeSpan wxTimeSpan::Day()
    %builtin %rename wxTimeSpan_Weeks   wxTimeSpan wxTimeSpan::Weeks(long days)
    %builtin %rename wxTimeSpan_Week    wxTimeSpan wxTimeSpan::Week()

    %constructor wxTimeSpanHMS(long hours, long minutes = 0, long seconds = 0, long milliseconds = 0)
    wxTimeSpan Add(const wxTimeSpan& diff) const
    wxTimeSpan Subtract(const wxTimeSpan& diff) const
    wxTimeSpan Multiply(int n) const
    wxTimeSpan Negate() const
    wxTimeSpan& Neg()
    wxTimeSpan Abs()
    bool IsNull() const
    bool IsPositive() const
    bool IsNegative() const
    bool IsEqualTo(const wxTimeSpan& ts) const
    bool IsLongerThan(const wxTimeSpan& ts) const
    bool IsShorterThan(const wxTimeSpan& ts) const
    int GetWeeks() const
    %property=Weeks, read
    int GetDays() const
    %property=Days, read
    int GetHours() const
    %property=Hours, read
    int GetMinutes() const
    %property=Minutes, read
    //wxLongLong GetSeconds() const
    //wxLongLong GetMilliseconds() const
    wxString Format(wxString format = "%H:%M:%S") const
%endclass

%endif wxLUA_USE_wxTimeSpan

///////////////////////////////////////////////////////////////////////////////
// wxDateSpan

%if wxLUA_USE_wxDateSpan

%include "wx/datetime.h"

%class %delete %noclassinfo %encapsulate wxDateSpan
    wxDateSpan(int years = 0, int months = 0, int weeks = 0, int days = 0)
    %builtin %rename wxDateSpan_Days   wxDateSpan wxDateSpan::Days(int days)
    %builtin %rename wxDateSpan_Day    wxDateSpan wxDateSpan::Day()
    %builtin %rename wxDateSpan_Weeks  wxDateSpan wxDateSpan::Weeks(int weeks)
    %builtin %rename wxDateSpan_Week   wxDateSpan wxDateSpan::Week()
    %builtin %rename wxDateSpan_Months wxDateSpan wxDateSpan::Months(int mon)
    %builtin %rename wxDateSpan_Month  wxDateSpan wxDateSpan::Month()
    %builtin %rename wxDateSpan_Years  wxDateSpan wxDateSpan::Years(int years)
    %builtin %rename wxDateSpan_Year   wxDateSpan wxDateSpan::Year()

    wxDateSpan& SetDays(int n)
    %property=Days, read, write
    wxDateSpan& SetWeeks(int n)
    %property=Weeks, read, write
    wxDateSpan& SetMonths(int n)
    %property=Months, read, write
    wxDateSpan& SetYears(int n)
    %property=Years, read, write
    int GetDays() const
    int GetWeeks() const
    int GetMonths() const
    int GetYears() const
    int GetTotalDays() const
    %property=TotalDays, read
    wxDateSpan Add(const wxDateSpan& other) const
    wxDateSpan Subtract(const wxDateSpan& other) const
    wxDateSpan Negate() const
    wxDateSpan& Neg()
    wxDateSpan Multiply(int factor) const
%endclass

%endif wxLUA_USE_wxDateSpan

///////////////////////////////////////////////////////////////////////////////
// wxTimer

%if wxLUA_USE_wxTimer

%include "wx/timer.h"

%class wxTimer, wxObject
    wxTimer(wxEvtHandler *owner, int id = -1)
    bool     IsOneShot() const
    bool     IsRunning() const
    void     Notify()
    void     SetOwner(wxEvtHandler *owner, int id = -1)
    bool     Start(int milliseconds = -1, bool oneShot = FALSE)
    void     Stop()
%endclass

%endif wxLUA_USE_wxTimer

///////////////////////////////////////////////////////////////////////////////
// wxCalendarCtrl

%if wxLUA_USE_wxCalendarCtrl

%include "wx/calctrl.h"

%enum
	wxCAL_SUNDAY_FIRST
	wxCAL_MONDAY_FIRST
	wxCAL_SHOW_HOLIDAYS
	wxCAL_NO_YEAR_CHANGE
	wxCAL_NO_MONTH_CHANGE
~%wxver22	wxCAL_SHOW_SURROUNDING_WEEKS
~%wxver22	wxCAL_SEQUENTIAL_MONTH_SELECTION
%end

%enum wxCalendarHitTestResult
    wxCAL_HITTEST_NOWHERE
    wxCAL_HITTEST_HEADER
    wxCAL_HITTEST_DAY
~%wxver22    wxCAL_HITTEST_INCMONTH
~%wxver22    wxCAL_HITTEST_DECMONTH
~%wxver22    wxCAL_HITTEST_SURROUNDING_WEEK
%end

%enum wxCalendarDateBorder
    wxCAL_BORDER_NONE
    wxCAL_BORDER_SQUARE
    wxCAL_BORDER_ROUND
%end

%class wxCalendarCtrl, wxControl
    wxCalendarCtrl(wxWindow* parent, wxWindowID id, const wxDateTime& date = wxDefaultDateTime, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxCAL_SHOW_HOLIDAYS, const wxString& name = wxCalendarNameStr)
    bool     Create(wxWindow* parent, wxWindowID id, const wxDateTime& date = wxDefaultDateTime, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxCAL_SHOW_HOLIDAYS, const wxString& name = wxCalendarNameStr)
    void     SetDate(const wxDateTime& date)
    void     EnableYearChange(bool enable = TRUE)
    void     EnableMonthChange(bool enable = TRUE)
    void     EnableHolidayDisplay(bool display = TRUE)
    void     SetHeaderColours(const wxColour& colFg, const wxColour& colBg)
    const wxColour&  GetHeaderColourFg() const
    %property=HeaderColourFg, read
    const wxColour&  GetHeaderColourBg() const
    %property=HeaderColourBg, read
    void     SetHighlightColours(const wxColour& colFg, const wxColour& colBg)
    const wxColour&  GetHighlightColourFg() const
    %property=HighlightColourFg, read
    const wxColour&  GetHighlightColourBg() const
    %property=HighlightColourBg, read
    void     SetHolidayColours(const wxColour& colFg, const wxColour& colBg)
    const wxColour& GetHolidayColourFg() const
    %property=HolidayColourFg, read
    const wxColour& GetHolidayColourBg() const
    %property=HolidayColourBg, read
    wxCalendarDateAttr * GetAttr(size_t day) const
    void     SetAttr(size_t day, wxCalendarDateAttr* attr)
    void     SetHoliday(size_t day)
    void     ResetAttr(size_t day)
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxCalendarDateAttr

%class %delete %noclassinfo %encapsulate wxCalendarDateAttr
    wxCalendarDateAttr()
    %constructor wxCalendarDateAttrColour(const wxColour& colText, const wxColour& colBack = wxNullColour, const wxColour& colBorder = wxNullColour, const wxFont& font = wxNullFont, wxCalendarDateBorder border = wxCAL_BORDER_NONE)
    %constructor wxCalendarDateAttrBorder(wxCalendarDateBorder border, const wxColour& colBorder = wxNullColour)
    void SetTextColour(const wxColour& colText)
    void SetBackgroundColour(const wxColour& colBack)
    void SetBorderColour(const wxColour& col)
    void SetFont(const wxFont& font)
    void SetBorder(wxCalendarDateBorder border)
    void SetHoliday(bool holiday)
    bool HasTextColour() const
    bool HasBackgroundColour() const
    bool HasBorderColour() const
    bool HasFont() const
    bool HasBorder() const
    bool IsHoliday() const
    const wxColour& GetTextColour() const
    %property=TextColour, read, write
    const wxColour& GetBackgroundColour()
    %property=BackgroundColour, read, write
    const wxColour& GetBorderColour() const
    %property=BorderColour, read, write
    const wxFont& GetFont() const
    %property=Font, read, write
    wxCalendarDateBorder GetBorder()
    %property=Border, read, write
%endclass

///////////////////////////////////////////////////////////////////////////////
// wxCalendarEvent

%include "wx/event.h"

%class %delete wxCalendarEvent, wxEvent
    %define %event wxEVT_CALENDAR_SEL_CHANGED
    %define %event wxEVT_CALENDAR_DAY_CHANGED
    %define %event wxEVT_CALENDAR_MONTH_CHANGED
    %define %event wxEVT_CALENDAR_YEAR_CHANGED
    %define %event wxEVT_CALENDAR_DOUBLECLICKED
    %define %event wxEVT_CALENDAR_WEEKDAY_CLICKED

    wxCalendarEvent(wxCalendarCtrl *cal, wxEventType type)
    const wxDateTime& GetDate() const
    %property=Date, read
    wxDateTime::WeekDay GetWeekDay() const
    %property=WeekDay, read
%endclass

%endif wxLUA_USE_wxCalendarCtrl
