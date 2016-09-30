'use strict';


(function() {
	var old_getTime = Date.prototype.getTime;

	Date.prototype.getTime = function(force)
	{
		var old_time = old_getTime.call(this);
		if (force) {
			return old_time;
		}
		if ( old_time == (new Date()).getTime(true)) {
			return old_time + parseInt(this.getTimezoneOffset()) * 60000;
		}
		return  old_time;
	}

})();