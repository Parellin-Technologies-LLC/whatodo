/** ****************************************************************************************************
 * File: binding.js
 * Project: whatodo
 * @author Nick Soggin <iSkore@users.noreply.github.com> on 25-Apr-2018
 *******************************************************************************************************/
'use strict';

const todo = require( './build/Release/whatodo' );
return todo;

function WhatodoNapi(name) {
	this.greet = function(str) {
		return _addonInstance.greet(str);
	}
	
	var _addonInstance = new addon.WhatodoNapi(name);
}

module.exports = WhatodoNapi;