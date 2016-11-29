import Request from '../utils/Request'

export const CHANGE_CONTEXT_STATE = 'CHANGE_CONTEXT_STATE';
export const ChangeContextState = (name, state, data) => ({type: CHANGE_CONTEXT_STATE, name: name, state:state,  data:data});

export const CHECK_AUTH_REQUEST 	= 'CHECK_AUTH_REQUEST';
export const CHECK_AUTH_RESPONSE 	= 'CHECK_AUTH_RESPONSE';
export const CheckAuthRequest 		= () => ({ type: CHECK_AUTH_REQUEST})
export const CheckAuthResponse 		= (success) => ({ type: CHECK_AUTH_REQUEST, success: success})

export const LOGIN_SUCCESS 	= 'LOGIN_SUCCESS'
export const Login 			= (user) => ({type: LOGIN_SUCCESS, user: user});

export const LOGIN_REDIRECT = 'LOGIN_REDIRECT'
export const LoginRedirect 	= (location) => ({type: LOGIN_REDIRECT, redirect: location});

export const LOGOUT_SUCCESS 	= 'LOGOUT_SUCCESS';
export const Logout 		= () => ({type: LOGOUT_SUCCESS});

export const VIEWPORT_RESIZE 	= 'VIEWPORT_RESIZE';
export const ViewportResize 	= (width, height) => ({ type: VIEWPORT_RESIZE, width: width, height:height});

export const REQUEST_ACTION = 'REQUEST_ACTION';
export const RequestAction = (scope, request) => ({type: REQUEST_ACTION, scope:scope, request:request});

export const REQUEST_SUCCESS = 'REQUEST_SUCCESS';
export const RequestSuccess = (scope, response, options) => ({type: REQUEST_SUCCESS, scope:scope, response:response, options: options});

export const REQUEST_ERROR = 'REQUEST_ERROR';
export const RequestError = (scope, error) => ({type: REQUEST_ERROR, scope:scope, error:error});

export const OBJECT_MERGE = 'OBJECT_MERGE';
export const ObjectMerge = (path, object) => ({type: OBJECT_MERGE, path: path, object:object});

export const OBJECT_REMOVE = 'OBJECT_REMOVE';
export const ObjectRemove = (path) => ({type: OBJECT_REMOVE, path: path});

export const OBJECT_SET	   = 'OBJECT_SET';
export const ObjectSet = (path, object) => ({type: OBJECT_SET, path: path, object:object});

export const LIST_ITEM_UPDATE = 'LIST_ITEM_UPDATE';
export const ListItemUpdate = (path, object, pkAttribute) => ({type: LIST_ITEM_UPDATE, path: path, object: object, pkAttribute:pkAttribute});

export const LIST_ITEM_REMOVE = 'LIST_ITEM_REMOVE';
export const ListItemRemove = (path, pkAttribute, pkValue) => ({type: LIST_ITEM_REMOVE, path: path, pkAttribute:pkAttribute, pkValue:pkValue});

export const LIST_ITEM_ADD = 'List_ITEM_ADD';
export const ListItemAdd = (path, object) => ({type: LIST_ITEM_ADD, path: path, object:object});

const CreateRequest = (method, scope, request, options) => {
	return (dispatch, getState) => {
		dispatch(RequestAction(scope, request));

		method(request.url, request.params)
		.then((response) => {
			dispatch(RequestSuccess(scope, response, options));
			if (options && options.onSuccess) {
				dispatch(options.onSuccess);
			}
		})
		.catch((error) => {
			dispatch(RequestError(scope, error, request.request));
			if (options && options.onError) {
				dispatch(options.onError);
			}
		});
	}
}

export const Fetch = (scope, request, options) => {
	if (typeof(request) === "string") {
		request = {url: request};
	}
	return CreateRequest(Request.get, scope, request, options);
}

export const Post = (scope, request, options, aOptions) => {
	if (typeof(request) === "string") {
		request = {url: request, params:options};
		options = aOptions;
	}
	return CreateRequest(Request.post, scope, request, options);
}

export const Put = (scope, request, options, aOptions) => {
	if (typeof(request) === "string") {
		request = {url: request, params:options};
		options = aOptions;
	}
	return CreateRequest(Request.put, scope, request, options);
}

export const Delete = (scope, request, options, aOptions) => {
	if (typeof(request) === "string") {
		request = {url: request, params:options};
		options = aOptions;
	}
	return CreateRequest(Request.delete, scope, request, options);
}