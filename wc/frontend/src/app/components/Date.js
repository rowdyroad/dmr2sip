'use strict'

Date.monthNames  = ["Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"];

Date.prototype.toString = function()
{
  return this.getDate() + ' '
        +  Date.monthNames[this.getMonth()] + '  '
        + this.getFullYear() + ' '
        + this.getHours().padLeft(2) + ':'
        + this.getMinutes().padLeft(2);
}

Date.fromUTCString = function(value)
{
  var m = value.match(/(\d{4})-(\d{2})-(\d{2}) (\d{2}):(\d{2}):(\d{2})/i);
  return new Date(Date.UTC(m[1],m[2] - 1,m[3],m[4],m[5],m[6]));
}