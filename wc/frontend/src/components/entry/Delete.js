import React, { Component } from 'react'
import { connect } from 'react-redux'
import * as Actions from '../../actions'
import * as UI from '../UIKit'
import * as Utils from './Utils'

export default (component, props) => {
  	let mapState = (state, ownProps) => {
	  	return {
	  				...props
              }
  	}

  	let mapDispatch = (dispatch) => {
	    return {
	 	    actions: {
	      		delete: (data) => {
	      			dispatch(Actions.Delete(props.scope, props.apiUrl + '/' + data[props.pkAttribute], {}, {
	      				onSuccess:() => {
	      					let actions =  [ Actions.ObjectRemove(props.scope) ] ;
	      					if (props.listScope) {
	      						actions.push(Actions.ListItemRemove([props.listScope, "response", "data"], props.pkAttribute, data[props.pkAttribute]))
	      					}
	      					return actions;
	      				}
	      			}))
	      		},
	      		close: (data) => {
	      			Utils.Redirect(props.closeRedirect, data);
	      		}
	      	}
	  	}
	}


	class DeleteProxy extends Component
	{
		componentWillUpdate = (next) => {
			if (!next.data && this.props.data) {
				Utils.Redirect(props.successRedirect, this.props.data);
			}
		}

		render = () => {
			if (!this.props.data) {
				return null;
			}
			return React.createElement(component, this.props);
		}
	}

	return connect(mapState, mapDispatch)(DeleteProxy)
}

