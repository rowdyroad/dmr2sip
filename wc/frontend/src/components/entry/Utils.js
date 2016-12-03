import {browserHistory} from 'react-router'

export const Redirect = (redirect, data) => {
	if (redirect) {
		let match = redirect.match(/:(\w+)/i);
		if (match && match.length > 1) {
			redirect = redirect.replace(":" + match[1], data[match[1]]);
		}
		browserHistory.push(redirect);
	}
}

