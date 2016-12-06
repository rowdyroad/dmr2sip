import React, { Component } from 'react'
import { connect } from 'react-redux'
import * as Actions from '../../actions'
import { reduxForm,SubmissionError,setSubmitSucceeded,isDirty} from 'redux-form/immutable'
import * as Utils from './Utils'
import {Map, fromJS} from 'immutable'
export default (component, props) => {

	let mapData = (state, ownProps) => {
		return {
    		initialValues:ownProps.data,
    		data: (state.main.getIn([props.scope, 'submitted'])
    				? state.main.getIn([props.scope, 'response'], Map())
    				: state.form.getIn([props.scope, 'values'], Map())).toJS(),
    		context: ownProps.context || state.main.has(props.scope) ? state.main.get(props.scope).toJS() : null,
    		submitted: state.main.getIn([props.scope, 'submitted']),
    		success: state.main.getIn([props.scope, 'success'])
    	}
	}

	let mapDispatch = (dispatch, state) => {
	  return {
		    actions: {
		      save: (data) => {
		      	data = data.toJS();
		      	let id = data[props.pkAttribute];
		      	let func = id ? Actions.Put : Actions.Post;
		        dispatch(func(props.scope, props.apiUrl + (id ? ("/" + id) : ""), data));
		      },
		      close: (data) => {
		      	Utils.Redirect(props.closeRedirect, data);
		      }
		    }
		}
	}


	class Proxy extends Component
	{
		componentWillUpdate = (next) => {
			if (next.submitted && !this.props.submitted) {
				Utils.Redirect(props.successRedirect, next.data);
				return;
			}
		}

		componentWillUnmount = () => {
			this.props.reset(props.scope);
		}
		render = () => {
			return  React.createElement(component,{
								...this.props,
				  			  	onSubmit:this.props.handleSubmit(this.props.actions.save),
				                onClose:(e)=>{ this.props.actions.close(this.props.data) }
			                  });
		}
	}

	return connect(mapData, mapDispatch)(reduxForm({ form: props.scope })(Proxy))
}