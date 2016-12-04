import React, { Component } from 'react'
import { connect } from 'react-redux'
import * as Actions from '../../actions'
import { reduxForm } from 'redux-form/immutable'
import * as Utils from './Utils'

export default (component, props) => {

	let mapData = (state, ownProps) => {

    	return {
    		initialValues:ownProps.data,
    		data: state.form.getIn([props.scope, 'submitSucceeded'])
    			  ? state.main.getIn([props.scope, 'response']).toJS()
    			  : (state.form.hasIn([props.scope, 'values'])
    			  		? state.form.getIn([props.scope,'values']).toJS()
    			  		: ownProps.data)
    	}
	}

	let mapDispatch = (dispatch, state) => {
	  return {
		    actions: {
		      save: (data) => {
		      	return new Promise((resolve, reject) => {
			      	data = data.toJS();
			      	let id = data[props.pkAttribute];
			      	let func = id ? Actions.Put : Actions.Post;
			        dispatch(func(props.scope, props.apiUrl + (id ? ("/" + id) : ""), data, {
			          onSuccess:(data)=> {
						resolve();
			          	if (props.listScope) {
			              return id ? Actions.ListItemUpdate([props.listScope, "response","data"], data, props.pkAttribute)
			              			: Actions.ListItemAdd([props.listScope, "response","data"], data);
			            }
			          },
			          onError:() => {
			          	 reject();
			          }
			        }));
			    });
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
			if (next.submitSucceeded && next.data[props.pkAttribute]) {
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