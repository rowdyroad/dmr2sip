
const createRequest = (url, request, method) =>  {
	 return new Promise((resolve, reject) => {
	 	fetch(url, {
	      method: method,
	      credentials: 'same-origin',
	      headers: {
	        'Accept': 'application/json',
	        'Content-Type': 'application/json'
	      },
	      body: JSON.stringify(request)
	    }).then((response) => {
	      if (response.ok) {
	        return response.json().catch(() => {
	        	if (resolve) {
	        		resolve({});
	        	}
	        });
	      } else {
	     	if (reject) {
	     		reject(response.status, response.statusText);
	     	}
	     }
	    })
	    .then((data) => {
	    	if (resolve) {
	    		resolve(data);
	    	}
	    })
	});
}




export default {
	post: function(url, request)
	{
		return createRequest(url, request, 'POST');
	},

	put: function(url, request) {
		return createRequest(url, request, 'PUT');
	},
	delete: function(url, request) {
		return createRequest(url, request, 'DELETE');
	},
	get: function(url, params) {
		return new Promise((resolve,reject) => {

			 url = new URL(window.location.origin + url);
			 if (params) {
			 	Object.keys(params).forEach(key => url.searchParams.append(key, params[key]));
			 }

			fetch(url, {
		      method: 'GET',
		      credentials: 'same-origin',
		      headers: {
		        'Accept': 'application/json',
		        'Content-Type': 'application/json'
		      }
		    }).then((response) => {
		      if (response.ok) {
		        return response.json();
		      } else {
			      if (reject) {
			     	reject(response.status, response.statusText);
			      }
			  }
		    })
		    .then((data) => {
		    	if (resolve) {
		    		resolve(data);
		    	}
		    })
		});
	}
}