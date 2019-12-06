//////////////////////////////////////////////////////////////////////////
// convert tmElements_t data type to time string
// return time string in hh:nn:ss format (eg. 12:34:56)
//////////////////////////////////////////////////////////////////////////
String timeStr(tmElements_t tm) {
  byte hh = tm.Hour;
  String result = int2str(hh) + (":") + int2str(tm.Minute) + (":") + int2str(tm.Second);
  return result;
}

//////////////////////////////////////////////////////////////////////////
// convert tmElements_t data type to date string
// return date string in yy-mm-dd format (eg. 16-12-31)
//////////////////////////////////////////////////////////////////////////
String dateStr(tmElements_t tm) {
  String result = int2str(tmYearToCalendar(tm.Year)) + ("-") + int2str(tm.Month) + ("-") + int2str(tm.Day);
  return result;
}

//////////////////////////////////////////////////////////////////////////
// check the validity of the time
//////////////////////////////////////////////////////////////////////////
boolean check_time(byte hours, byte minutes, byte seconds) {
  if ((hours > 23) || (hours < 0)) return false;
  if ((minutes > 59) || (minutes < 0)) return false;
  if ((seconds > 59) || (seconds < 0)) return false;
  return true;
}

//////////////////////////////////////////////////////////////////////////
// check the validity of the date
//////////////////////////////////////////////////////////////////////////
boolean check_date(int years, byte months, byte days) {
  if ((years > 2099) || (years < 0)) return false;
  if ((months > 12) || (months < 1)) return false;
  byte lastday = get_lastday(years, months);
  if ((days > lastday) || (days < 1)) return false;
  return true;
}

//////////////////////////////////////////////////////////////////////////
// get last day of a month
//////////////////////////////////////////////////////////////////////////
byte get_lastday(int years, byte months) {
  byte daysInMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  byte lastday = daysInMonth[months - 1];
  if (((years % 4) == 0) && (months == 2)) lastday = 29; //leap year
  return lastday;
}
