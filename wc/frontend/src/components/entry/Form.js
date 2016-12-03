import React, { Component } from 'react'
import * as UI from '../../components/UIKit'
import { connect } from 'react-redux'
import * as Actions from '../../actions';
import {Router, browserHistory, Link} from 'react-router'
import { Field, reduxForm, FormSection } from 'redux-form/immutable'


export default (component, props) => {

	let mapData = (state, ownProps) => {

		let item = ownProps.data && ownProps.data[props.pkAttribute]
					? ownProps.data : ( state.main.getIn([props.scope, 'success']) ? state.main.getIn([props.scope, 'response']).toJS() : null);

    	return {
    		initialValues:item,
    		data: state.form.hasIn([props.scope,'values']) ? state.form.getIn([props.scope,'values']).toJS() : item
    	}
	}

	let mapDispatch = (dispatch) => {
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
			          }
			        }));
			    });
		      },
		      close: (data) => {
		        dispatch(Actions.ObjectRemove(props.scope));
		      }
		    }
		}
	}


	class Proxy extends Component
	{
		componentWillUpdate = (next) => {
			if (props.successRedirect && next.submitSucceeded) {
				let match = props.successRedirect.match(/:(\w+)/);
				let redirect = props.successRedirect.replace(":" + match[1], next.data[match[1]]);
				browserHistory.push(redirect);
				return null;
			}
		}
		render = () => {
			return  React.createElement(component,{
								...this.props,
				  			  	onSubmit:this.props.handleSubmit(this.props.actions.save),
				                onClose:this.props.actions.close
			                  });
		}
	}

	return connect(mapData, mapDispatch)(reduxForm({ form: props.scope })(Proxy))
}